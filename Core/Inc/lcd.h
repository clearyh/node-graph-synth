/*
 * lcd.h
 *
 *  Created on: Nov 23, 2024
 *      Author: harmo
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include <cstdint>
#include "clut.h"

extern uint8_t screen[130560];

enum icon_name {
	I_ADD_NODE,
	I_PAUSE,
	I_PLAY,
	I_EDIT_NODE,
	I_DELETE_NODE,
	I_DARKMODE,
	I_PLOT
};

void drawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t c);

void drawPixel(uint16_t x0, uint16_t y0, uint8_t c);

void drawChar(uint16_t x0, uint16_t y0, char l, uint8_t c);

void drawSymbol(uint16_t x0, uint16_t y0, icon_name I, uint8_t c);

void drawString(uint16_t x0, uint16_t y0, const char *l, uint16_t length, uint8_t c);

void drawCurve(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t c);

void drawFrame(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t c_f, uint8_t c_b);

void drawSocket(uint16_t x0, uint16_t y0, uint8_t c);
#endif /* INC_LCD_H_ */
