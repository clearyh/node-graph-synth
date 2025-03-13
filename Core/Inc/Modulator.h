/*
 * Filter.h
 *
 *  Created on: Aug 3, 2024
 *      Author: harmo
 */

#ifndef SRC_FILTER_H_
#define SRC_FILTER_H_

#include <Node.h>

class HighPassFilter : public Node
{
	int n_inputs() const { return 2; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "hpf"; }

	float in_previous = 0.0f;
	float out_previous = 0.0f;

public:
	HighPassFilter();
	void evaluate(bool e);
};


class LowPassFilter : public Node
{
	int n_inputs() const { return 2; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "lpf"; }

	float in_previous = 0.0f;
	float out_previous = 0.0f;

public:
	LowPassFilter();
	void evaluate(bool e);
};

class ResonantFilter : public Node
{
private:

	int n_inputs() const { return 3; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "bpf"; }

	bool normalized = true;

	float in_z2 = 0.0f;
	float in_z1 = 0.0f;
	float out_z2 = 0.0f;
	float out_z1 = 0.0f;

public:

	ResonantFilter();
	void evaluate(bool e);
};

class NotchFilter : public Node
{
private:

	int n_inputs() const { return 3; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "bsf"; }

	bool normalized = true;

	float in_z2 = 0.0f;
	float in_z1 = 0.0f;
	float out_z2 = 0.0f;
	float out_z1 = 0.0f;

public:

	NotchFilter();
	void evaluate(bool e);
};

#define MAX_DELAY_SAMPLES 12000

class Delay : public Node
{
private:
	int n_inputs() const { return 4; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "dly"; }

	int index = 0;

	float delay_buffer[MAX_DELAY_SAMPLES];

public:
	Delay();
	void evaluate(bool e);

};



class Quantize : public Node
{
private:
	int n_inputs() const { return 2; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "qtz"; }

public:
	Quantize();
	void evaluate(bool e);
};



class Saturate : public Node
{
private:
	int n_inputs() const { return 3; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "sat"; }

	bool mode = false;

public:
	Saturate();
	void evaluate(bool e);
};

class SampleHold : public Node
{
private:
	int n_inputs() const { return 2; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "s+h"; }

public:
	SampleHold();
	void evaluate(bool e);
};


#endif /* SRC_FILTER_H_ */
