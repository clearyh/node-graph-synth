/*
 * Operator.h
 *
 *  Created on: Aug 4, 2024
 *      Author: harmo
 */

#ifndef SRC_OPERATOR_H_
#define SRC_OPERATOR_H_

#include <Node.h>

class Constant : public Node
{
private:
	int n_inputs() const { return 1; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "c  "; }

public:
	Constant();
	void evaluate(bool e);
};

class Sum : public Node
{

private:
	int n_inputs() const { return 2; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "add"; }

public:
	Sum();
	void evaluate(bool e);

};



class Switch : public Node
{
private:
	int n_inputs() const { return 3; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "sw "; }

public:
	Switch();
	void evaluate(bool e);
};

class Interpolate : public Node
{
private:
	int n_inputs() const { return 3; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "int"; }

public:
	Interpolate();
	void evaluate(bool e);
};



class Multiply : public Node
{
private:
	int n_inputs() const { return 2; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "mul"; }

public:
	Multiply();
	void evaluate(bool e);
};

class Exponential : public Node
{
private:
	int n_inputs() const { return 2; }
	int n_outputs() const { return 1; }
	const char* get_name() const { return "exp"; }

public:
	Exponential();
	void evaluate(bool e);
};



#endif /* SRC_OPERATOR_H_ */
