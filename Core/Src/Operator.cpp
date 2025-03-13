/*
 * Operator.cpp
 *
 *  Created on: Aug 4, 2024
 *      Author: harmo
 */

#include "Operator.h"

/*
 * Constant
INPUTS:
[0 - 4] SIGNALS
*/

Constant::Constant()
{

	inputs = new Input[1];
	outputs = new Output[1];
	outputs[0].set_parent(*this);

}

void Constant::evaluate(bool e)
{
	Node::evaluate(e);
	float out = inputs[0].read(e);
	outputs[0].write(out);
}

/*
 * SUM
INPUTS:
[0 - 4] SIGNALS
*/

Sum::Sum()
{

	inputs = new Input[2];
	outputs = new Output[1];
	outputs[0].set_parent(*this);

}

void Sum::evaluate(bool e)
{
	Node::evaluate(e);

	float out = inputs[0].read(e);
	out += inputs[1].read(e);

	outputs[0].write(out);
}

/*
 * SWITCH
INPUTS:
[0] - CHANNEL 1
[1] - CHANNEL 2
[2] - CONDITION
*/

Switch::Switch()
{
	inputs = new Input[3];
	outputs = new Output[1];
	outputs[0].set_parent(*this);

	inputs[2].data_type = DT_INT;
}

void Switch::evaluate(bool e)
{
	Node::evaluate(e);

	if (inputs[2].read(e) > 0) outputs[0].write(inputs[1].read(e));
	else outputs[0].write(inputs[0].read(e));
}

/*
 * INTERPOLATE
INPUTS:
[0] - CHANNEL 1
[1] - CHANNEL 2
[2] - MIX
*/

Interpolate::Interpolate()
{
	inputs = new Input[3];
	outputs = new Output[1];
	outputs[0].set_parent(*this);
}

void Interpolate::evaluate(bool e)
{
	Node::evaluate(e);

	float mix1 = inputs[2].read(e);
	float mix2 = 1 - mix1;

	float out = mix1 * inputs[0].read(e) + mix2 * inputs[0].read(e);

	outputs[0].write(out);
}


/*
 * MULTIPLY
INPUTS:
[0] - SIGNAL 1
[1] - SIGNAL 2
*/

Multiply::Multiply()
{
	inputs = new Input[2];
	outputs = new Output[1];
	outputs[0].set_parent(*this);
}

void Multiply::evaluate(bool e)
{
	Node::evaluate(e);

	outputs[0].write(inputs[0].read(e) * inputs[1].read(e));
}

/*
 * MULTIPLY
INPUTS:
[0] - SIGNAL 1
[1] - SIGNAL 2
*/

Exponential::Exponential()
{
	inputs = new Input[2];
	outputs = new Output[1];
	outputs[0].set_parent(*this);
}

void Exponential::evaluate(bool e)
{
	Node::evaluate(e);

	//outputs[0].write(inputs[0].read(e) * inputs[1].read(e));
}
