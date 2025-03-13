/*
 * Generator.cpp
 *
 *  Created on: Aug 3, 2024
 *      Author: harmo
 */

#include "Generator.h"

#include <math.h>
#include <random>

/*
 * SINE
INPUTS:
[0] - FREQUENCY
[1] - AMPLITUDE
[2] - SYNCHRONIZE
*/

Sine::Sine()
{
    inputs = new Input[3];
    outputs = new Output[1];
    outputs[0].set_parent(*this);

    inputs[0].write(440.0f * PI / (SAMPLING_RATE/2));
    inputs[0].data_type = DT_HERTZ;

    inputs[1].write(1.0f);
    inputs[1].data_type = DT_DECIBEL;

    inputs[2].parameterized = false;
    inputs[2].data_type =  DT_INT;

}

void Sine::evaluate(bool e) {
    Node::evaluate(e);

    bool sync_new = (inputs[2].read(e) > 0);
    if (sync_new && !sync_previous) {
        phase = 0.0f;
    }
    else {
        float frequency = inputs[0].read(e);
        phase += frequency;
        if (phase > PI) phase -= 2*PI;
        if (phase <= -PI) phase += 2*PI;
    }

    float amplitude = inputs[1].read(e);
    float coefficient = sin(phase);


    sync_previous = sync_new;
    outputs[0].write(amplitude * coefficient);
}

/*
 * TRIANGLE
INPUTS:
[0] - FREQUENCY
[1] - AMPLITUDE
[2] - SYNCHRONIZE
*/

Triangle::Triangle()
{
    inputs = new Input[3];
    outputs = new Output[1];
    outputs[0].set_parent(*this);

    inputs[0].write(440.0f * PI / 24000);
    inputs[0].data_type = DT_HERTZ;

    inputs[1].write(1.0f);
    inputs[1].data_type = DT_DECIBEL;

    inputs[2].parameterized = false;
    inputs[2].data_type = DT_INT;

}

void Triangle::evaluate(bool e) {
    Node::evaluate(e);

    bool sync_new = (inputs[2].read(e) > 0);
    if (sync_new && !sync_previous) {
        phase = 0.0f;
    }
    else {
        float frequency = inputs[0].read(e);
        phase += frequency;
        while (phase > PI) phase -= 2*PI;
        while (phase <= -PI) phase += 2*PI;
    }

    float amplitude = inputs[1].read(e);
    float coefficient = phase * (1/PI);

    sync_previous = sync_new;
    outputs[0].write(amplitude * coefficient);
}

/*
 * SQUARE
INPUTS:
[0] - FREQUENCY
[1] - AMPLITUDE
[2] - SYNCHRONIZE
[3] - DUTY CYCLE
*/

Square::Square()
{
    inputs = new Input[4];
    outputs = new Output[1];
    outputs[0].set_parent(*this);

    inputs[0].write(440.0f * PI / 24000);
    inputs[0].data_type = DT_HERTZ;

    inputs[1].write(1.0f);
    inputs[1].data_type = DT_DECIBEL;

    inputs[2].parameterized = false;
    inputs[2].data_type = DT_INT;

    inputs[3].write(0.5f);

}

void Square::evaluate(bool e) {
    Node::evaluate(e);

    bool sync_new = (inputs[2].read(e) > 0);
    if (sync_new && !sync_previous) {
        phase = 0.0f;
    }
    else {
        float frequency = inputs[0].read(e);
        phase += frequency;
        while (phase > PI) phase -= 2*PI;
        while (phase <= -PI) phase += 2*PI;
    }

    float amplitude = inputs[1].read(e);
    float coefficient = 1;
    if ((phase * (0.5 / PI) + 0.5) > inputs[3].read(e)) coefficient = -1;

    sync_previous = sync_new;
    outputs[0].write(amplitude * coefficient);
}

/*
INPUTS:
[0] - DT (frequency)
[1] - SCALE
[2] - OFFSET
*/

Chaos::Chaos()
{

	inputs = new Input[3];
	outputs = new Output[1];
	outputs[0].set_parent(*this);

	inputs[0].write(1000.0f * PI / 24000);
	inputs[0].data_type = DT_HERTZ;

	inputs[1].write(1.0);
	inputs[1].data_type = DT_DECIBEL;

}

void Chaos::evaluate(bool e)
{
	Node::evaluate(e);

	float time_step = inputs[0].read(e) * (0.75 / (PI * 2));
	if (time_step > 0.15) time_step = 0;

	float envelope = inputs[1].read(e);
	float offset = inputs[2].read(e);

	float dx = (10 * (y - x)) * time_step;
	float dy = (x * (28 - z) - y) * time_step;
	float dz = (x * y - 2.6 * z) * time_step;

	x += dx;
	y += dy;
	z += dz;

	float out = (z - 27.0f) / 36.0f * envelope + offset;
	outputs[0].write(out);

}



/*
INPUTS:
[0] - amplitude
[1] - offset
*/

Noise::Noise()
{
	inputs = new Input[2];
	outputs = new Output[1];
	outputs[0].set_parent(*this);
	inputs[0].write(1.0f);
	inputs[0].data_type = DT_DECIBEL;
	inputs[1].write(0.0f);


}

void Noise::evaluate(bool e)
{
	Node::evaluate(e);

	float amplitude = inputs[0].read(e);
	float offset = inputs[1].read(e);
	float random_f = (float) std::rand() / (INT32_MAX/2) - 1;

	outputs[0].write(random_f * amplitude + offset);
}

/*
INPUTS:
[0] - GATE
[1] - AMPLITUDE
[2] - ATTACK TIME
[3] - RELEASE TIME
*/


Envelope::Envelope()
{
    inputs = new Input[4];
    outputs = new Output[1];
    outputs[0].set_parent(*this);
    inputs[1].write(1.0f);
    inputs[1].data_type = DT_DECIBEL;
    inputs[2].write(0.01f);
    inputs[2].data_type = DT_TIME;
    inputs[3].write(0.5f);
    inputs[3].data_type = DT_TIME;


}

void Envelope::evaluate(bool e)
{
    Node::evaluate(e);

    float gate = inputs[0].read(e);
    bool gate_new = (gate > 0);

    output_value += rate;

    if (gate_new && !gate_old) {
        rate = 1 / (inputs[2].read(e) * SAMPLING_RATE);
        state = 1;
    }

    else if (!gate_new && gate_old) {
        output_value = 1;
        rate = - 1.0f / (inputs[3].read(e) * SAMPLING_RATE);
        state = 0;
    }

    if (state == 1 && output_value > 1) {
        output_value = 1;
        rate = - 1.0f / (inputs[3].read(e) * SAMPLING_RATE);
        state = 0;

    }
    else if (state == 0 && output_value < 0) {
    	rate = 0;
    	output_value = 0;
    }


    float scale = inputs[1].read(e);

    outputs[0].write(output_value * scale);
    gate_old = gate_new;
}

