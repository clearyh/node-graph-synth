/*
 * Hardware.cpp
 *
 *  Created on: Aug 4, 2024
 *      Author: harmo
 */

#include "Hardware.h"
#include "ctp.h"

/*
 * DIAL
INPUTS:
[0] - Dial No.
[1] - CCW
[2] - CW
 */

Dial::Dial()
{

	inputs = new Input[3];
	outputs = new Output[1];
	outputs[0].set_parent(*this);
	inputs[0].write(1.0f);
	inputs[0].data_type = DT_INT;

	inputs[1].write(0.0f);
	inputs[2].write(1.0f);

}

void Dial::evaluate(bool e)
{
	Node::evaluate(e);
	int dial_no = (int) inputs[0].read(e);
	float ccw = inputs[2].read(e);
	float cw = inputs[1].read(e);
	if (dial_no > 8 || dial_no < 1) dial_no = 1;
	float d_state = xfr_ptr->dial_state[dial_no - 1];
	float out = ccw + d_state * (cw - ccw);

	outputs[0].write(out);

}

/*
 * BUTTON
INPUTS:
[0] - Button no.
[0] - OFF
[1] - ON
 */

Button::Button()
{

	inputs = new Input[3];
	outputs = new Output[1];
	outputs[0].set_parent(*this);
	inputs[0].write(1.0f);
	inputs[0].data_type = DT_INT;

	inputs[1].write(0.0f);
	inputs[2].write(1.0f);

}

void Button::evaluate(bool e)
{
	Node::evaluate(e);

	int btn_no = (int) inputs[0].read(e);
	if (btn_no > 8 || btn_no < 1) btn_no = 1;
	uint8_t b_state = xfr_ptr->button_state[btn_no - 1];

	if (b_state > 0) outputs[0].write(inputs[2].read(e));
	else outputs[0].write(inputs[1].read(e));

}

/*
 * Keyboard
INPUTS:
[1-8] - Key
 */

Keyboard::Keyboard()
{

	inputs = new Input[8];
	outputs = new Output[1];
	outputs[0].set_parent(*this);

	inputs[0].write(220.0f * PI / (SAMPLING_RATE/2));
	inputs[1].write(247.0f * PI / (SAMPLING_RATE/2));
	inputs[2].write(261.6f * PI / (SAMPLING_RATE/2));
	inputs[3].write(293.6f * PI / (SAMPLING_RATE/2));
	inputs[4].write(329.6f * PI / (SAMPLING_RATE/2));
	inputs[5].write(349.2f * PI / (SAMPLING_RATE/2));
	inputs[6].write(392.0f * PI / (SAMPLING_RATE/2));
	inputs[7].write(440.0f * PI / (SAMPLING_RATE/2));

	for(int i = 0; i < 8; i++) {
		inputs[i].data_type = DT_NOTE;
		inputs[i].parameterized = false;
	}

}

void Keyboard::evaluate(bool e)
{
	Node::evaluate(e);

	if (key_tracker) {
		uint8_t b_state1 = xfr_ptr->button_state[key_tracker - 1];
		if (!b_state1) {
			key_tracker = 0;
		}
	}
	else {
		for (int i = 0; i < 8; i++) {
			uint8_t b_state1 = xfr_ptr->button_state[i];
			if (b_state1 && (inputs[i].constant_data > 0)) key_tracker = i + 1;
		}
	}
	float out;
	if (key_tracker) {
		out = inputs[key_tracker - 1].read(e);
	}
	else out = 0.0f;
	outputs[0].write(out);
}


/*
 * Sequencer
INPUTS:
[0] - trigger
[1] - length
[1-8] - Key
 */

Sequencer::Sequencer()
{

	inputs = new Input[10];
	outputs = new Output[1];
	outputs[0].set_parent(*this);

	inputs[0].write(1.0f * PI / (SAMPLING_RATE/2));
	inputs[0].data_type = DT_HERTZ;

	inputs[1].write(8.0f);
	inputs[1].data_type = DT_INT;

	inputs[2].write(261.6f * PI / (SAMPLING_RATE/2));
	inputs[3].write(293.6f * PI / (SAMPLING_RATE/2));
	inputs[4].write(329.6f * PI / (SAMPLING_RATE/2));
	inputs[5].write(349.2f * PI / (SAMPLING_RATE/2));
	inputs[6].write(392.0f * PI / (SAMPLING_RATE/2));
	inputs[7].write(440.0f * PI / (SAMPLING_RATE/2));
	inputs[8].write(220.0f * PI / (SAMPLING_RATE/2));
	inputs[9].write(247.0f * PI / (SAMPLING_RATE/2));

	for(int i = 2; i < 10; i++) {
		inputs[i].data_type = DT_NOTE;
		inputs[i].parameterized = false;
	}

}

void Sequencer::evaluate(bool e)
{
	Node::evaluate(e);

	float frequency = inputs[0].read(e);
	if (phase <= 0) {
		phase += frequency;
		if (phase > PI) phase -= 2*PI;
		if (phase <= -PI) phase += 2*PI;
		if (phase > 0) {
			step_no++;
			if (step_no > (int) inputs[1].read(e) - 1 || step_no > 7) {
				step_no = 0;
			}
			outputs[0].write(0.0f);
		}
	}

	else {
		phase += frequency;
		if (phase > PI) phase -= 2*PI;
		if (phase <= -PI) phase += 2*PI;
		uint8_t b_state1 = xfr_ptr->button_state_t[step_no];
		float out;
		if (b_state1) out = inputs[step_no + 2].read(e);
		else out = 0.0f;

		outputs[0].write(out);
	}

}


Touch0D::Touch0D()
{

	inputs = new Input[2];
	outputs = new Output[1];
	outputs[0].set_parent(*this);
	inputs[0].write(0.0f);
	inputs[1].write(1.0f);

}

void Touch0D::evaluate(bool e)
{
	Node::evaluate(e);
	float out;
	if (mix) out = inputs[1]. read(e);
	else out = inputs[0].read(e);
	outputs[0].write(out);
}

bool Touch0D::poll() {
	if (!getTouchF()) {
		mix = false;
		return false;
	}
	uint16_t tx = getTouchX();
	uint16_t ty = getTouchY();
	bool ret = Node::poll();
	if ((!ret) && (tx > x0) && (tx < x1) && (ty > y0) && (ty < y1)) {
		mix = true;
	}
	else mix = false;
	return ret;
}



Touch1D::Touch1D()
{

	inputs = new Input[2];
	outputs = new Output[1];
	outputs[0].set_parent(*this);
	inputs[0].write(0.0f);
	inputs[1].write(1.0f);

}

void Touch1D::evaluate(bool e)
{
	Node::evaluate(e);

	float s1 = inputs[0].read(e);
	float s2 = inputs[1]. read(e);
	float out = s1 * (1 - mix) + s2 * mix;
	outputs[0].write(out);
}

bool Touch1D::poll() {
	uint16_t tx = getTouchX();
	uint16_t ty = getTouchY();
	bool ret = Node::poll();
	if ((!ret) && (tx > x0 + 5) && (tx < x1 - 5) && (ty > y0 + 20) && (ty < y1 - 10)) {
		uint16_t x_offset = tx - x0 - 5;
		mix = 1.0 * x_offset / 40.0f;
		return true;
	}
	return ret;
}


