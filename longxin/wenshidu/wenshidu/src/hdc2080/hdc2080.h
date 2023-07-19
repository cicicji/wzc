/*
 * hdc2080.h
 *
 * created: 2021/6/5
 *  author: 
 */

#ifndef _HDC2080_H
#define _HDC2080_H

extern unsigned char data_buf[4];

void I2C1_init(void);
void Get_HDC_ID(void);
void HDC_Get_Temp_Hum(float *temp, float *hum);
void LED8_ON(void);
void LED8_OFF(void);

#endif // _HDC2080_H

