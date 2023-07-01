// ArduinoC.h

#ifndef _IO_h
#define _IO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/*-------------------------------------------------------
   Bandswitching and RX/TX switching
--------------------------------------------------------*/
// SMD PCB
#define SW_RX     0x55    //0101 0101
#define SW_TX     0x95    //1001 0000 // 0000 0101

void io_setbfo(uint32_t bfo_frq);
void io_setvfo(uint32_t frq, uint32_t offset_frq);
void init_io();
void start_cal();
void cal_vfo();
void cal_bfo();
void io_rxtx(uint8_t rxtx);

struct Enc_irq
{
	bool flag;
	uint32_t counter;
};

extern RotaryEncoder decoder;
extern Enc_irq enc_irq;
#endif
