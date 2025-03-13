/*
 * InputEditor.h
 *
 *  Created on: Jan 14, 2025
 *      Author: harmo
 */

#ifndef SRC_INPUTEDITOR_H_
#define SRC_INPUTEDITOR_H_

#include <cstdint>


class Input;

class InputEditor {
public:
	uint16_t x0 = 145, y0, x1 = 230, y1;
	Input* input_ptr;

	InputEditor();
	void draw();

	bool poll();

};

#endif /* SRC_INPUTEDITOR_H_ */
