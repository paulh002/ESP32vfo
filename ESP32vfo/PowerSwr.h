#pragma once
#include <lvgl.h>
#include "SwrBarClass.h"

class PowerSwr
{
public:
	void init(lv_obj_t* tabview);
	void setPowerValue(double pwrValue);
	void setSwrValue(uint16_t swrValue);
	void barValue(double value);
private:
	const int swrHeight = 200;
	lv_obj_t *bg_swr, *bg_pwr, *swr, *pwr,* pwr_unit, *swr_unit, *pwr_label;
	SwrBarClass SwrBar;
	lv_style_t	mid_style;
	lv_obj_t* tabview;
};

extern PowerSwr	powerSwr;