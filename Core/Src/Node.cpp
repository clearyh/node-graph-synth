/*
 * Node.cpp
 *
 *  Created on: Aug 3, 2024
 *      Author: harmo
 */

#include "Node.h"
#include "lcd.h"
#include "clut.h"
#include "ctp.h"
#include "graph.h"
#include "InputEditor.h"


void Output::set_parent(Node& n)
{
	parent = &n;
}

void Output::write(float d)
{
	data = d;
}

float Output::read(bool e)
{
	if (parent->is_evaluated(e)) return data;
	else parent->evaluate(e);
	return data;
}

void Output::draw(int x_0, int y_0)
{
	x0 = x_0;
	y0 = y_0;
	drawSocket (x0, y0, C_SOCKET);
}

//INPUT CLASS

void Input::connect(Output& o)
{
	connected = true;
	output_ptr = &o;
}

void Input::disconnect()
{
	connected = false;
	output_ptr = nullptr;
}

float Input::read(bool e)
{
	if (connected) return output_ptr->read(e);
	return constant_data;
}

bool Input::is_connected(Output* o)
{
	return (output_ptr == o);
}

void Input::write(float d)
{
	constant_data = d;
}

float Input::get_data()
{
	return constant_data;
}

void Input::draw(int x_0, int y_0)
{
	x0 = x_0;
	y0 = y_0;
	uint8_t c = data_type + 16;
	drawSocket (x0, y0, c);
	if (connected) {
		drawCurve(output_ptr->x0, output_ptr->y0, x0, y0, c);
	}
}

void Input::erase() {
	if (connected) {
		drawCurve(output_ptr->x0, output_ptr->y0, x0, y0, C_BACKGROUND);
	}
}

// NODE CLASS

Node::Node()
{
	x0 = 10;
	x1 = 60;
	y0 = 10;
}

Node::~Node()
{
	delete[] inputs;
	delete[] outputs;
}

void Node::evaluate(bool e)
{
	evaluated = e;
}

bool Node::is_evaluated(bool e) const
{
	return (evaluated == e);
}

#define SOCKET_SPACING 18
#define TITLE_SPACING 14

void Node::draw()
{
	int param_counter = 0;
	for (int i = 0; i < this->n_inputs(); i++) {
		if (inputs[i].parameterized) {
			param_counter++;
		}
	}
	if (param_counter < 1) param_counter = 1;

	y1 = param_counter * SOCKET_SPACING + y0 + TITLE_SPACING + 4;
	x1 = x0 + 55;
	//draw frame
	drawFrame(x0, y0, x1, y1, C_NODE_FILL, C_NODE_FRAME_N);

	//draw title
	drawString(x0 + 18, y0 + 1, get_name(), 3, C_TEXT_TITLE);

	//draw delete button
	drawSymbol(x0, y0, I_DELETE_NODE, C_DELETE_SYMBOL);

	//draw edit button
	drawSymbol(x1 - 16, y0, I_EDIT_NODE, C_EDIT_SYMBOL);

	param_counter = 0;
	for (int i = 0; i < this->n_inputs(); i++) { // draw output sockets
		if (inputs[i].parameterized) {
			inputs[i].draw(x0, y0 + 26 + param_counter * SOCKET_SPACING);
			param_counter++;

		}
	}

	for (int i = 0; i < this->n_outputs(); i++) { // draw output sockets
		outputs[i].draw(x1 - 1, y0 + 26 + i * SOCKET_SPACING);
	}
}


bool Node::poll() {
	int tx = getTouchX();
	if (tx < x0 || tx > x1) return false;
	int ty = getTouchY();
	if (ty < y0 || ty > y1) return false;
	if (ty < y0 + TITLE_SPACING) {
		if (tx < x0 + TITLE_SPACING) {
			deleteNode(this);
			return true;
		}
		if (tx > x1 - TITLE_SPACING) { // MOVE NODE
			this->edit();
		}
		else { // MOVE NODE
			while (getTouchF() == true) {
				updateTouch();
			}
			int dx = getTouchX() - tx;
			int dy = getTouchY() - ty;
			uint16_t x0_temp = x0 - 2;
			uint16_t y0_temp = y0;
			uint16_t x1_temp = x1 + 2;
			uint16_t y1_temp = y1;
			x0 += dx;
			if (x0 > 480) x0 = 0;
			y0 += dy;
			if (y0 > 272) y0 = 0;
			for (int i = 0; i < n_inputs(); i++) {
				inputs[i].erase();
			}
			eraseOutput(&outputs[0]);
			this->draw();
			redrawGraphSection(x0_temp, y0_temp, x1_temp, y1_temp);
			redrawConnections();
			return true;
		}
	}
	else {
		if (tx > x1 - SOCKET_SPACING) { // CONNECT AN OUTPUT
			connection(&outputs[0]);
			return true;
		}
	}
	return false;
}

InputEditor* Node::input_editors = nullptr;

#define INPUT_EDITOR_SPACING 26
#define NODE_EDITOR_X0 150
#define NODE_EDITOR_X1 330

void Node::edit() {
	int n = this->n_inputs();

	input_editors = new InputEditor[n];

	//draw frame
	int height = n * INPUT_EDITOR_SPACING / 2;

	int y_frame_start = 132 - height;
	int y_frame_stop = 136 + height;


	drawFrame(NODE_EDITOR_X0, y_frame_start, NODE_EDITOR_X1, y_frame_stop, C_NODE_FILL, C_NODE_FRAME_S);

	for (int i = 0; i < n; i++) {
		input_editors[i].y0 = 136 - height + i * INPUT_EDITOR_SPACING;
		input_editors[i].y1 = input_editors[i].y0 + 22;
		input_editors[i].input_ptr = &inputs[i];
		input_editors[i].draw();
	}

	int tx, ty;

	while (getTouchF()) {
		updateTouch();
	}
	while (true) {
		updateTouch();
		if (getTouchF()) {
			tx = getTouchX();
			ty = getTouchY();
			if (tx < NODE_EDITOR_X0 - 5 || tx > NODE_EDITOR_X1 || ty < y_frame_start || ty > y_frame_stop) {
				delete[] input_editors;

				redrawGraphSection( NODE_EDITOR_X0 - 5, y_frame_start, NODE_EDITOR_X1, y_frame_stop);
				redrawGraphSection(x0 - 2, y0, x1 + 2, y1);
				this->draw();
				redrawConnections();

				return;
			}
			if (tx < 240) {
				for (int i = 0; i < n; i++) {
					if(input_editors[i].poll()) i = n;

				}
			}
		}
	}
}
