#include "BandFilter.h"
#include "PCF8574.h"
#include <TCA9548.h>

extern TCA9548	tca;
BandFilter bpf;

PCF8574 PCF_BPF0(0x38);
PCF8574 PCF_BPF1(0x39);
PCF8574 PCF_LPF0(0x3F);

// Low pass filter
// Pin 1 Q4 0x08   80m
// Pin 2 Q5 0x10   40m
// Pin 3 Q7 0x40   20m
// Pin 4 Q3 0x04   15m
// Pin 5 Q2 0x02   10m
// Pin 6 Q1 0x01   Not used / No filter yet fitted
// Pin 7 Q6 0x20   RX/TX


const int bpf080	= 0x16;
const int bpf040	= 0x19;
const int bpf020	= 0x25;
const int bpf017	= 0x15;
const int bpf0trg	= 0x15;
const int bpf180	= 0x85;
const int bpf140	= 0x85;
const int bpf120	= 0x85;
const int bpf117	= 0x86;
const int bpf1trg	= 0x89;

const int bpf180tx	= 0x45;
const int bpf140tx	= 0x45;
const int bpf120tx	= 0x45;
const int bpf117tx	= 0x46;
const int bpf1trgtx	= 0x49;

const int lpf160M = 0x01;
const int lpf80M = 0x08;
const int lpf60M = 0x10;
const int lpf40M = 0x10;
const int lpf30M = 0x40;
const int lpf20M = 0x40;
const int lpf15M = 0x04;
const int lpf10M = 0x02;
const int lpftx = 0x20;

void BandFilter::Setup()
{
    int i = 0;
    tca.selectChannel(4);
    do
    {
        if (!PCF_BPF0.isConnected())
        {
            Serial.println("=> PCF_BPF0 not connected");
        }
        else 
        {
            Serial.println("PCF_BPF0 connected...");
        }
        if (!PCF_BPF1.isConnected())
        {
            Serial.println("=> PCF_BPF1 not connected");
        }
        else
        {
            Serial.println("PCF_BPF1 connected...");
            break;
        }
        i++;
    } while (i < 10);
    PCF_BPF0.write8(bpf080);
    PCF_BPF1.write8(bpf080);
}

void BandFilter::setBand(int band, int rxtx)
{
    int bpf0, bpf1, lpf;

    tca.selectChannel(4);
    if (rxtx)
    {// tx
        switch (band)
        {
        case 160:
            bpf0 = bpf0trg;
            bpf1 = bpf1trgtx;
            break;
        case 80:
            bpf0 = bpf0trg;
            bpf1 = bpf1trgtx;
            break;
        case 60:
            bpf0 = bpf0trg;
            bpf1 = bpf1trgtx;
            break;
        case 40:
            bpf0 = bpf0trg;
            bpf1 = bpf1trgtx;
            break;
        case 30:
            bpf0 = bpf0trg;
            bpf1 = bpf1trgtx;
            break;
        case 20:
            bpf0 = bpf0trg;
            bpf1 = bpf1trgtx;
            break;
        case 18:
            bpf0 = bpf0trg;
            bpf1 = bpf1trgtx;
            break;
        case 15:
            bpf0 = bpf0trg;
            bpf1 = bpf1trgtx;
            break;
        case 10:
            bpf0 = bpf0trg;
            bpf1 = bpf1trgtx;
            break;
        }
    }
    else
    {// rx
        switch (band)
        {
        case 160:
            bpf0 = bpf0trg;
            bpf1 = bpf1trg;
            break;
        case 80:
            bpf0 = bpf0trg;
            bpf1 = bpf1trg;
            break;
        case 60:
            bpf0 = bpf0trg;
            bpf1 = bpf1trg;
            break;
        case 40:
            bpf0 = bpf0trg;
            bpf1 = bpf1trg;
            break;
        case 30:
            bpf0 = bpf0trg;
            bpf1 = bpf1trg;
            break;
        case 20:
            bpf0 = bpf0trg;
            bpf1 = bpf1trg;
            break;
        case 18:
            bpf0 = bpf0trg;
            bpf1 = bpf1trg;
            break;
        case 15:
            bpf0 = bpf0trg;
            bpf1 = bpf1trg;
            break;
        case 10:
            bpf0 = bpf0trg;
            bpf1 = bpf1trg;
            break;
        }
    }

    PCF_BPF0.write8(bpf0);
    PCF_BPF1.write8(bpf1);
    tca.selectChannel(2);
}


