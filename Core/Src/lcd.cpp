/*
 * lcd.cpp
 *
 *  Created on: Nov 23, 2024
 *      Author: harmo
 */

#include "lcd.h"
#include "font.h"
#include "clut.h"

uint8_t screen[130560];

void drawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t c) {
	if (x1 > 480) x1 = 480;
	if (y1 > 272) y1 = 272;
	for(int j = y0; j < y1; j++) {
		int mx = j * 480 + x0;
		for(int i = x0; i < x1; i++) {
			screen[mx] = c;
			mx++;
		}
	}
}

void drawPixel(uint16_t x0, uint16_t y0, uint8_t c){
	if(x0 <= 480 && y0 <= 272) {
		screen[y0 * 480 + x0] = c;
	}
}

void drawChar(uint16_t x0, uint16_t y0, char l, uint8_t c) {
	int mx = ((uint8_t) l) - 32;
	const uint16_t *chp = font[mx];
	for(int j = 0; j < 15; j++) {
		uint16_t row = chp[j];
		uint16_t test = 0b1000000000000000;
		for(int i = x0; i < x0 + 16; i++) {
			if((row & test)==test) drawPixel(i, y0 + j, c);
			test >>= 1;
		}
	}
}

void drawSymbol(uint16_t x0, uint16_t y0, icon_name I, uint8_t c) {
	const uint16_t *chp = icon[I];
	for(int j = 0; j < 16; j++) {
		uint16_t row = chp[j];
		uint16_t test = 0b1000000000000000;
		for(int i = x0; i < x0 + 16; i++) {
			if((row & test)==test) drawPixel(i, y0 + j, c);
			test >>= 1;
		}
	}
}

void drawString(uint16_t x0, uint16_t y0, const char *l, uint16_t length, uint8_t c) {
	int cx = x0;
	for(int k = 0; k < length; k++) {
		char ch = l[k];
		if (ch == '/') return;
		int mx = ((uint8_t) l[k]) - 32;

		const uint16_t *chp = font[mx];
		for(int j = 0; j < 15; j++) {
			uint16_t row = chp[j];
			uint16_t test = 0b1000000000000000;
			for(int i = cx; i < cx + 16; i++) {
				if((row & test)==test) drawPixel(i, y0 + j, c);
				test >>= 1;
			}
		}
		cx += chp[15];
	}
}

void drawCurve(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t c) {
	if (x0 > x1) {
		drawCurve(x1, y1, x0, y0, c);
		return;
	}
	float w = 1.0 * x1 - x0;
	float h = 1.0 * y1 - y0;
	float r = w / (2*h);
	if (r < 0) r = -r;
	if (r > 1) r = 1;
	float k = -6.0 * h / (w * w * w);
	for(float x = 0; x < w; x+=r) {
		float fx = (x * x * x / 3) - (w * x * x / 2);
		fx *= k;
		int16_t dx = (int16_t) x;
		int16_t dy = (int16_t) fx;
		drawPixel(dx + x0, dy + y0, c);
	}
}

void drawFrame(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t c_f, uint8_t c_b) {
	drawRect(x0 + 1, y0 + 1, x1 - 1, y1 - 1, c_f);
	drawRect(x0, y0 + 2, x0 + 1, y1 - 2, c_b);
	drawRect(x1 - 1, y0 + 2, x1, y1 - 2, c_b);
	drawRect(x0 + 2, y0, x1 - 2, y0 + 1, c_b);
	drawRect(x0 + 2, y1 - 1, x1 - 2, y1, c_b);
	drawPixel(x0 + 1, y0 + 1, c_b);
	drawPixel(x1 - 2, y0 + 1, c_b);
	drawPixel(x0 + 1, y1 - 2, c_b);
	drawPixel(x1 - 2, y1 - 2, c_b);
}

void drawSocket(uint16_t x0, uint16_t y0, uint8_t c) {
	drawRect(x0 - 1, y0 - 2, x0 + 2, y0 - 1, c);
	drawRect(x0 - 1, y0 + 2, x0 + 2, y0 + 3, c);
	drawRect(x0 - 2, y0 - 1, x0 - 1, y0 + 2, c);
	drawRect(x0 + 2, y0 - 1, x0 + 3, y0 + 2, c);
	drawRect(x0 - 1, y0 - 1, x0 + 2, y0 + 2, 0);
	drawPixel(x0, y0, c);
}


