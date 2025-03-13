/*
 * InputEditor.cpp
 *
 *  Created on: Jan 14, 2025
 *      Author: harmo
 */

#include "main.h"
#include "InputEditor.h"
#include "lcd.h"
#include "ctp.h"
#include "Node.h"
#include <cstdio>
#include <math.h>
#include <string.h>

char note_names[12][3] {
	"A ",
	"A#",
	"B ",
	"C ",
	"C#",
	"D ",
	"D#",
	"E ",
	"F ",
	"F#",
	"G ",
	"G#"
};

InputEditor::InputEditor() {
	// TODO Auto-generated constructor stub

}

void InputEditor::draw() {

	//get colors
	char c_f = input_ptr->data_type + 16;
	char c_b = input_ptr->data_type + 24;

	// toggle visibility button
	drawFrame(x0, y0 + 5, x0 + 11, y1 - 6, c_b, c_f);
	if (input_ptr->parameterized)
	drawSocket(x0 + 5, y0 + 10, c_f);

	//draw frame
	drawFrame(x0 + 15, y0, x1, y1, c_b, c_f);

	char dt = input_ptr->data_type;


	char buffer[25];
	float reading = input_ptr->constant_data;
	switch (dt) {
	case (DT_HERTZ):
		if (reading < 15/(SAMPLING_RATE / (2 * PI))) {
			reading *= 60 * (SAMPLING_RATE / (2 * PI));
			sprintf (buffer, "%3.1f bpm/", reading);
		}
		else if (reading < 1000/(SAMPLING_RATE / (2 * PI))){
			reading *= (SAMPLING_RATE / (2 * PI));
			sprintf (buffer, "%3.1f Hz/", reading);
		}
		else {
			reading *= (SAMPLING_RATE / (2 * PI)/1000);
			sprintf (buffer, "%2.2f kHz/", reading);
		}
	break;
	case (DT_TIME):
			if (reading < .001) {
				reading *= 1000000;
				sprintf (buffer, "%3.1f us/", reading);
			}
			else if (reading < 1){
				reading *= 1000;
				sprintf (buffer, "%3.1f ms/", reading);
			}
			else {
				sprintf (buffer, "%2.2f s/", reading);
			}
		break;
	case(DT_INT):
		sprintf (buffer, "%3i/", (int) reading);
		break;
	case(DT_SCALAR):

		sprintf (buffer, "%3.3f/", reading);
		break;
	case(DT_DECIBEL):
		sprintf (buffer, "%2.2f dB/", 10 * log10(reading));
		break;
	case(DT_NOTE):{
		if (reading <= 0.0f) {
			buffer[0] = '/';
			break;
			break;
		}
		double log_ratio = log2(reading / (440.0f * PI / (SAMPLING_RATE/2)));

		int note_no;
		note_no = (int) round(12.0 * log_ratio);
		note_no = note_no % 12;
		if (note_no < 0) note_no += 12;
		int octave = (4 + ((int) round(log_ratio - (3.0f/12))));

		sprintf (buffer, "  %.1i/",  octave);
		buffer[0] = note_names[note_no][0];
		buffer[1] = note_names[note_no][1];
		break;
	}

	case(DT_INTERVAL):
	default:
		buffer[0] = '/';
	}
	drawString(x0 + 20, y0 + 5, buffer, 15, c_f);

}

bool InputEditor::poll() {
	int ty = getTouchY();
	if (ty > y1 || ty < y0) return false;
	int tx = getTouchX();
	char dt = input_ptr->data_type;
	if (tx < x0 + 12) {
		input_ptr->parameterized = ! input_ptr->parameterized;
		if (input_ptr->parameterized) {
			char c_f = dt + 16;
			drawSocket(x0 + 5, y0 + 10, c_f);
		}
		else {
			char c_b = dt + 24;
			drawRect(x0 + 2, y0 + 7, x0 + 8, y0 + 13, c_b);
			input_ptr->disconnect();
		}
		return true;

	}
	else {
		uint32_t start = htim5.Instance->CNT;
		float reading = input_ptr->constant_data;
		while (getTouchF()){
			updateTouch();
			int dx = getTouchX() - tx;
			int dy = getTouchY() - ty;

			if (dy > 80) {
				input_ptr->write(reading);
				int new_dt = (dx / 30) + dt;
				if (new_dt > 6) new_dt = 6;
				if (new_dt < 0) new_dt = 0;
				input_ptr->data_type = new_dt;
			}
			else {
				float new_value = 0.0f;
				switch (input_ptr->data_type) {
				case (DT_HERTZ):
						new_value = reading * exp(.005f * dx);
						if (new_value > PI)
							new_value = PI;
						break;
				case(DT_NOTE):{

					double log_ratio = log2(reading / (440.0f * PI / (SAMPLING_RATE/2)));
					int note_no = (int) (12 * log_ratio);

					note_no += (dx / 15);

					double new_ratio = pow(2, 1.0 * note_no / 12.0);
					new_value = new_ratio * (440.0 * PI / (SAMPLING_RATE/2));
					break;
				}
				case(DT_SCALAR):
						new_value = reading + 0.001f * dx;
						break;
				case(DT_DECIBEL):
						new_value = reading * exp(.005f * dx);
						break;
				case(DT_TIME):
						new_value = reading * exp(.005f * dx);
						if(new_value < (1.0f / SAMPLING_RATE)) new_value = (1.0f / SAMPLING_RATE);
						break;
				case(DT_INT):
						new_value = (float)((int) (reading + 0.1f * dx));
						break;
				}
				input_ptr->write(new_value);
			}
			draw();
		}
		uint32_t stop = htim5.Instance->CNT;
		if (stop - start < 24000000) {
			input_ptr->write(0.0f);
			draw();
		}
	}
	return true;
}
