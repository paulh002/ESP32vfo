#include <WiFi.h>
#include <WiFiClient.h>
#include <TCA9548.h>
#include <RotaryEncoder.h>
#include "gui.h"
#include "network.h"
#include "IO.h"
#include "measurement.h"
#include "si5351.h"
#include "BandFilter.h"

TCA9548		tca(0x70);

const int	tca_sda = 21;
const int	tca_sdi = 22;
const int	tca_pcf = 2;
static	int	s_channel {-1};
/*-------------------------------------------------------
   Mechanical Rotary encoder settings
--------------------------------------------------------*/
const int	no_rotary_encoders = 2; 
RotaryEncoder decoder(0x3C, no_rotary_encoders, RotaryEncoder::LatchMode::TWO03);

Enc_irq enc_irq {false, 0};

void IRAM_ATTR moved()
{
	enc_irq.flag = true;
	enc_irq.counter++;
}

/*-------------------------------------------------------
   Si5351
--------------------------------------------------------*/
#define SI5351_BUS_BASE_ADDR		0x60
#define SI5351_XTAL_FREQ1 SI5351_XTAL_FREQ //25 mhz
//#define SI5351_XTAL_FREQ1 32000000

#define CLK_BFO_RX           SI5351_CLK1
#define CLK_VFO_RX           SI5351_CLK1
#define CLK_BFO_TX           SI5351_CLK0
#define CLK_VFO_TX           SI5351_CLK0
#define CLK_NA               SI5351_CLK2

Si5351 si5351(SI5351_BUS_BASE_ADDR);
Si5351 si5351_bfo(SI5351_BUS_BASE_ADDR);

void tca_channel(int channel)
{
	if (channel != s_channel)
	{
		tca.selectChannel(2);
		s_channel = channel;
	}
}

void init_io()
{
	char str[80];

	if (tca.begin(tca_sda, tca_sdi, 100000))
		Serial.println("TCA OK");
	else
		Serial.println("Error TCA9548 not found");
	delay(20);
	tca.selectChannel(2);
	delay(20);
	if (decoder.isConnected())
		Serial.println("PCF OK");
	else
		Serial.println("Error PCF8754 not found");

	sprintf(str, "I2C clock speed %d", Wire.getClock());
	Serial.println(str);
	pinMode(4, INPUT_PULLUP);
	attachInterrupt(4, moved, FALLING);
	decoder.setButtonPorts(5, 4);

	tca.selectChannel(7);
	if (si5351.init(SI5351_CRYSTAL_LOAD_8PF, SI5351_XTAL_FREQ1, 0) == false)
		Serial.println("SI5351 not found");
	else
		Serial.println("SI5351 found");

	Serial.println("SI5351 correction: " + String(R.correction_si5351_no1));
	si5351.set_correction(R.correction_si5351_no1, SI5351_PLL_INPUT_XO);
	si5351.drive_strength(CLK_VFO_RX, SI5351_DRIVE_2MA);
	si5351.drive_strength(CLK_VFO_TX, SI5351_DRIVE_2MA);
	si5351.output_enable(CLK_VFO_RX, 1);
	si5351.output_enable(CLK_VFO_TX, 0);
	si5351.output_enable(CLK_NA, 0);

	tca.selectChannel(6);
	if (si5351_bfo.init(SI5351_CRYSTAL_LOAD_8PF, SI5351_XTAL_FREQ1, 0) == false)
		Serial.println("SI5351 bfo not found");
	else
		Serial.println("SI5351 bfo found");

	Serial.println("SI5351 bfo correcttion: " + String(R.correction_si5351_no2));
	si5351_bfo.set_correction(R.correction_si5351_no2, SI5351_PLL_INPUT_XO);
	si5351_bfo.drive_strength(CLK_BFO_RX, SI5351_DRIVE_2MA);
	si5351_bfo.drive_strength(CLK_BFO_TX, SI5351_DRIVE_2MA);
	si5351_bfo.output_enable(CLK_BFO_RX, 1);
	si5351_bfo.output_enable(CLK_BFO_TX, 0);
	si5351_bfo.output_enable(CLK_NA, 0);

	bpf.Setup();
	tca.selectChannel(2);
}

void io_setbfo(uint32_t bfo_frq)
{
	char str[80];

	sprintf(str, "io_setbfo %ld\n", bfo_frq);
	Serial.println(str);

	tca.selectChannel(6);
	uint64_t freq = (uint64_t)bfo_frq * SI5351_FREQ_MULT;
	si5351_bfo.set_freq(freq, CLK_BFO_RX);
	si5351_bfo.set_freq(freq, CLK_BFO_TX);
	tca.selectChannel(2);
}

void io_setvfo(uint32_t frq, uint32_t offset_frq)
{
	char str[80];

	sprintf(str, "io_setvfo %ld\n", frq);
	Serial.println(str);

	tca.selectChannel(7);
	delay(10);
	uint64_t freq = (uint64_t)(frq + offset_frq) * SI5351_FREQ_MULT;
	si5351.set_freq(freq, CLK_VFO_RX);
	si5351.set_freq(freq, CLK_VFO_TX);
	delay(10);
	tca.selectChannel(2);
}

void io_rxtx(uint8_t rxtx)
{
	if (rxtx)
	{
		tca.selectChannel(6);
		si5351_bfo.output_enable(CLK_BFO_RX, 0);
		si5351_bfo.output_enable(CLK_BFO_TX, 1);
		tca.selectChannel(7);
		si5351.output_enable(CLK_VFO_RX, 0);
		si5351.output_enable(CLK_VFO_TX, 1);
		Serial.println("TX output enable");
		tca.selectChannel(2);
	}else
	{
		tca.selectChannel(6);
		si5351_bfo.output_enable(CLK_BFO_RX, 1);
		si5351_bfo.output_enable(CLK_BFO_TX, 0);
		tca.selectChannel(7);
		si5351.output_enable(CLK_VFO_RX, 1);
		si5351.output_enable(CLK_VFO_TX, 0);
		tca.selectChannel(2);
		Serial.println("RX output enable");
	}
}


#define CAL_FREQ		12500000L//10000000L
#define CAL_BFO_FREQ	8998000L


void start_cal()
{
	cal_bfo();
	cal_vfo();
}

void cal_vfo()
{
	long frq = CAL_FREQ;

	gui.updateBottomStatus(LV_COLOR_RED, "Calibration frequency " + String(CAL_FREQ / 1000) + " Khz", 0);

	si5351.set_correction(R.correction_si5351_no1, SI5351_PLL_INPUT_XO);
	tca.selectChannel(6);
	si5351.set_freq(frq * SI5351_FREQ_MULT, CLK_VFO_RX);
}

void cal_bfo()
{
	long frq = CAL_BFO_FREQ;

	gui.updateBottomStatus(LV_COLOR_RED, "Calibration frequency " + String(CAL_BFO_FREQ / 1000) + " Khz", 0);

	si5351_bfo.set_correction(R.correction_si5351_no2, SI5351_PLL_INPUT_XO);
	tca.selectChannel(7);
	si5351_bfo.set_freq(frq * SI5351_FREQ_MULT, CLK_VFO_RX);
}
