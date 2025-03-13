/*
 * graph.h
 *
 *  Created on: Dec 29, 2024
 *      Author: harmo
 */

#ifndef INC_GRAPH_H_
#define INC_GRAPH_H_

#include <cstdint>
#include "Node.h"

extern bool play_state;
extern Input final_input;

void startupAnimation();

void initializeGraph();

void drawGraph();

void redrawGraphSection(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

void addNode();
void deleteNode(Node* n);

void drawSignalPlot();

void pollGraph();

void eraseOutput(Output* o);
void redrawConnections();
void connection(Output* o);

#endif /* INC_GRAPH_H_ */
