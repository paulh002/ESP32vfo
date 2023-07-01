#pragma once
#include <string>

class BandFilter
{
public:
	void Setup();
	void setBand(int band, int txrx);
};

extern BandFilter bpf;