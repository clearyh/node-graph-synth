/*
 * Generator.h
 *
 *  Created on: Aug 3, 2024
 *      Author: harmo
 */

#ifndef SRC_GENERATOR_H_
#define SRC_GENERATOR_H_

#include <Node.h>

class Sine : public Node
{
private:
	int n_inputs() const { return 3; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "sin"; }

	float phase = 0.0f; // phase varies from -PI to PI

	bool sync_previous = false;

public:
	Sine();
	void evaluate(bool e);
};


class Triangle : public Node
{
private:
	int n_inputs() const { return 3; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "tri"; }

	float phase = 0.0f; // phase varies from -PI to PI

	bool sync_previous = false;
public:
	Triangle();
	void evaluate(bool e);
};

class Square : public Node
{
private:
	int n_inputs() const { return 4; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "squ"; }

	float phase = 0.0f; // phase varies from -PI to PI

	bool sync_previous = false;

public:
	Square();
	void evaluate(bool e);
};


class Chaos : public Node
{
private:
	int n_inputs() const { return 2; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "cha"; }

	float x = 0.1f;
	float y = 0.25f;
	float z = 25.0f;

public:
	Chaos();
	void evaluate(bool e);
};



class Noise : public Node
{
private:
	int n_inputs() const { return 2; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "wgn"; }

public:
	Noise();
	void evaluate(bool e);
};


class Envelope : public Node
{
private:
	int n_inputs() const { return 4; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "env"; }

	char state = 0; // 0 off | 1 attack | 2 decay | 3 release
	bool gate_old = false;
	float output_value = 0;
	float rate = 0;

public:
	Envelope();
	void evaluate(bool e);
};




#endif /* SRC_GENERATOR_H_ */
