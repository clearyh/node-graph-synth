/*
 * Graph.cpp
 *
 *  Created on: Dec 29, 2024
 *      Author: harmo
 */


#include "graph.h"
#include <vector>

#include "main.h"
#include "lcd.h"
#include "ctp.h"
#include "Generator.h"
#include "Modulator.h"
#include "Operator.h"
#include "Hardware.h"
#include <random>
#include <cstdio>
#include "../../Drivers/STM32H745I-DISCO/stm32h745i_discovery_audio.h"

std::vector<Node*> node_vector;
int node_counter = 0;

bool play_state = true;
bool eval_ff = false;

#define FINAL_INPUT_X 474
#define FINAL_INPUT_Y 136

Input final_input;

#define N_ANIM 100

void drawCurvePoint(uint16_t x0, uint16_t y0, uint16_t t, uint8_t c) {
	float w = 1.0 * FINAL_INPUT_X - x0;
	float h = 1.0 * FINAL_INPUT_Y - y0;
	float k = -6.0 * h / (w * w * w);
	float x = 1.0 * t;
	float fx = (1.0 * x * x * x / 3) - (w * x * x / 2);
	fx *= k;
	int16_t dx = t;
	int16_t dy = (int16_t) fx;
	drawPixel(dx + x0, dy + y0, c);
}

void startupAnimation() {
	drawGraph();
	HAL_Delay(20);
	uint16_t x[N_ANIM];
	uint16_t y[N_ANIM];
	uint16_t dt[N_ANIM];
	uint16_t c[N_ANIM];
	std::srand(128);
	for (int i = 0; i < N_ANIM; i++) {
		x[i] = std::rand() & 0xFF;
		y[i] = (std::rand() & 0xFF) + 8;
		dt[i] = std::rand() & 0x7F;
		c[i] = (std::rand() % 8) + 16;
	}

	for (int t = 0; t < 680; t++) {
		for (int i = 0; i < N_ANIM; i++) {
			if (FINAL_INPUT_X - x[i] > (t - dt[i]) && (t - dt[i]) > 0)
			drawCurvePoint(x[i], y[i], (t- dt[i]), c[i]);
			int t_d = t - dt[i] - 50;
			if (t_d >= 0 && FINAL_INPUT_X - x[i] > t_d )
			drawCurvePoint(x[i], y[i], t_d, C_BACKGROUND);
		}
		HAL_Delay(1);
	}
	drawFrame(470, 132, 479, 141, C_BACKGROUND, C_SOCKET);
	final_input.draw(FINAL_INPUT_X, FINAL_INPUT_Y);
}

void initializeGraph() {

	Node* node0 = new Delay();

	node0->x0 = 410;
	node0->y0 = 110;
	node0->inputs[1].write(0.3f);
	node0->inputs[3].write(0.95f);
	final_input.connect(node0->outputs[0]);

	Node* node1 = new ResonantFilter();

	node1->x0 = 345;
	node1->y0 = 60;
	node0->inputs[0].connect(node1->outputs[0]);

	Node* node2 = new Dial();

	node2->x0 = 345;
	node2->y0 = 140;
	node2->inputs[0].write(2.0f);
	node0->inputs[2].connect(node2->outputs[0]);

	Node* node3 = new Dial();

	node3->x0 = 280;
	node3->y0 = 140;
	node1->inputs[1].connect(node3->outputs[0]);

	Node* node4 = new Triangle();

	node4->x0 = 215;
	node4->y0 = 80;
	node1->inputs[0].connect(node4->outputs[0]);

	Node* node5 = new Envelope();

	node5->x0 = 150;
	node5->y0 = 150;
	node5->inputs[1].write(4.0f);
	node5->inputs[3].write(0.3f);
	node4->inputs[1].connect(node5->outputs[0]);

	Node* node6 = new Keyboard();

	node6->x0 = 85;
	node6->y0 = 100;
	node4->inputs[0].connect(node6->outputs[0]);
	node5->inputs[0].connect(node6->outputs[0]);


	node_vector.push_back(node6);
	node_vector.push_back(node5);
	node_vector.push_back(node4);
	node_vector.push_back(node3);
	node_vector.push_back(node2);
	node_vector.push_back(node1);
	node_vector.push_back(node0);
	node_counter = 7;

	drawGraph();
}

void drawGraph() {
	drawRect(0,0,480,272, C_BACKGROUND);
	for (Node* n : node_vector) {
		n->draw();
	}
	//draw add node button
	drawFrame(450, 2, 478, 20, C_BACKGROUND, C_SYMBOL);
	drawSymbol(456, 3, I_ADD_NODE, C_SYMBOL);

	//draw play/pause button
	drawFrame(420, 2, 448, 20, C_BACKGROUND, C_SYMBOL);
	if (play_state)drawSymbol(426, 3, I_PAUSE, C_SYMBOL);
	else drawSymbol(426, 3, I_PLAY, C_SYMBOL);

	//draw plot
	drawFrame(390, 2, 418, 20, C_BACKGROUND, C_SYMBOL);
	drawSymbol(396, 3, I_PLOT, C_SYMBOL);

	/*//draw darkmode symbol
	drawFrame(2, 2, 30, 20, C_BACKGROUND, C_SYMBOL);
	drawSymbol(8, 3, I_DARKMODE, C_SYMBOL);*/

	drawFrame(470, 132, 479, 141, C_BACKGROUND, C_SOCKET);
	final_input.draw(FINAL_INPUT_X, FINAL_INPUT_Y);
}

void redrawGraphSection(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	if (x0 > 480) x0 = 0;
	if (y0 > 272) y0 = 0;
	if (x1 > 390 && y0 < 20) {
		drawFrame(450, 2, 478, 20, C_BACKGROUND, C_SYMBOL);
		drawSymbol(456, 3, I_ADD_NODE, C_SYMBOL);
		drawFrame(420, 2, 448, 20, C_BACKGROUND, C_SYMBOL);
		if (play_state)drawSymbol(426, 3, I_PAUSE, C_SYMBOL);
		else drawSymbol(426, 3, I_PLAY, C_SYMBOL);
		drawFrame(390, 2, 418, 20, C_BACKGROUND, C_SYMBOL);
		drawSymbol(396, 3, I_PLOT, C_SYMBOL);
	}
	drawRect(x0,y0,x1,y1, C_BACKGROUND);
	for (Node* n : node_vector) {
			if ((n->x0 < x1 && n->x1 > x0) && (n->y0 < y1 && n->y1 > y0))
				n->draw();
	}
}

void pollGraph() {
	if (!getTouchF()) return;
	uint16_t tx = getTouchX();
	uint16_t ty = getTouchY();
	if (ty < 21) {
		/*if (tx < 30) {
			darkmode = !darkmode;
			drawGraph();
		}*/
		if (tx > 450){
			addNode();
			return;
		}
		else if (tx > 420) {
			drawFrame(420, 2, 448, 20, C_BACKGROUND, C_SYMBOL);
			if (play_state) {
				pause_audio();
				drawSymbol(426, 3, I_PLAY, C_SYMBOL);

			}
			else {
				play_audio();
				drawSymbol(426, 3, I_PAUSE, C_SYMBOL);
			}
			play_state = !play_state;
			while (getTouchF()) {
				updateTouch();
			}
			return;
		}
		else if (tx > 390) {
			drawSignalPlot();
			return;
		}
	}
	for (Node* n : node_vector) {
			n->poll();
	}
}



const char* inp_node_names[8] = {
		"button/",
		"dial/",
		"keyboard/",
		"sequencer/",

		"touch 0d/",
		"touch 1d/",

};

const char* gen_node_names[8] = {
		"sine/",
		"triangle/",
		"square/",
		"chaos/",
		"noise/",
		"envelope/",
};

const char* mod_node_names[8] = {

		"high pass/",
		"low pass/",
		"band pass/",
		"band stop/",


		"delay/",
		"sample+hold/"
};
const char* utl_node_names[8] = {

		"switch/",
		"interpolate/",
		"constant/",
		"add/",
		"multiply/",
		"exponential/",

		"quantize/",
		"saturate/",
};



void addNode() {
	if (node_counter > 18) {
		drawSymbol(456, 3, I_ADD_NODE, C_SYMBOL_ERROR);
		return;
	}
	drawFrame(80, 32, 400, 232, C_NODE_FILL, C_NODE_FRAME_S);
	drawString(212, 38, "add node", 8, C_TEXT_TITLE);
	for(int i = 0; i < 6; i++) {
		drawString(96, 60 + i * 20, inp_node_names[i], 12, C_NC_INP);
	}
	for(int i = 0; i < 6; i++) {
		drawString(171, 60 + i * 20, gen_node_names[i], 12, C_NC_GEN);
	}
	for(int i = 0; i < 6; i++) {
		drawString(251, 60 + i * 20, mod_node_names[i], 12, C_NC_MOD);
	}
	for(int i = 0; i < 8; i++) {
		drawString(326, 60 + i * 20, utl_node_names[i], 12, C_NC_UTL);
	}
	while (getTouchF() == true) {
		updateTouch();
	}
	while (getTouchF() == false) {
		updateTouch();
	}
	uint16_t tx = getTouchX();
	uint16_t ty = getTouchY();

	if (tx > 80 && tx < 400 && ty > 60 && ty < 220) {
		int grid_n = (ty - 60) / 20;
		if (tx > 165) grid_n += 8;
		if (tx > 240) grid_n += 8;
		if (tx > 315) grid_n += 8;
		Node* added_node = nullptr;

		switch (grid_n) {
		case 0:
			added_node = new Button();
			break;
		case 1:
			added_node = new Dial();
			break;
		case 2:
			added_node = new Keyboard();
			break;
		case 3:
			added_node = new Sequencer();
			break;
		case 4:
			added_node = new Touch0D();
			break;
		case 5:
			added_node = new Touch1D();
			break;
		case 8:
			added_node = new Sine();
			break;
		case 9:
			added_node = new Triangle();
			break;
		case 10:
			added_node = new Square();
			break;
		case 11:
			added_node = new Chaos();
			break;
		case 12:
			added_node = new Noise();
			break;
		case 13:
			added_node = new Envelope();
			break;
		case 16:
			added_node = new HighPassFilter();
			break;
		case 17:
			added_node = new LowPassFilter();
			break;
		case 18:
			added_node = new ResonantFilter();
			break;
		case 19:
			added_node = new NotchFilter();
			break;

		case 22:
			added_node = new Delay();
			break;
		case 23:
			added_node = new SampleHold();
			break;
		case 24:
			added_node = new Switch();
			break;
		case 25:
			added_node = new Interpolate();
			break;
		case 26:
			added_node = new Constant();
			break;
		case 27:
			added_node = new Sum();
			break;
		case 28:
			added_node = new Multiply();
			break;
		case 29:
			added_node = new Exponential();
			break;
		case 30:
			added_node = new Quantize();
			break;
		case 31:
			added_node = new Saturate();
			break;
		}


		if (added_node != nullptr) {
			node_vector.push_back(added_node);
			node_counter++;
		}
	}
	drawGraph();
}

void deleteNode (Node* n) {
	for (Node* a : node_vector) {
		for (int i = 0; i < a->n_inputs(); i++) {
			if (a->inputs[i].output_ptr->parent == n){
				a->inputs[i].erase();
				a->inputs[i].disconnect();
			}
		}
	}
	if (final_input.output_ptr->parent == n){
		final_input.erase();
		final_input.disconnect();
	}
	for (int i = 0; i < n->n_inputs(); i++) {
		if (n->inputs[i].connected) {
			n->inputs[i].erase();
			n->inputs[i].disconnect();
		}
	}
	for (int i = 0; i < node_counter; i++) {
		if (node_vector[i] == n) {
			node_vector.erase(node_vector.begin() + i);
		}
	}
	redrawGraphSection(n->x0 - 2, n->y0, n->x1 + 2, n->y1);
	delete n;
	node_counter--;
}

void drawSignalPlot() {
	drawRect(0, 223, 160, 239, C_BACKGROUND);
	drawFrame(0, 240, 480, 272,  C_NODE_FILL, C_NODE_FRAME_N);
	for(int t = 1; t < 479; t++) {
		float fp_sample = final_input.read(eval_ff) * 8;
		eval_ff = !eval_ff;

		int int_sample = (int) fp_sample;
		if (int_sample < -15) int_sample = -15;
		if (int_sample > 15) int_sample = 15;

		drawPixel(t, 256 - int_sample, C_SYMBOL);
	}

	while (getTouchF()) {
		updateTouch();
	}

}


void eraseOutput(Output* o) {
	for (Node* n : node_vector) {
		for (int i = 0; i < n->n_inputs(); i++) {
			if (n->inputs[i].is_connected(o)) n->inputs[i].erase();
		}
	}
	if (final_input.is_connected(o)) final_input.erase();
}

void redrawConnections() {
	for (Node* n : node_vector) {
		for (int i = 0; i < n->n_inputs(); i++) {
			if (n->inputs[i].connected) n->inputs[i].draw(n->inputs[i].x0, n->inputs[i].y0);
		}
	}
	if (final_input.connected) final_input.draw(FINAL_INPUT_X, FINAL_INPUT_Y);
}

#define INPUT_WIDTH 9

void connection(Output* o) {
	while (getTouchF()) {
		updateTouch();
	}
	uint16_t tx = getTouchX();
	uint16_t ty = getTouchY();
	if (tx > 468 && ty > 128 && ty < 144) {
		if (!final_input.connected) {
			final_input.connect(*o);
			drawFrame(470, 132, 479, 141, C_BACKGROUND, C_SOCKET);
			final_input.draw(FINAL_INPUT_X, FINAL_INPUT_Y);
			return;
		}
		else if (final_input.is_connected(o)) {
			final_input.erase();
			final_input.disconnect();
			return;
		}
		else {
			final_input.erase();
			final_input.disconnect();
			final_input.connect(*o);
			drawFrame(470, 132, 479, 141, C_BACKGROUND, C_SOCKET);
			final_input.draw(FINAL_INPUT_X, FINAL_INPUT_Y);
			return;
		}
	}
	for (Node* n : node_vector) {
		uint16_t x = n->x0;
		if (x - INPUT_WIDTH < tx && x + INPUT_WIDTH > tx){
			for (int i = 0; i < n->n_inputs(); i++) {
				if (n->inputs[i].parameterized) {
					uint16_t y = n->inputs[i].y0;
					if (y - INPUT_WIDTH < ty && y + INPUT_WIDTH > ty) {
						if (!n->inputs[i].connected) {
							n->inputs[i].connect(*o);
							n->inputs[i].draw(n->inputs[i].x0, n->inputs[i].y0);
							return;
						}
						else if (n->inputs[i].is_connected(o)) {
							n->inputs[i].erase();
							n->inputs[i].disconnect();
							return;
						}
						else {
							n->inputs[i].erase();
							n->inputs[i].disconnect();
							n->inputs[i].connect(*o);
							n->inputs[i].draw(n->inputs[i].x0, n->inputs[i].y0);
							return;
						}
					}
				}
			}
		}
	}
	if (final_input.is_connected(o)) final_input.draw(FINAL_INPUT_X, FINAL_INPUT_Y);
}
