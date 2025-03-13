/*
 * ctp.h
 *
 *  Created on: Dec 8, 2024
 *      Author: harmo
 */

#ifndef INC_CTP_H_
#define INC_CTP_H_

#include <cstdint>

extern uint8_t ctp_data[5];

void updateTouch();

bool getTouchF();
int getTouchX();
int getTouchY();
int getTouchW();



#endif /* INC_CTP_H_ */
