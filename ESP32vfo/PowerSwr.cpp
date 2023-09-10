#include "PowerSwr.h"
#include "measurement.h"

PowerSwr powerSwr;

LV_FONT_DECLARE(FreeSansOblique42);
LV_FONT_DECLARE(FreeSansOblique24);

extern const int screenWidth;
extern const int screenHeight;
extern const int topHeight;
extern const int bottomHeight;

void PowerSwr::init(lv_obj_t* scr)
{
	lv_style_init(&mid_style);
	lv_style_set_radius(&mid_style, 0);
	lv_style_set_bg_color(&mid_style, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_border_width(&mid_style, 0);

	tabview =  lv_obj_create(scr);
	lv_obj_add_style(tabview, &mid_style, 0);
	lv_obj_set_pos(tabview, 0, topHeight);
	lv_obj_set_size(tabview, LV_HOR_RES, screenHeight - topHeight);
	lv_obj_clear_flag(tabview, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_hor(tabview, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_ver(tabview, 0, LV_PART_MAIN);
	lv_obj_move_background(tabview);

	int swrHeight = lv_obj_get_height(tabview) / 2;	
	const int pwr_width = 4 * (LV_HOR_RES / 8) + 10;
	const int swr_width = 3 * (LV_HOR_RES / 8);

	bg_pwr = lv_obj_create(tabview);
	lv_obj_set_width(bg_pwr, lv_pct(55));
	lv_obj_set_height(bg_pwr, lv_pct(45));
	lv_obj_set_x(bg_pwr, lv_pct(0));
	lv_obj_set_y(bg_pwr, lv_pct(14));
	lv_obj_clear_flag(bg_pwr, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_bg_color(bg_pwr, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(bg_pwr, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(bg_pwr, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(bg_pwr, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(bg_pwr, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(bg_pwr, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

	bg_swr = lv_obj_create(tabview);
	lv_obj_set_width(bg_swr, lv_pct(40));
	lv_obj_set_height(bg_swr, lv_pct(45));
	lv_obj_set_x(bg_swr, 0);
	lv_obj_set_y(bg_swr, lv_pct(14));
	lv_obj_set_align(bg_swr, LV_ALIGN_TOP_RIGHT);
	lv_obj_clear_flag(bg_swr, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	lv_obj_set_style_bg_color(bg_swr, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(bg_swr, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(bg_swr, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(bg_swr, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(bg_swr, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(bg_swr, 0, LV_PART_MAIN | LV_STATE_DEFAULT);


	pwr = lv_label_create(bg_pwr);
	lv_obj_set_width(pwr, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(pwr, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(pwr, LV_ALIGN_CENTER);
	lv_label_set_text(pwr, "999 mW");
	lv_obj_set_style_text_color(pwr, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(pwr, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(pwr, &lv_font_montserrat_36, LV_PART_MAIN | LV_STATE_DEFAULT);

	pwr_label = lv_label_create(bg_pwr);
	lv_obj_set_width(pwr_label, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(pwr_label, LV_SIZE_CONTENT);    /// 1
	lv_label_set_text(pwr_label, "Power");
	lv_obj_set_style_text_color(pwr_label, lv_color_hex(0xC9DF0F), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(pwr_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	swr = lv_label_create(bg_swr);
	lv_obj_set_width(swr, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(swr, LV_SIZE_CONTENT);    /// 1
	lv_obj_set_align(swr, LV_ALIGN_CENTER);
	lv_label_set_text(swr, "1:1.8");
	lv_obj_set_style_text_color(swr, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(swr, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(swr, &lv_font_montserrat_36, LV_PART_MAIN | LV_STATE_DEFAULT);

	swr_unit = lv_label_create(bg_swr);
	lv_obj_set_width(swr_unit, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(swr_unit, LV_SIZE_CONTENT);    /// 1
	lv_label_set_text(swr_unit, "SWR");
	lv_obj_set_style_text_color(swr_unit, lv_color_hex(0xC9DF0F), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(swr_unit, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	
	SwrBar.init(tabview, 7 * (LV_HOR_RES / 8), 30);
	SwrBar.align(tabview, LV_ALIGN_BOTTOM_MID, 0, 0);
}

void PowerSwr::setPowerValue(double pwrValue)
{
	std::string value;

	print_p_mw_value(pwrValue, value);
	lv_label_set_text(pwr, value.c_str());
}
void PowerSwr::setSwrValue(uint16_t swrValue)
{
	char buf[32];
	sprintf(buf, "1:%d.%d", swrValue / 100, (swrValue / 10) % 10);
	lv_label_set_text(swr, buf);
}

void PowerSwr::barValue(double value)
{
	double dbm = log10(value) * 10;
	SwrBar.value(floor(dbm));
}
