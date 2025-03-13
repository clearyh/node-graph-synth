/*
 * ctp.cpp
 *
 *  Created on: Dec 8, 2024
 *      Author: harmo
 */

#include "ctp.h"
#include "main.h"

#define MAX_TOUCH 2
#define CTP_ADDR 0x70

uint8_t ctp_data[5];

/*void updateTouch() {
	uint8_t s_adr = 0x03;
	uint8_t data[6 * MAX_TOUCH];
	HAL_I2C_Master_Transmit (&hi2c4, ADDR, &s_adr, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive (&hi2c4, ADDR, data, 6 * MAX_TOUCH, HAL_MAX_DELAY);
	for(int i = 0; i < MAX_TOUCH; i++) {
		int mx = i * 6;
		if(!((data[mx] & 0b01000000) == 0b01000000)) tp[i].flag = true;
		else tp[i].flag = false;
		data[mx] &= 0b00111111;
		tp[i].ypos = data[mx];
		tp[i].ypos  <<= 8;
		tp[i].ypos |= data[mx + 1];
		data[mx + 2] &= 0x0F;
		tp[i].xpos = data[mx + 2];
		tp[i].xpos <<= 8;
		tp[i].xpos |= data[mx + 3];
		tp[i].weight = data[mx + 4];
	}
}*/

void updateTouch() {
	uint8_t s_adr = 0x03;
	HAL_I2C_Master_Transmit (&hi2c4, CTP_ADDR, &s_adr, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive (&hi2c4, CTP_ADDR, ctp_data, 5, HAL_MAX_DELAY);
}

bool getTouchF() {
	if ((ctp_data[0] & 0b10000000) == 0b10000000) return true;
	else return false;
}

int getTouchX() {
	int result = ctp_data[2] & 0x0F;
	result <<= 8;
	result |= ctp_data[3];
	return result;
}

int getTouchY() {
	int result = ctp_data[0] & 0x0F;
	result <<= 8;
	result |= ctp_data[1];
	return result;
}

int getTouchW() {
	int result = ctp_data[4];
	return result;
}


