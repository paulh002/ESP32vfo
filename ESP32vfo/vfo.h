// VFO.h

#ifndef _Vfo_h_
#define _Vfo_h_

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#define MODE_LSB 0
#define MODE_USB 1

#define USB_FREQUENCY 9001000 //9000500 //8998000 
#define LSB_FREQUENCY 8998000 //8998000 //8995000

extern uint8_t			f_rxtx;  // initial rx

void next_band(uint8_t dir, uint8_t& band);
void check_rx_tx();
void band_roller_str(char* str);
long set_encoder_count_to_vfo_frequency(int count, int active_vfo);
long band_select(char* buf, int active_vfo);
long band_select_next(char* buf, int active_vfo);
uint16_t get_band(char* str);
int  get_band(int active_vfo);
void	set_vfo_frequency(long frq, int active_vfo);
long	set_encoder_count_to_bfo_frequency(int count);
void init_vfo();
void start_vfo();
void save_vfo();
void setbfo(uint32_t bfo_frq);
long switch_vfo(int active_vfo);
void switch_band(int active_vfo, int rxtx);
long get_vfo_frequency(int active_vfo);
uint32_t get_bfo(int active_vfo);
uint16_t get_smeter();
void switch_mode(uint8_t mode, int active_vfo);
bool CheckCAT();
#endif
