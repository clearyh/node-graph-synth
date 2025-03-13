/*
 * Hardware.h
 *
 *  Created on: Aug 4, 2024
 *      Author: harmo
 */

#ifndef SRC_HARDWARE_H_
#define SRC_HARDWARE_H_

#include <Node.h>

struct shared_data
{
	uint8_t button_state[8];
	uint8_t button_state_t[8];
	float dial_state[8];
};

volatile struct shared_data * const xfr_ptr = (struct shared_data *)0x38001000;

class Dial : public Node
{
private:
	int n_inputs() const { return 3; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "d/"; }

public:
	Dial();
	void evaluate(bool e);
};



class Button : public Node
{
private:
	int n_inputs() const { return 3; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "b/"; }


public:
	Button();
	void evaluate(bool e);
};



class Keyboard : public Node
{
private:
	int n_inputs() const { return 8; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "kb/"; }
	uint8_t key_tracker = 0;

public:
	Keyboard();
	void evaluate(bool e);
};



class Sequencer : public Node
{
private:
	int n_inputs() const { return 10; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "seq"; }
	float phase = 0.0f; // phase varies from -PI to PI
	uint8_t step_no = 1;

public:
	Sequencer();
	void evaluate(bool e);
};

class Touch0D : public Node
{
private:
	int n_inputs() const { return 2; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "t0d"; }
	bool mix = false;



public:
	Touch0D();
	void evaluate(bool e);
	bool poll();
};

class Touch1D : public Node
{
private:
	int n_inputs() const { return 2; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "t1d"; }
	float mix = 0.0f;



public:
	Touch1D();
	void evaluate(bool e);
	bool poll();
};




#endif /* SRC_HARDWARE_H_ */
