/*
 * Filter.cpp
 *
 *  Created on: Aug 3, 2024
 *      Author: harmo
 */

#include<math.h>
#include <Modulator.h>

/*
 * HIGH PASS FILTER
INPUTS:
[0] - SIGNAL
[1] - CUTOFF FREQUENCY
*/

HighPassFilter::HighPassFilter()
{
    inputs = new Input[2];
    outputs = new Output[1];
    outputs[0].set_parent(*this);

    inputs[1].write(1000.0f * PI / 24000.0f);
    inputs[1].data_type = DT_HERTZ;
}

void HighPassFilter::evaluate(bool e)
{
    Node::evaluate(e);

    float cutoff_frequency = fabsf(inputs[1].read(e));
    float coefficient = cutoff_frequency / (cutoff_frequency + 1);
    float signal = inputs[0].read(e);

    float out_new = (1 - coefficient) * (signal - in_previous + out_previous);

    outputs[0].write(out_new);
    out_previous = out_new;
    in_previous = signal;
}

/*
 * LOW PASS FILTER
INPUTS:
[0] - SIGNAL
[1] - CUTOFF FREQUENCY
*/

LowPassFilter::LowPassFilter()
{
    inputs = new Input[2];
    outputs = new Output[1];
    outputs[0].set_parent(*this);
    inputs[1].write(1000.0f * PI / 24000.0f);
    inputs[1].data_type = DT_HERTZ;
}

void LowPassFilter::evaluate(bool e)
{
    Node::evaluate(e);

    float signal = inputs[0].read(e);
    float cutoff_frequency = fabsf(inputs[1].read(e));
    float coefficient = cutoff_frequency / (cutoff_frequency + 1);

    float out_new = coefficient * signal + (1 - coefficient) * out_previous;

    outputs[0].write(out_new);
    out_previous = out_new;
    in_previous = signal;

}

/*
 * RESONANT FILTER
INPUTS:
[0] - SIGNAL
[1] - RESONANT FREQUENCY
[2] - RESONANCE (pole radius)
*/

ResonantFilter::ResonantFilter()
{
	inputs = new Input[3];
	outputs = new Output[1];
	outputs[0].set_parent(*this);
	inputs[1].write(1000.0f * PI / 24000);
	inputs[1].data_type = DT_HERTZ;

	inputs[2].write(0.8);
}

void ResonantFilter::evaluate(bool e)
{
	Node::evaluate(e);

	float signal = inputs[0].read(e);
	float f_r = inputs[1].read(e);
	float R = fabsf(inputs[2].read(e));
	if (R >= 0.9999) R = 0.9999;

	float a1 = -2.0f * R * cos(f_r);
	float a2 = R * R;

	if (normalized) signal *= (1 - a2) / 2;

	float out_new = signal - in_z2;

	in_z2 = in_z1;
	in_z1 = signal;

	out_new +=  - a1 * out_z1 - a2 * out_z2;

	out_z2 = out_z1;
	out_z1 = out_new;

	outputs[0].write(out_new);
}


/*
 * NOTCH FILTER
INPUTS:
[0] - SIGNAL
[1] - NOTCH FREQUENCY
[2] - POLE RADIUS
*/

NotchFilter::NotchFilter()
{
	inputs = new Input[3];
	outputs = new Output[1];
	outputs[0].set_parent(*this);
	inputs[1].write(1000.0f * PI / 24000);
	inputs[1].data_type = DT_HERTZ;

	inputs[2].write(0.8);

}

void NotchFilter::evaluate(bool e)
{
	Node::evaluate(e);

	float signal = inputs[0].read(e);
	float f_r = inputs[1].read(e);
	float R = fabsf(inputs[2].read(e));
	if (R >= 1) R = 1;

	float a1 = 2.0f * cos(f_r);
	float a2 = R * R;

	if (normalized) {
		float magnitude_reciprocal = (1 + a2)/2;
		signal *= magnitude_reciprocal;
	}

	float out_new = signal - a1 * in_z1 + in_z2;

	in_z2 = in_z1;
	in_z1 = signal;

	out_new += R * a1 * out_z1 - a2 * out_z2 ;

	out_z2 = out_z1;
	out_z1 = out_new;

	outputs[0].write(out_new);


}

/*
 * DELAY
INPUTS:
[0] - SIGNAL
[1] - DELAY TIME
[2] - FEEDBACK GAIN
[3] - MIX DRY/WET
*/



Delay::Delay()
{
	for (int i = 0; i < MAX_DELAY_SAMPLES; i++) {
		delay_buffer[i] = 0.0f;
	}

	inputs = new Input[4];
	outputs = new Output[1];
	outputs[0].set_parent(*this);
	inputs[1].write(0.1f);
	inputs[1].data_type = DT_TIME;
	inputs[2].write(0.8f);
	inputs[3].write(1);
}

void Delay::evaluate(bool e)
{
	Node::evaluate(e);

	index++;
	if (index == MAX_DELAY_SAMPLES) index = 0;

	int n_delay_samples = (int)(inputs[1].read(e) * SAMPLING_RATE);

	int delay_index = index - n_delay_samples;
	while (delay_index < 0) delay_index += MAX_DELAY_SAMPLES;
	while (delay_index >= MAX_DELAY_SAMPLES) delay_index -= MAX_DELAY_SAMPLES;

	float delay_output = delay_buffer[delay_index];
	float fb_output =  delay_output * inputs[2].read(e);
	float signal = inputs[0].read(e);

	delay_buffer[index] = signal + fb_output;

	float mix = inputs[3].read(e);

	float wet_signal = delay_output * mix;
	float dry_signal = signal * (1 - mix);

	outputs[0].write(wet_signal + dry_signal);
}

/*
 * QUANTIZE
INPUTS:
[0] - SIGNAL
[1] - BITSHIFT
*/

Quantize::Quantize()
{
	inputs = new Input[2];
	outputs = new Output[1];
	outputs[0].set_parent(*this);
}

void Quantize::evaluate(bool e)
{
	Node::evaluate(e);

	float signal = inputs[0].read(e);
	float bitshift_fp = inputs[1].read(e);
	if (bitshift_fp < 0) bitshift_fp = 0;
	if (bitshift_fp > 1) bitshift_fp = 1;
	bitshift_fp *= 15;
	int bit_shift = (int) (bitshift_fp);

	if (signal == 0.0f) {
		outputs[0].write(0.0f);
		return;
	}
	int q = (int) (signal * 32767);
	q >>= bit_shift;
	float r = ((float) q) + 0.5;
	outputs[0].write(r / (1 << (15 - bit_shift)));

}

/*
 * SATURATE
INPUTS:
[0] - SIGNAL
[1] - THRESHOLD
[2] - GAIN
*/

Saturate::Saturate()
{

	inputs = new Input[3];
	outputs = new Output[1];
	outputs[0].set_parent(*this);

	inputs[1].write(1.0f);
	inputs[1].data_type = DT_DECIBEL;

	inputs[2].write(1.0f);
	inputs[2].data_type = DT_DECIBEL;

}

void Saturate::evaluate(bool e)
{
	Node::evaluate(e);

	float signal = inputs[0].read(e) * inputs[1].read(e);
	float limit = inputs[2].read(e);

	if (limit == 0.0f) {
		outputs[0].write(0.0f);
		return;
	}

	if (mode) {

		if (signal > limit) signal = limit;
		if (signal < -limit) signal = -limit;
		outputs[0].write(signal);
		return;
	}
	else {
		float out = limit * tanh(signal / limit);
		outputs[0].write(out);
		return;
	}
}

/*
 * SAMPLE AND HOLD
INPUTS:
[0] - SIGNAL
[1] - THRESHOLD
[2] - GAIN
*/

SampleHold::SampleHold()
{
	inputs = new Input[2];
	outputs = new Output[1];
	outputs[0].set_parent(*this);

	inputs[1].data_type = DT_INT;
}

void SampleHold::evaluate(bool e)
{
	Node::evaluate(e);

	float out = inputs[0].read(e);

	if (inputs[1].read(e) <= 0) outputs[0].write(out);
}
