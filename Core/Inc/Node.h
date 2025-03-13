/*
 * Node.h
 *
 *  Created on: Aug 3, 2024
 *      Author: harmo
 */

#ifndef SRC_NODE_H_
#define SRC_NODE_H_

#include <cstdint>
#include "InputEditor.h"
class InputEditor;

#define PI  3.14159265358979323846
#define SAMPLING_RATE  24000

class Node;

enum DataType {

	DT_NOTE, // purple
	DT_INTERVAL, // pink
	DT_HERTZ, // blue
	DT_SCALAR, // white
	DT_DECIBEL, // green
	DT_TIME, // cyan
	DT_INT, // red

};

// OUTPUT CLASS

class Output {
private:


	float data = 0; // contained data

public:
	Node* parent = nullptr; //pointer to parent node
	uint16_t x0 = 0, y0 = 0;
	char data_type = DT_SCALAR;
	void set_parent(Node& n); // called on output initialization
	void write(float d); // write data to output
	float read(bool e); // read data from output

	void draw(int x_0, int y_0);


};

// INPUT CLASS

class Input {

public:
	Output* output_ptr = nullptr; // pointer to connected output (nullptr if not connected)
	float constant_data = 0.0f; // constant data for unconnected input
	uint16_t x0 = 0, y0 = 0;
	char data_type = DT_SCALAR;
	bool connected = false; // whether input is connected

	void connect(Output& o); // connect to output o
	void disconnect();
	bool is_connected(Output* o);
	float read(bool e); // if connected, get data from output; if not, return constant data


	void write(float d); // write constant data
	float get_data(); // return constant data without evaluating output

	bool parameterized = true; // whether input is visible
	void draw(int x0, int y0);
	void erase();
};

// NODE CLASS

class Node
{
public:
	virtual int n_inputs() const {return 1;}; // returns number of inputs
	virtual int n_outputs() const {return 1;}; // returns number of outputs
	virtual const char* get_name() const {return "NDE"; };

	bool evaluated = false;

	uint16_t x0, y0, x1, y1;
	Input* inputs = nullptr; // pointer to array of inputs (initialized in derived class constructor)
	Output* outputs = nullptr; // pointer to array of outputs (initialized in derived class constructor)

	static InputEditor* input_editors;
	Node();
	~Node();

	// Signal chain evaluation functions
	virtual void evaluate(bool e);
	bool is_evaluated(bool e) const;

	// GUI functions
	void draw();
	void erase();
	virtual bool poll();

	void edit();
	void drawEditor();
	void pollEditor();
};

#endif /* SRC_NODE_H_ */
