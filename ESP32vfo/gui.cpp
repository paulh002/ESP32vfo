// 
// 
// 
#include <time.h>
#include <ctime>
#include <atomic>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <RotaryEncoder.h>
#include <lvgl.h>
#include <ESP32Encoder.h>
#include "measurement.h"
#include "network.h"
#include "vfo.h"
#include "io.h"
#include "gui.h"

Gui gui;
#define LVGL_TICK_PERIOD 10

#define PULSE_INPUT_PIN 17  // Rotaty Encoder A
#define PULSE_CTRL_PIN  23  // Rotaty Encoder B

const int screenWidth = 320;
const int screenHeight = 240;
const int topHeight = 25;
const int bottomHeight = 40;
const int nobuttons = 6;
const int bottombutton_width = (screenWidth / nobuttons) - 2;
const int bottombutton_width1 = (screenWidth / nobuttons);
static const char* btnm_map[] = { "1", "2", "3", "4", "5", "" };

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

LV_FONT_DECLARE(FreeSansOblique42);
LV_FONT_DECLARE(FreeSansOblique32);

lv_indev_state_t	enc_button_state = LV_INDEV_STATE_REL;
int					encoder_count = 0;
TaskHandle_t		hWifiTask;

void mode_button_eh(lv_event_t* e);
void event_button_bfo_cb(lv_event_t* e);
void event_button_save(lv_event_t* e);
void event_button_reset_cb(lv_event_t* e);
void event_button_ad8307_cb(lv_event_t* e);
void event_button_wifi_cb(lv_event_t* e);
void wifi_button_eh(lv_event_t* e);
void close_event_handler(lv_event_t* e);
void matrix_event_handler(lv_event_t* e);
void keyboard_event_cb(lv_event_t* e);
void event_button_setting_cb(lv_event_t* e);
void event_band_button(lv_event_t* e);
void si_vfo_button_cb(lv_event_t* e);
void si_bfo_button_cb(lv_event_t *e);
void mode_button_cal_save(lv_event_t* e);
void mode_button_cal_cancel(lv_event_t* e);

void lv_spinbox_increment_event_ad1(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_increment(gui.ad_spinbox1);
		R.cal_AD[0].db10m = lv_spinbox_get_value(gui.ad_spinbox1);
	}
}

void lv_spinbox_decrement_event_ad1(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_decrement(gui.ad_spinbox1);
		R.cal_AD[0].db10m = lv_spinbox_get_value(gui.ad_spinbox1);
	}
}

void lv_spinbox_increment_event_ad2(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_increment(gui.ad_spinbox2);
		R.cal_AD[1].db10m = lv_spinbox_get_value(gui.ad_spinbox2);
	}
}

void lv_spinbox_increment_event_ad4(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_increment(gui.ad_spinbox4);
		R.cal_AD[1].Fwd = (double)lv_spinbox_get_value(gui.ad_spinbox4) / 1000.0;
	}
}

void lv_spinbox_decrement_event_ad3(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_decrement(gui.ad_spinbox3);
		R.scale_watt = lv_spinbox_get_value(gui.ad_spinbox3);
	}
}

void lv_spinbox_increment_event_ad3(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_increment(gui.ad_spinbox3);
		R.scale_watt = lv_spinbox_get_value(gui.ad_spinbox3);
	}
}

void lv_spinbox_decrement_event_ad2(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_decrement(gui.ad_spinbox2);
		R.cal_AD[1].db10m = lv_spinbox_get_value(gui.ad_spinbox2);
	}
}

void lv_spinbox_decrement_event_ad4(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_decrement(gui.ad_spinbox4);
		R.cal_AD[1].Fwd = (double)lv_spinbox_get_value(gui.ad_spinbox4) / 1000.0;
	}
}

void lv_spinbox_event_save(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {

		R.cal_AD[0].db10m = lv_spinbox_get_value(gui.ad_spinbox1);
		R.cal_AD[0].Rev = R.cal_AD[0].Fwd = adc_ref * ((double)analogRead(FWD_METER) / 4096.0);
		lv_spinbox_set_value(gui.ad_spinbox4, (uint32_t)(R.cal_AD[0].Fwd * 1000.0));
	}
}

void lv_spinbox_event_save1(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {

		R.cal_AD[1].db10m = lv_spinbox_get_value(gui.ad_spinbox2);
		R.cal_AD[1].Rev = R.cal_AD[1].Fwd = adc_ref * ((double)analogRead(FWD_METER) / 4096.0);
		lv_spinbox_set_value(gui.ad_spinbox4, (uint32_t)(R.cal_AD[0].Fwd * 1000.0));
	}
}

void lv_spinbox_event_copy(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT)
	{
		// Set second calibration point at 30 dB less
		R.cal_AD[1].db10m = R.cal_AD[0].db10m - 300;
		R.cal_AD[1].Fwd = R.cal_AD[0].Fwd - LOGAMP1_SLOPE * .001 * 30;
		R.cal_AD[1].Rev = R.cal_AD[0].Fwd - LOGAMP2_SLOPE * .001 * 30;
		lv_spinbox_set_value(gui.ad_spinbox2, R.cal_AD[1].db10m);
	}
}

void lv_spinbox_event_save4(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
		uint32_t value;

		value = lv_spinbox_get_value(gui.ad_spinbox4);
		R.cal_AD[0].Rev = R.cal_AD[0].Fwd = (double)value / 1000.0;
	}
}

void lv_spinbox_event_save5(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
		uint32_t value;

		value = lv_spinbox_get_value(gui.ad_spinbox4);
		R.cal_AD[1].Rev = R.cal_AD[1].Fwd = (double)value / 1000.0;
	}
}

void Gui::show_cal_screen_si(bool show)
{
	if (show)
	{
		gui.f_cal_si5351 = true;
		lv_obj_move_foreground(bg_middle3);
		lv_indev_set_group(encoder_indev_t, si_5351_group);
		gui.si_setcalfrequency(R.correction_si5351_no1);
		lv_obj_add_state(si_vfo_button, LV_STATE_CHECKED);
	}
	else
	{
		lv_obj_move_foreground(bg_middle);
		lv_obj_move_background(bg_middle3);
		lv_indev_set_group(encoder_indev_t, vfo_group);
		f_cal_si5351 = false;
	}
}

void si_save_button_cb(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_PRESSED) {
		SaveEEPROM();
		gui.updateBottomStatus(LV_COLOR_GREEN, "Si5351 calibration saved", 0);
		gui.show_cal_screen_si(false);
	}
}

void si_cancel_button_cb(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_PRESSED) {
		gui.show_cal_screen_si(false);
	}
}

#if LV_USE_LOG1 != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char* file, uint32_t line, const char* fn_name, const char* dsc)
{
	Serial.printf("%s(%s)@%d->%s\r\n", file, fn_name, line, dsc);
	Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
{
	uint32_t w = (area->x2 - area->x1 + 1);
	uint32_t h = (area->y2 - area->y1 + 1);

	tft.startWrite();
	tft.setAddrWindow(area->x1, area->y1, w, h);
	tft.pushColors((uint16_t*)&color_p->full, w * h, true);
	tft.endWrite();

	lv_disp_flush_ready(disp);
}

void read_encoder(lv_indev_drv_t* indev, lv_indev_data_t* data)
{
	data->enc_diff = (int)decoder.getDirection(0);
	if (data->enc_diff)
		decoder.ClearEncoder(0);
	data->state = enc_button_state;
	if (data->enc_diff > 0)
		data->enc_diff = 1;
	if (data->enc_diff < 0)
		data->enc_diff = -1;
	return;
}

/*
void read_encoder(lv_indev_drv_t* indev, lv_indev_data_t* data)
{
	data->enc_diff = 0;
	if (encoder_count || enc_button_state == LV_INDEV_STATE_PRESSED)
	{
		//char str[80];
		data->enc_diff = encoder_count * -1;
		//sprintf(str, "encoder_count %d state = %d\n\r", encoder_count, (int)enc_button_state);
		//Serial.print(str);
		encoder_count = 0;
	}
	data->state = enc_button_state;
	if (data->enc_diff > 0)
		data->enc_diff = 1;
	if (data->enc_diff < 0)
		data->enc_diff = -1;
	return ;
}
*/

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t* indev_driver, lv_indev_data_t* data)
{
	uint16_t touchX, touchY;

	bool touched = tft.getTouch(&touchX, &touchY, 600);

	if (tft.getRotation() == 1)
	{
		touchX = screenWidth - touchX;
		//touchY = screenHeight - touchY;  // only for 3.2 inch
	}

	if (!touched)
	{
		data->state = LV_INDEV_STATE_REL;
	}
	else
	{
		data->state = LV_INDEV_STATE_PR;

		/*Set the coordinates*/
		data->point.x = touchX;
		data->point.y = touchY;
	}
}

void guiTask(void* arg) {
	gui(arg);

}

void Gui::gui_setup()
{
	gui.init();
	xTaskCreate(guiTask,
		"gui",
		4096 * 2,
		NULL,
		2,
		NULL);
}

void Gui::init()
{
	GuiBinarySemaphore = xSemaphoreCreateMutex();
	if (GuiBinarySemaphore == NULL) {
		Serial.println("Error creating the GuiBinarySemaphore");
	}

	ESP32Encoder::useInternalWeakPullResistors = NONE;
	Enc_vfo.attachFullQuad(PULSE_INPUT_PIN, PULSE_CTRL_PIN);

	lv_init();
	tft.begin(); /* TFT init */
	tft.setRotation(1);
	//tft.fillScreen(TFT_BLACK);
	
#if LV_USE_LOG1 != 0
	lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

	uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
	tft.setTouch(calData);

	lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

	/*Initialize the display*/
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	/*Change the following line to your display resolution*/
	disp_drv.hor_res = screenWidth;
	disp_drv.ver_res = screenHeight;
	disp_drv.flush_cb = my_disp_flush;
	disp_drv.draw_buf = &draw_buf;
	lv_disp_drv_register(&disp_drv);

	/*Initialize the (dummy) input device driver*/
	static lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = my_touchpad_read;
	lv_indev_drv_register(&indev_drv);
	
	static lv_indev_drv_t indev_drv_enc;
	lv_indev_drv_init(&indev_drv_enc);
	indev_drv_enc.type = LV_INDEV_TYPE_ENCODER;
	indev_drv_enc.read_cb = read_encoder;
	encoder_indev_t = lv_indev_drv_register(&indev_drv_enc);

	lv_theme_t* th = lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_CYAN), LV_THEME_DEFAULT_DARK, &lv_font_montserrat_14);
	lv_disp_set_theme(NULL, th);
	scr = lv_scr_act();

	lv_style_init(&top_style);
	lv_style_set_radius(&top_style, 0);
	lv_style_set_bg_color(&top_style, lv_palette_main(LV_PALETTE_INDIGO));
	lv_style_set_border_width(&top_style, 0);

	bg_top = lv_obj_create(scr);
	lv_obj_add_style(bg_top, &top_style, 0);
	lv_obj_set_size(bg_top, LV_HOR_RES, topHeight);
	lv_obj_clear_flag(bg_top, LV_OBJ_FLAG_SCROLLABLE);

	label_status = lv_label_create(bg_top);
	lv_label_set_long_mode(label_status, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_width(label_status, LV_HOR_RES - 20);
	lv_label_set_text(label_status, "");
	lv_obj_align(label_status, LV_ALIGN_CENTER, 20, 0);

	lv_style_init(&mid_style);
	lv_style_set_radius(&mid_style, 0);
	lv_style_set_bg_color(&mid_style, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_border_width(&mid_style, 0);

	bg_middle = lv_obj_create(scr);
	lv_obj_add_style(bg_middle, &mid_style, 0);
	lv_obj_set_pos(bg_middle, 0, topHeight);
	lv_obj_set_size(bg_middle, LV_HOR_RES, screenHeight - topHeight);
	lv_obj_clear_flag(bg_middle, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_hor(bg_middle, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_ver(bg_middle, 0, LV_PART_MAIN);

	smeterLabel = lv_label_create(bg_middle);
	lv_obj_set_width(smeterLabel, LV_SIZE_CONTENT);   /// 1
	lv_obj_set_height(smeterLabel, LV_SIZE_CONTENT);    /// 1
	//lv_obj_set_x(smeterLabel, 0);
	//lv_obj_set_y(smeterLabel, 120);
	lv_obj_align(smeterLabel, LV_ALIGN_TOP_MID, 0, 100);
	lv_label_set_text(smeterLabel, "S         1     3     5     7     9     20     40     60");
	lv_obj_set_style_text_color(smeterLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(smeterLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	SmeterBar.init(bg_middle, 6 * (LV_HOR_RES / 8), 120);
	SmeterBar.align(bg_middle, LV_ALIGN_TOP_MID, 0, 120);
	SmeterBar.SetRange(4096);
	SmeterBar.value(100);

	// Create groups for encoder support for the different screens
	vfo_group = lv_group_create();
	lv_indev_set_group(encoder_indev_t, vfo_group);

	lv_style_init(&style_btn);
	lv_style_set_radius(&style_btn, 10);
	lv_style_set_bg_color(&style_btn, lv_color_make(0x60, 0x60, 0x60));
	lv_style_set_bg_grad_color(&style_btn, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_btn, 255);
	lv_style_set_border_color(&style_btn, lv_color_make(0x9b, 0x36, 0x36));      // lv_color_make(0x2e, 0x44, 0xb2)
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_opa(&style_btn, 255);
	lv_style_set_outline_color(&style_btn, lv_color_black());
	lv_style_set_outline_opa(&style_btn, 255);

	lsb_button = lv_btn_create(bg_middle);
	lv_obj_add_style(lsb_button, &style_btn, 0);
	lv_obj_add_event_cb(lsb_button, mode_button_eh, LV_EVENT_CLICKED, NULL);
	lv_obj_align(lsb_button, LV_ALIGN_BOTTOM_LEFT, 0, 0);
	lv_obj_add_flag(lsb_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(lsb_button, bottombutton_width, bottomHeight);
	lv_obj_t* label = lv_label_create(lsb_button);
	lv_label_set_text(label, "Lsb");
	lv_obj_center(label);

	usb_button = lv_btn_create(bg_middle);
	lv_obj_add_style(usb_button, &style_btn, 0);
	lv_obj_add_event_cb(usb_button, mode_button_eh, LV_EVENT_CLICKED, NULL);
	lv_obj_align(usb_button, LV_ALIGN_BOTTOM_LEFT, bottombutton_width1, 0);
	lv_obj_add_flag(usb_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(usb_button, bottombutton_width, bottomHeight);
	label = lv_label_create(usb_button);
	lv_label_set_text(label, "Usb");
	lv_obj_center(label);

	vfo1_button = lv_btn_create(bg_middle);
	lv_obj_add_style(vfo1_button, &style_btn, 0);
	lv_obj_add_event_cb(vfo1_button, mode_button_eh, LV_EVENT_CLICKED, NULL);
	lv_obj_align(vfo1_button, LV_ALIGN_BOTTOM_LEFT, 2 * bottombutton_width1, 0);
	lv_obj_add_flag(vfo1_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(vfo1_button, bottombutton_width, bottomHeight);
	label = lv_label_create(vfo1_button);
	lv_label_set_text(label, "Vfo 1");
	lv_obj_center(label);

	vfo2_button = lv_btn_create(bg_middle);
	lv_obj_add_style(vfo2_button, &style_btn, 0);
	lv_obj_add_event_cb(vfo2_button, mode_button_eh, LV_EVENT_CLICKED, NULL);
	lv_obj_align(vfo2_button, LV_ALIGN_BOTTOM_LEFT, 3 * bottombutton_width1, 0);
	lv_obj_add_flag(vfo2_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(vfo2_button, bottombutton_width, bottomHeight);
	label = lv_label_create(vfo2_button);
	lv_label_set_text(label, "Vfo 2");
	lv_obj_center(label);

	band_button = lv_btn_create(bg_middle);
	lv_obj_add_style(band_button, &style_btn, 0);
	lv_obj_add_event_cb(band_button, event_band_button, LV_EVENT_CLICKED, NULL);
	lv_obj_align(band_button, LV_ALIGN_BOTTOM_LEFT, 4 * bottombutton_width1, 0);
	lv_obj_add_flag(band_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(band_button, bottombutton_width, bottomHeight);
	Band_btn_label = lv_label_create(band_button);
	lv_label_set_text(Band_btn_label, "80 m");
	lv_obj_center(Band_btn_label);

	setting_button = lv_btn_create(bg_middle);
	lv_obj_add_style(setting_button, &style_btn, 0);
	lv_obj_add_event_cb(setting_button, event_button_setting_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_align(setting_button, LV_ALIGN_BOTTOM_LEFT, 5 * bottombutton_width1, 0);
	lv_obj_add_flag(setting_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(setting_button, bottombutton_width, bottomHeight);
	label = lv_label_create(setting_button);
	lv_label_set_text(label, LV_SYMBOL_SETTINGS);
	lv_obj_center(label);

	wifi1_button = lv_btn_create(bg_middle);
	lv_obj_add_style(wifi1_button, &style_btn, 0);
	lv_obj_add_event_cb(wifi1_button, event_button_wifi_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_align(wifi1_button, LV_ALIGN_BOTTOM_LEFT, 0, -1 * (bottomHeight + 2));
	lv_obj_add_flag(wifi1_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(wifi1_button, bottombutton_width, bottomHeight);
	label = lv_label_create(wifi1_button);
	lv_label_set_text(label, LV_SYMBOL_WIFI);
	lv_obj_center(label); 
	lv_obj_add_flag(wifi1_button, LV_OBJ_FLAG_HIDDEN);

	si5351_button = lv_btn_create(bg_middle);
	lv_obj_add_style(si5351_button, &style_btn, 0);
	lv_obj_add_event_cb(si5351_button, event_button_wifi_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_align(si5351_button, LV_ALIGN_BOTTOM_LEFT, bottombutton_width1, -1 * (bottomHeight + 2));
	lv_obj_add_flag(si5351_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(si5351_button, bottombutton_width, bottomHeight);
	label = lv_label_create(si5351_button);
	lv_label_set_text(label, "5351");
	lv_obj_center(label);
	lv_obj_add_flag(si5351_button, LV_OBJ_FLAG_HIDDEN);

	ad8307_button = lv_btn_create(bg_middle);
	lv_obj_add_style(ad8307_button, &style_btn, 0);
	lv_obj_add_event_cb(ad8307_button, event_button_ad8307_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_align(ad8307_button, LV_ALIGN_BOTTOM_LEFT, 2 * bottombutton_width1, -1 * (bottomHeight + 2));
	lv_obj_add_flag(ad8307_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(ad8307_button, bottombutton_width, bottomHeight);
	label = lv_label_create(ad8307_button);
	lv_label_set_text(label, "8307");
	lv_obj_center(label);
	lv_obj_add_flag(ad8307_button, LV_OBJ_FLAG_HIDDEN);

	reset_button = lv_btn_create(bg_middle);
	lv_obj_add_style(reset_button, &style_btn, 0);
	lv_obj_add_event_cb(reset_button, event_button_reset_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_align(reset_button, LV_ALIGN_BOTTOM_LEFT, 3 * bottombutton_width1, -1 * (bottomHeight + 2));
	lv_obj_add_flag(reset_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(reset_button, bottombutton_width, bottomHeight);
	label = lv_label_create(reset_button);
	lv_label_set_text(label, "Reset");
	lv_obj_center(label);
	lv_obj_add_flag(reset_button, LV_OBJ_FLAG_HIDDEN);

	bfo_button = lv_btn_create(bg_middle);
	lv_obj_add_style(bfo_button, &style_btn, 0);
	lv_obj_add_event_cb(bfo_button, event_button_bfo_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_align(bfo_button, LV_ALIGN_BOTTOM_LEFT, 4 * bottombutton_width1, -1 * (bottomHeight + 2));
	lv_obj_add_flag(bfo_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(bfo_button, bottombutton_width, bottomHeight);
	label = lv_label_create(bfo_button);
	lv_label_set_text(label, "Bfo");
	lv_obj_center(label);
	lv_obj_add_flag(bfo_button, LV_OBJ_FLAG_HIDDEN);

	save_button = lv_btn_create(bg_middle);
	lv_obj_add_style(save_button, &style_btn, 0);
	lv_obj_add_event_cb(save_button, event_button_save, LV_EVENT_CLICKED, NULL);
	lv_obj_align(save_button, LV_ALIGN_BOTTOM_LEFT, 5 * bottombutton_width1, -1 * (bottomHeight + 2));
	lv_obj_add_flag(save_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(save_button, bottombutton_width, bottomHeight);
	label = lv_label_create(save_button);
	lv_label_set_text(label, "Save");
	lv_obj_center(label);
	lv_obj_add_flag(save_button, LV_OBJ_FLAG_HIDDEN);

	/*Set a background color and a radius*/
	lv_style_init(&text_style);
	lv_style_set_radius(&text_style, 5);
	lv_style_set_bg_opa(&text_style, LV_OPA_COVER);
	lv_style_set_bg_color(&text_style, lv_color_black());
	lv_style_set_text_font(&text_style, &FreeSansOblique42);
	lv_style_set_text_color(&text_style, lv_color_white());

	vfo_frequency = lv_label_create(bg_middle);
	lv_obj_add_style(vfo_frequency, &text_style, 0);
	lv_obj_set_width(vfo_frequency, LV_HOR_RES - 20);
	lv_label_set_text(vfo_frequency, "");
	lv_obj_set_height(vfo_frequency, 40);
	lv_obj_align_to(vfo_frequency, bg_middle, LV_ALIGN_CENTER, -30, -84);
	lv_obj_set_style_text_align(vfo_frequency, LV_TEXT_ALIGN_CENTER, 0);

	vfo_unit = lv_label_create(bg_middle);
	lv_obj_add_style(vfo_unit, &text_style, 0);
	lv_label_set_text(vfo_unit, "Khz");
	lv_obj_set_height(vfo_unit, 40);
	lv_obj_align_to(vfo_unit, bg_middle, LV_ALIGN_CENTER, 110, -84);

	/*Set a background color and a radius*/
	lv_style_init(&text_style12);
	lv_style_set_radius(&text_style12, 5);
	lv_style_set_bg_opa(&text_style12, LV_OPA_COVER);
	lv_style_set_bg_color(&text_style12, lv_color_black());
	lv_style_set_text_font(&text_style12, &FreeSansOblique32);
	lv_style_set_text_color(&text_style12, lv_color_make(0x80, 0x80, 0x00));

	vfo2_frequency = lv_label_create(bg_middle);
	lv_obj_add_style(vfo2_frequency, &text_style12, 0);
	lv_obj_set_width(vfo2_frequency, LV_HOR_RES - 20);
	lv_label_set_text(vfo2_frequency, "28.000.00");
	lv_obj_set_height(vfo2_frequency, 40);
	lv_obj_align_to(vfo2_frequency, bg_middle, LV_ALIGN_CENTER, -35, -44);
	lv_obj_set_style_text_align(vfo2_frequency, LV_TEXT_ALIGN_CENTER, 0);

	vfo2_unit = lv_label_create(bg_middle);
	lv_obj_add_style(vfo2_unit, &text_style12, 0);
	lv_label_set_text(vfo2_unit, "Khz");
	lv_obj_set_height(vfo2_unit, 40);
	lv_obj_align_to(vfo2_unit, bg_middle, LV_ALIGN_CENTER, 120, -49);

	bfo_label = lv_label_create(bg_middle);
	lv_obj_set_width(bfo_label, 120);
	lv_label_set_text(bfo_label, "8998.0");
	lv_obj_align_to(bfo_label, NULL, LV_ALIGN_LEFT_MID, 10, -30);

	label_wifi = lv_label_create(bg_top);
	lv_obj_set_width(label_wifi, LV_HOR_RES - 20);
	lv_label_set_text(label_wifi, LV_SYMBOL_WIFI);
	lv_obj_align(label_wifi, LV_ALIGN_TOP_LEFT, 0, 5);
	lv_obj_add_flag(label_wifi, LV_OBJ_FLAG_HIDDEN);
	
	Tx_led1 = lv_led_create(bg_middle);
	lv_obj_align_to(Tx_led1, NULL, LV_ALIGN_CENTER, -125, -70);
	lv_led_off(Tx_led1);
	lv_obj_set_size(Tx_led1, 20, 20);

	lv_obj_t* Tx_ledlabel = lv_label_create(bg_middle);
	lv_obj_align_to(Tx_ledlabel, Tx_led1, LV_ALIGN_OUT_TOP_MID, 7, 0);
	lv_label_set_text(Tx_ledlabel, "TX");
	setfrequencylabel(3600000, 3600000, false);
	
	// Keep de order of the focus correct
	lv_group_add_obj(vfo_group, lsb_button);
	lv_group_add_obj(vfo_group, usb_button);
	lv_group_add_obj(vfo_group, vfo1_button);
	lv_group_add_obj(vfo_group, vfo2_button);
	lv_group_add_obj(vfo_group, band_button);
	lv_group_add_obj(vfo_group, setting_button);

	lv_group_add_obj(vfo_group, wifi1_button);
	lv_group_add_obj(vfo_group, si5351_button);
	lv_group_add_obj(vfo_group, ad8307_button);
	lv_group_add_obj(vfo_group, reset_button);
	lv_group_add_obj(vfo_group, bfo_button);
	lv_group_add_obj(vfo_group, save_button);

	// Set default focus to band selector
	lv_group_focus_obj(lsb_button);
	lv_obj_add_state(lsb_button, LV_STATE_CHECKED);
	init_wifi();
	return;
}

void	Gui::init_wifi()
{
	// Wifi gui
	bg_wifigui = lv_obj_create(scr);
	lv_obj_add_style(bg_wifigui, &mid_style, 0);
	lv_obj_set_pos(bg_wifigui, 0, topHeight);
	lv_obj_set_size(bg_wifigui, LV_HOR_RES, screenHeight - topHeight);
	lv_obj_move_background(bg_wifigui);
	lv_obj_clear_flag(bg_wifigui, LV_OBJ_FLAG_SCROLLABLE);

	static lv_style_t	wifi_style;
	lv_style_init(&wifi_style);
	lv_style_set_radius(&wifi_style, 0);
	lv_style_set_bg_color(&wifi_style, LV_COLOR_ORANGE);
	lv_style_set_border_width(&wifi_style, 0);

	wifi_scan_list = lv_list_create(bg_wifigui);
	//lv_obj_add_style(wifi_scan_list, &wifi_style, 0);
	lv_obj_set_size(wifi_scan_list, 280, 140);
	lv_obj_align(wifi_scan_list, LV_ALIGN_CENTER, 0, -20);

	cancel_btn = lv_btn_create(bg_wifigui);
	lv_obj_align_to(cancel_btn, NULL, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_add_event_cb(cancel_btn, close_event_handler, LV_EVENT_CLICKED, NULL);
	lv_obj_t* label = lv_label_create(cancel_btn);
	lv_label_set_text(label, "Cancel");
	lv_obj_center(label);

	wifi_button = lv_btn_create(bg_top);
	lv_obj_add_event_cb(wifi_button, wifi_button_eh, LV_EVENT_CLICKED, NULL);
	lv_obj_align_to(wifi_button, NULL, LV_ALIGN_CENTER, 170, 10);
	lv_obj_add_flag(wifi_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_add_state(wifi_button, LV_STATE_CHECKED);
	lv_obj_set_size(wifi_button, 40, 20);
	label = lv_label_create(wifi_button);
	lv_label_set_text(label, "Wifi");


	btnm1 = lv_btnmatrix_create(bg_wifigui);
	lv_btnmatrix_set_map(btnm1, btnm_map);
	lv_obj_align_to(btnm1, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_btnmatrix_set_btn_ctrl(btnm1, 6, LV_BTNMATRIX_CTRL_CHECKABLE);
	lv_obj_add_event_cb(btnm1, matrix_event_handler, LV_EVENT_CLICKED, NULL);
	lv_obj_add_flag(btnm1, LV_OBJ_FLAG_HIDDEN);

	pwd_label1 = lv_label_create(bg_wifigui);
	lv_label_set_text(pwd_label1, "ssid slot");
	lv_obj_align_to(pwd_label1, btnm1, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
	lv_obj_add_flag(pwd_label1, LV_OBJ_FLAG_HIDDEN);



	kb = lv_keyboard_create(lv_scr_act());
	lv_obj_set_size(kb, LV_HOR_RES, LV_VER_RES / 2);

	/* Create the password box */
	ta_password = lv_textarea_create(bg_wifigui);
	lv_textarea_set_text(ta_password, "");
	lv_textarea_set_one_line(ta_password, true);
	lv_textarea_set_password_mode(ta_password, true);
	lv_obj_set_width(ta_password, LV_HOR_RES / 2 - 20);
	lv_obj_set_pos(ta_password, 5, 40);
	lv_obj_add_flag(ta_password, LV_OBJ_FLAG_HIDDEN);
	//	lv_obj_set_event_cb(pwd_ta, ta_event_cb);

		/* Create a label and position it above the text box */
	pwd_label = lv_label_create(bg_wifigui);
	lv_label_set_text(pwd_label, "Password:");
	lv_obj_align_to(pwd_label, ta_password, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
	lv_obj_add_flag(pwd_label, LV_OBJ_FLAG_HIDDEN);

	lv_keyboard_set_textarea(kb, ta_password);
	lv_obj_add_event_cb(kb, keyboard_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
	lv_obj_move_background(kb);

	static lv_style_t text_style_ota;
	/*OTA */
	lv_style_init(&text_style_ota);
	lv_style_set_radius(&text_style_ota, 5);
	lv_style_set_bg_opa(&text_style_ota, LV_OPA_COVER);
	lv_style_set_bg_color(&text_style_ota, lv_color_black());
	lv_style_set_text_font(&text_style_ota, &lv_font_montserrat_18);
	lv_style_set_text_color(&text_style_ota, lv_color_make(0x80, 0x80, 0x00));

	bg_wifiota = lv_obj_create(scr);
	lv_obj_add_style(bg_wifiota, &mid_style, 0);
	lv_obj_set_pos(bg_wifiota, 0, topHeight);
	lv_obj_set_size(bg_wifiota, LV_HOR_RES, screenHeight - topHeight - bottomHeight);
	lv_obj_move_background(bg_wifiota);

	ota_label = lv_label_create(bg_wifiota);
	lv_label_set_text(ota_label, "");
	lv_obj_add_style(ota_label, &text_style, 0);
	lv_obj_align_to(ota_label, NULL, LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_move_background(bg_wifiota);

	ota_label1 = lv_label_create(bg_wifiota);
	lv_obj_add_style(ota_label1, &text_style_ota, 0);
	lv_label_set_text(ota_label1, "");
	lv_obj_align_to(ota_label1, NULL, LV_ALIGN_TOP_LEFT, 0, 30);
	lv_obj_move_background(bg_wifiota);

	wifi_group = lv_group_create();
	lv_group_add_obj(wifi_group, wifi_scan_list);
	lv_group_add_obj(wifi_group, cancel_btn);
}

void Gui::init_cal()
{
	bg_middle3 = lv_obj_create(scr);
	lv_obj_add_style(bg_middle3, &mid_style, 0);
	lv_obj_set_pos(bg_middle3, 0, topHeight);
	lv_obj_set_size(bg_middle3, LV_HOR_RES, screenHeight - topHeight);
	lv_obj_clear_flag(bg_middle3, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_hor(bg_middle3, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_ver(bg_middle3, 0, LV_PART_MAIN);
	lv_obj_move_background(bg_middle3);

	si_vfo_button = lv_btn_create(bg_middle3);
	lv_obj_add_style(si_vfo_button, &style_btn, 0);
	lv_obj_add_event_cb(si_vfo_button, si_vfo_button_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_align(si_vfo_button, LV_ALIGN_BOTTOM_LEFT, 0, 0);
	lv_obj_add_flag(si_vfo_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(si_vfo_button, bottombutton_width, bottomHeight);
	lv_obj_t* label = lv_label_create(si_vfo_button);
	lv_label_set_text(label, "VFO");
	lv_obj_add_state(si_vfo_button, LV_STATE_CHECKED);

	si_bfo_button = lv_btn_create(bg_middle3);
	lv_obj_add_style(si_bfo_button, &style_btn, 0);
	lv_obj_add_event_cb(si_bfo_button, si_bfo_button_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_align(si_bfo_button, LV_ALIGN_BOTTOM_LEFT, bottombutton_width1, 0);
	lv_obj_add_flag(si_bfo_button, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_size(si_bfo_button, bottombutton_width, bottomHeight);
	label = lv_label_create(si_bfo_button);
	lv_label_set_text(label, "BFO");
	//lv_obj_add_state(wifi_button, LV_STATE_CHECKED);

	si_save_button = lv_btn_create(bg_middle3);
	lv_obj_add_style(si_save_button, &style_btn, 0);
	lv_obj_add_event_cb(si_save_button, si_save_button_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_align(si_save_button, LV_ALIGN_BOTTOM_LEFT, 2 * bottombutton_width1, 0);
	lv_obj_set_size(si_save_button, bottombutton_width, bottomHeight);
	label = lv_label_create(si_save_button);
	lv_label_set_text(label, "Save");

	si_cancel_button = lv_btn_create(bg_middle3);
	lv_obj_add_style(si_cancel_button, &style_btn, 0);
	lv_obj_add_event_cb(si_cancel_button, si_cancel_button_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_align(si_cancel_button, LV_ALIGN_BOTTOM_LEFT, 3 * bottombutton_width1, 0);
	lv_obj_set_size(si_cancel_button, bottombutton_width, bottomHeight);
	label = lv_label_create(si_cancel_button);
	lv_label_set_text(label, "Cancel");

	si_frequency = lv_label_create(bg_middle3);
	lv_obj_add_style(si_frequency, &text_style, 0);
	lv_obj_set_width(si_frequency, LV_HOR_RES - 20);
	lv_label_set_text(si_frequency, "");
	lv_obj_set_height(si_frequency, 40);
	lv_obj_align_to(si_frequency, bg_middle, LV_ALIGN_CENTER, -30, -84);

	si_unit = lv_label_create(bg_middle3);
	lv_obj_add_style(si_unit, &text_style, 0);
	lv_label_set_text(si_unit, "Khz");
	lv_obj_set_height(si_unit, 40);
	lv_obj_align_to(si_unit, bg_middle, LV_ALIGN_CENTER, 110, -87);

	si_label1 = lv_label_create(bg_middle3);
	lv_label_set_text(si_label1, "Calibrating si5351 VFO");
	lv_obj_align(si_label1, LV_ALIGN_CENTER, 0, 0);

	si_5351_group = lv_group_create();
	lv_group_add_obj(si_5351_group, si_vfo_button);
	lv_group_add_obj(si_5351_group, si_bfo_button);
	lv_group_add_obj(si_5351_group, si_save_button);
	lv_group_add_obj(si_5351_group, si_cancel_button);

	bg_top_cal = lv_obj_create(scr);
	lv_obj_add_style(bg_top_cal, &top_style, 0);
	lv_obj_set_size(bg_top_cal, LV_HOR_RES, topHeight);
	lv_obj_clear_flag(bg_top_cal, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_move_background(bg_top_cal);

	bg_calgui = lv_obj_create(scr);
	lv_obj_add_style(bg_calgui, &mid_style, 0);
	lv_obj_set_pos(bg_calgui, 0, topHeight);
	lv_obj_set_size(bg_calgui, LV_HOR_RES, screenHeight - topHeight);
	lv_obj_clear_flag(bg_calgui, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_hor(bg_calgui, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_ver(bg_calgui, 0, LV_PART_MAIN);
	lv_obj_move_background(bg_calgui);

	label_cal = lv_label_create(bg_top_cal);
	lv_label_set_text(label_cal, "AD8307 Calibration");
	lv_obj_align(label_cal, LV_ALIGN_TOP_MID, 50, 5);

#define XPOS_SPIN		-50
#define POS_SPIN		-80
#define POS_SPIN_DELTA	45

	cal_group = lv_group_create();

	ad_spinbox1 = lv_spinbox_create(bg_calgui);
	lv_spinbox_set_range(ad_spinbox1, 0, 999);
	lv_spinbox_set_digit_format(ad_spinbox1, 3, 2);
	lv_spinbox_step_prev(ad_spinbox1);
	lv_obj_set_width(ad_spinbox1, 50);
	lv_obj_align(ad_spinbox1, LV_ALIGN_CENTER, XPOS_SPIN, POS_SPIN);
	lv_spinbox_set_value(ad_spinbox1, R.cal_AD[0].db10m);

	ad_spinbox2 = lv_spinbox_create(bg_calgui);
	lv_spinbox_set_range(ad_spinbox2, 0, 999);
	lv_spinbox_set_digit_format(ad_spinbox2, 3, 2);
	lv_spinbox_step_prev(ad_spinbox2);
	lv_obj_set_width(ad_spinbox2, 50);
	lv_obj_align(ad_spinbox2, LV_ALIGN_CENTER, XPOS_SPIN, POS_SPIN + POS_SPIN_DELTA);
	lv_spinbox_set_value(ad_spinbox2, R.cal_AD[1].db10m);

	ad_spinbox3 = lv_spinbox_create(bg_calgui);
	lv_spinbox_set_range(ad_spinbox3, 0, 999);
	lv_spinbox_set_digit_format(ad_spinbox3, 3, 0);
	lv_spinbox_step_prev(ad_spinbox3);
	lv_obj_set_width(ad_spinbox3, 50);
	lv_obj_align(ad_spinbox3, LV_ALIGN_CENTER, XPOS_SPIN, POS_SPIN + 2 * POS_SPIN_DELTA);
	lv_spinbox_set_value(ad_spinbox3, R.scale_watt);

	ad_spinbox4 = lv_spinbox_create(bg_calgui);
	lv_spinbox_set_range(ad_spinbox4, 0, 9999);
	lv_spinbox_set_digit_format(ad_spinbox4, 4, 1);
	lv_spinbox_step_prev(ad_spinbox4);
	lv_obj_set_width(ad_spinbox4, 50);
	lv_obj_align(ad_spinbox4, LV_ALIGN_CENTER, XPOS_SPIN, POS_SPIN + 3 * POS_SPIN_DELTA);
	lv_spinbox_set_value(ad_spinbox4, (int32_t)(R.cal_AD[0].Fwd * 1000.0));

	lv_obj_t* pwd_label1 = lv_label_create(bg_calgui);
	lv_label_set_text(pwd_label1, "Ref1:");
	lv_obj_align_to(pwd_label1, ad_spinbox1, LV_ALIGN_OUT_LEFT_MID, -45, 0);

	pwd_label1 = lv_label_create(bg_calgui);
	lv_label_set_text(pwd_label1, "dbm");
	lv_obj_align_to(pwd_label1, ad_spinbox1, LV_ALIGN_OUT_LEFT_MID, -45, 14);

	pwd_label1 = lv_label_create(bg_calgui);
	lv_label_set_text(pwd_label1, "Ref2:");
	lv_obj_align_to(pwd_label1, ad_spinbox2, LV_ALIGN_OUT_LEFT_MID, -45, 0);

	pwd_label1 = lv_label_create(bg_calgui);
	lv_label_set_text(pwd_label1, "dbm");
	lv_obj_align_to(pwd_label1, ad_spinbox2, LV_ALIGN_OUT_LEFT_MID, -45, 14);

	pwd_label1 = lv_label_create(bg_calgui);
	lv_label_set_text(pwd_label1, "Watt:");
	lv_obj_align_to(pwd_label1, ad_spinbox3, LV_ALIGN_OUT_LEFT_MID, -45, 0);

	pwd_label1 = lv_label_create(bg_calgui);
	lv_label_set_text(pwd_label1, "Volt:");
	lv_obj_align_to(pwd_label1, ad_spinbox4, LV_ALIGN_OUT_LEFT_MID, -45, 0);

	lv_coord_t h = lv_obj_get_height(ad_spinbox1);
	lv_obj_t* btn_plus1 = lv_btn_create(bg_calgui);
	lv_obj_set_size(btn_plus1, h, h);
	lv_obj_align_to(btn_plus1, ad_spinbox1, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
	lv_obj_set_style_bg_img_src(btn_plus1, LV_SYMBOL_PLUS, 0);
	lv_obj_add_event_cb(btn_plus1, lv_spinbox_increment_event_ad1, LV_EVENT_ALL, NULL);

	lv_obj_t* btn_min1 = lv_btn_create(bg_calgui);
	lv_obj_align_to(btn_min1, ad_spinbox1, LV_ALIGN_OUT_LEFT_MID, -5, 0);
	lv_obj_add_event_cb(btn_min1, lv_spinbox_decrement_event_ad1, LV_EVENT_ALL, NULL);
	lv_obj_set_style_bg_img_src(btn_min1, LV_SYMBOL_MINUS, 0);

	lv_obj_t* btn_ok1 = lv_btn_create(bg_calgui);
	lv_obj_set_size(btn_ok1, h, h);
	lv_obj_align_to(btn_ok1, ad_spinbox1, LV_ALIGN_OUT_RIGHT_MID, 40, 0);
	lv_obj_set_style_bg_img_src(btn_ok1, LV_SYMBOL_OK, 0);
	lv_obj_add_event_cb(btn_ok1, lv_spinbox_event_save, LV_EVENT_CLICKED, NULL);

	lv_obj_t* btn_plus2 = lv_btn_create(bg_calgui);
	lv_obj_set_size(btn_plus2, h, h);
	lv_obj_align_to(btn_plus2, ad_spinbox2, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
	lv_obj_set_style_bg_img_src(btn_plus2, LV_SYMBOL_PLUS, 0);
	lv_obj_add_event_cb(btn_plus2, lv_spinbox_increment_event_ad2, LV_EVENT_CLICKED, NULL);

	lv_obj_t* btn_min2 = lv_btn_create(bg_calgui);
	lv_obj_align_to(btn_min2, ad_spinbox2, LV_ALIGN_OUT_LEFT_MID, -5, 0);
	lv_obj_add_event_cb(btn_min2, lv_spinbox_decrement_event_ad2, LV_EVENT_CLICKED, NULL);
	lv_obj_set_style_bg_img_src(btn_min2, LV_SYMBOL_MINUS, 0);

	lv_obj_t* btn_ok2 = lv_btn_create(bg_calgui);
	lv_obj_set_size(btn_ok2, h, h);
	lv_obj_align_to(btn_ok2, ad_spinbox2, LV_ALIGN_OUT_RIGHT_MID, 40, 0);
	lv_obj_set_style_bg_img_src(btn_ok2, LV_SYMBOL_OK, 0);
	lv_obj_add_event_cb(btn_ok2, lv_spinbox_event_save1, LV_EVENT_CLICKED, NULL);

	lv_obj_t* btn_refr = lv_btn_create(bg_calgui);
	lv_obj_set_size(btn_refr, h, h);
	lv_obj_align_to(btn_refr, ad_spinbox2, LV_ALIGN_OUT_RIGHT_MID, 80, 0);
	lv_obj_set_style_bg_img_src(btn_refr, LV_SYMBOL_REFRESH, 0);
	lv_obj_add_event_cb(btn_refr, lv_spinbox_event_copy, LV_EVENT_CLICKED, NULL);

	lv_obj_t* btn_save = lv_btn_create(bg_calgui);
	lv_obj_add_style(btn_save, &style_btn, 0);
	lv_obj_add_event_cb(btn_save, mode_button_cal_save, LV_EVENT_CLICKED, 0);
	lv_obj_align(btn_save, LV_ALIGN_BOTTOM_LEFT, 0, 0);
	lv_obj_set_size(btn_save, bottombutton_width, bottomHeight);
	pwd_label1 = lv_label_create(btn_save);
	lv_label_set_text(pwd_label1, "save");
	lv_obj_center(pwd_label1);

	lv_obj_t* btn_cancel = lv_btn_create(bg_calgui);
	lv_obj_add_style(btn_cancel, &style_btn, 0);
	lv_obj_add_event_cb(btn_cancel, mode_button_cal_cancel, LV_EVENT_CLICKED, NULL);
	lv_obj_align(btn_cancel, LV_ALIGN_BOTTOM_LEFT, LV_HOR_RES - bottombutton_width, 0);
	lv_obj_set_size(btn_cancel, bottombutton_width, bottomHeight);
	pwd_label1 = lv_label_create(btn_cancel);
	lv_label_set_text(pwd_label1, "Cancel");
	lv_obj_center(pwd_label1);

	lv_obj_t* btn_plus3 = lv_btn_create(bg_calgui);
	lv_obj_set_size(btn_plus3, h, h);
	lv_obj_align_to(btn_plus3, ad_spinbox3, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
	lv_obj_set_style_bg_img_src(btn_plus3, LV_SYMBOL_PLUS, 0);
	lv_obj_add_event_cb(btn_plus3, lv_spinbox_increment_event_ad3, LV_EVENT_CLICKED, NULL);

	lv_obj_t* btn_min3 = lv_btn_create(bg_calgui);
	lv_obj_align_to(btn_min3, ad_spinbox3, LV_ALIGN_OUT_LEFT_MID, -5, 0);
	lv_obj_add_event_cb(btn_min3, lv_spinbox_decrement_event_ad3, LV_EVENT_CLICKED, NULL);
	lv_obj_set_style_bg_img_src(btn_min3, LV_SYMBOL_MINUS, 0);

	lv_obj_t* btn_plus4 = lv_btn_create(bg_calgui);
	lv_obj_set_size(btn_plus4, h, h);
	lv_obj_align_to(btn_plus4, ad_spinbox4, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
	lv_obj_set_style_bg_img_src(btn_plus4, LV_SYMBOL_PLUS, 0);
	lv_obj_add_event_cb(btn_plus4, lv_spinbox_increment_event_ad4, LV_EVENT_CLICKED, NULL);

	lv_obj_t* btn_min4 = lv_btn_create(bg_calgui);
	lv_obj_align_to(btn_min4, ad_spinbox4, LV_ALIGN_OUT_LEFT_MID, -5, 0);
	lv_obj_add_event_cb(btn_min4, lv_spinbox_decrement_event_ad4, LV_EVENT_CLICKED, NULL);
	lv_obj_set_style_bg_img_src(btn_min4, LV_SYMBOL_MINUS, 0);

	lv_obj_t* btn_ok4 = lv_btn_create(bg_calgui);
	lv_obj_set_size(btn_ok4, h, h);
	lv_obj_align_to(btn_ok4, ad_spinbox4, LV_ALIGN_OUT_RIGHT_MID, 40, 0);
	lv_obj_add_event_cb(btn_ok4, lv_spinbox_event_save4, LV_EVENT_CLICKED, NULL);
	pwd_label1 = lv_label_create(btn_ok4);
	lv_label_set_text(pwd_label1, "R1");
	lv_obj_center(pwd_label1);

	lv_obj_t* btn_ok5 = lv_btn_create(bg_calgui);
	lv_obj_set_size(btn_ok5, h, h);
	lv_obj_align_to(btn_ok5, ad_spinbox4, LV_ALIGN_OUT_RIGHT_MID, 80, 0);
	lv_obj_add_event_cb(btn_ok5, lv_spinbox_event_save5, LV_EVENT_CLICKED, NULL);
	pwd_label1 = lv_label_create(btn_ok5);
	lv_label_set_text(pwd_label1, "R2");
	lv_obj_center(pwd_label1);

	lv_group_add_obj(cal_group, btn_min1);
	lv_group_add_obj(cal_group, ad_spinbox1);
	lv_group_add_obj(cal_group, btn_plus1);
	lv_group_add_obj(cal_group, btn_ok1);

	lv_group_add_obj(cal_group, btn_min2);
	lv_group_add_obj(cal_group, ad_spinbox2);
	lv_group_add_obj(cal_group, btn_plus2);
	lv_group_add_obj(cal_group, btn_ok2);
	lv_group_add_obj(cal_group, btn_refr);

	lv_group_add_obj(cal_group, btn_min3);
	lv_group_add_obj(cal_group, ad_spinbox3);
	lv_group_add_obj(cal_group, btn_plus3);

	lv_group_add_obj(cal_group, btn_min4);
	lv_group_add_obj(cal_group, ad_spinbox4);
	lv_group_add_obj(cal_group, btn_plus4);
	lv_group_add_obj(cal_group, btn_ok4);
	lv_group_add_obj(cal_group, btn_ok5);

	lv_group_add_obj(cal_group, btn_save);
	lv_group_add_obj(cal_group, btn_cancel);
}

void start_ad8307_cal()
{
	stop_measurement();
	gui.f_cal_ad8307 = true;
}

void Gui::cal_foreground()
{
	lv_obj_move_foreground(bg_middle3);
	lv_indev_set_group(encoder_indev_t, si_5351_group);
	gui.si_setcalfrequency(R.correction_si5351_no1);
	lv_obj_add_state(si_vfo_button, LV_STATE_CHECKED);
}

void gui_setup_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_VALUE_CHANGED) {
		
		uint32_t abutton = lv_btnmatrix_get_selected_btn(obj);
		//uint16_t abutton = lv_btnmatrix_get_active_btn(obj);
		switch (abutton)
		{
		case 0:
			gui.show_wifi_gui(true);
			break;
		case 1:
			//Si5351 calibration
			vTaskDelay(100);
			gui.show_cal_screen_si(true);
			start_cal();
			break;
		case 2:
			gui.show_cal_screen_ad(true);
			start_ad8307_cal();
			break;
		case 3:
			memset(&R, 0, sizeof(R));
			init_vfo();
			SaveEEPROM();
			ESP.restart();
			break;
		} 

	}
}

void Gui::gui_setup(lv_obj_t* scr)
{
	static const char* setup_btnm_map[] = { LV_SYMBOL_WIFI, "Si5351", "Calibrate\n AD8307", "Reset", "" };

	bg_middle2 = lv_obj_create(scr);
	lv_obj_add_style(bg_middle2, &mid_style, 0);
	lv_obj_set_pos(bg_middle2, 0, topHeight);
	lv_obj_set_size(bg_middle2, LV_HOR_RES, screenHeight - topHeight);
	lv_obj_clear_flag(bg_middle2, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_hor(bg_middle2, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_ver(bg_middle2, 0, LV_PART_MAIN);
	lv_obj_move_background(bg_middle2);

	btn_matrix = lv_btnmatrix_create(bg_middle2);
	lv_btnmatrix_set_map(btn_matrix, setup_btnm_map);
	lv_obj_align(btn_matrix, LV_ALIGN_CENTER, 0, 0);
	lv_btnmatrix_set_btn_ctrl(btn_matrix, 6, LV_BTNMATRIX_CTRL_CHECKABLE);
	lv_obj_add_event_cb(btn_matrix, gui_setup_event_handler, LV_EVENT_CLICKED, nullptr);
	setup_group = lv_group_create();
	lv_group_add_obj(setup_group, btn_matrix);

	lv_obj_t* pwd_label1 = lv_label_create(bg_middle2);
	lv_label_set_text(pwd_label1, "Setup");
	lv_obj_align_to(pwd_label1, btn_matrix, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
}

extern RotaryEncoder decoder;

void Gui::operator()(void* arg)
{
	char str[80];
	static int val1 = 0, val2 = 0, ii;
	bool b;

	while (1)
	{
		if (enc_irq.flag)
		{
			long mtime, mtime1;
			mtime = mtime1 = millis();
			int count_i = 0;
			while (mtime1 - mtime < 100 && count_i == 0)
			{
				decoder.tick();
				count_i = (int)decoder.getDirection(1);
				if (count_i)
				{
					decoder.ClearEncoder(1);
					sprintf(str, "decoder %d val  %d\n\r", 1, count_i);
					Serial.print(str);
				}
				mtime1 = millis();
			}
			enc_irq.flag = false;
		}
		
/*		int count_i = (int)decoder.getDirection(1);
		if (count_i)
		{
			decoder.ClearEncoder(1);
			sprintf(str, "decoder %d val  %d\n\r", 1, count_i);
			Serial.print(str);
		}
*/
		xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
		lv_timer_handler();
		xSemaphoreGive(GuiBinarySemaphore);
		
		SmeterBar.value(get_smeter());

		if ((f_cal_si5351 == false) && (f_bfo_adjust == false))  // Both si5351 and bfo adjust use optical encoder, so don't change vfo freq at the same time
		{
			int count = Enc_vfo.getCount();
			if (count != 0)
			{
				/*
				* Translate the encoder movement to a new frequency
				* If the band is changed also update the band roller
				*/
				char str[32];
				long frq = set_encoder_count_to_vfo_frequency(count, active_vfo);
				Enc_vfo.clearCount();
				setfrequencylabel(frq, get_vfo_frequency(1 - active_vfo));
				uint16_t i = get_band(str);
				lv_label_set_text(Band_btn_label, str);
			}
		}
		
		if (f_bfo_adjust)
		{
			int count = Enc_vfo.getCount();
			if (count != 0)
			{
				/*
				* Translate the encoder movement to a new bfo frequency
				* If the band is changed also update the band roller
				*/
				char str[32];
				long freq = 0L;
				Enc_vfo.clearCount();
				freq = set_encoder_count_to_bfo_frequency(count);
				setbfolabel(freq);
			}
		}

		// Calibrate si5351

		if (f_cal_si5351 == true)
		{
			int count = Enc_vfo.getCount();
			if (count != 0)
			{
				/*
				* Translate the encoder movement to a new frequency
				* If the band is changed also update the band roller
				*/
				char str[32];
				long freq = 0L;
				Enc_vfo.clearCount();
				if (f_cal_si5351_vfo)
				{
					R.correction_si5351_no1 += count * 10;
					if (R.correction_si5351_no1 < -900000L) R.correction_si5351_no1 = -900000L;
					if (R.correction_si5351_no1 > 900000L) R.correction_si5351_no1 = 0;
					freq = R.correction_si5351_no1;
					cal_vfo();
				}
				else
				{
					R.correction_si5351_no2 += count;
					if (R.correction_si5351_no2 < -900000L) R.correction_si5351_no2 = -900000L;
					if (R.correction_si5351_no2 > 900000L) R.correction_si5351_no2 = 0;
					freq = R.correction_si5351_no2;
					cal_bfo();
				}
				si_setcalfrequency(freq);
			}
		}
		
		if (f_cal_ad8307)
		{
			uint8_t inputq = check_input_cal();
			lv_color_t color_t = LV_COLOR_RED;
			String	sStr;

			switch (inputq)
			{
			case CAL_FWD:
				color_t = LV_COLOR_GREEN;
				sStr = String("Power Ok:");
				break;
			case CAL_REV:
				color_t = LV_COLOR_ORANGE;
				sStr = String("Power rev:");
				break;
			case CAL_BAD:
				color_t = LV_COLOR_RED;
				sStr = String("Power Low:");
				break;
			}
			uint16_t fwd, rev;

			fwd = analogRead(FWD_METER);
			rev = analogRead(REV_METER);
			updateBottomStatus(color_t, String(sStr + " Fwd: " + String(adc_ref * ((double)fwd / 4096.0)) + "V Rev: " + String(adc_ref * ((double)rev / 4096.0))).c_str(), 0);
		}

		CheckCAT();
		check_rx_tx(); 
		vTaskDelay(1);
	}
}

void Gui::set_time_label()
{
	std::time_t result = std::time(nullptr);
	std::string s = std::asctime(std::localtime(&result));
	lv_label_set_text(label_date_time, s.c_str());
}

void mode_button_eh(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	bool	bchecked = lv_obj_get_state(obj) & LV_STATE_CHECKED;
	if (code == LV_EVENT_CLICKED)
	{
		if (obj == gui.get_usb_button())
		{
			if (bchecked)
			{
				lv_obj_clear_state(gui.get_lsb_button(), LV_STATE_CHECKED);
				switch_mode(MODE_USB, gui.active_vfo);
			}
			else
			{
				lv_obj_add_state(gui.get_lsb_button(), LV_STATE_CHECKED);
				switch_mode(MODE_LSB, gui.active_vfo);
			}
		}
		if (obj == gui.get_lsb_button())
		{
			if (bchecked)
			{
				lv_obj_clear_state(gui.get_usb_button(), LV_STATE_CHECKED);
				switch_mode(MODE_LSB, gui.active_vfo);
			}
			else
			{
				lv_obj_add_state(gui.get_usb_button(), LV_STATE_CHECKED);
				switch_mode(MODE_USB, gui.active_vfo);
			}
		}
	}
}

void event_button_bfo_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED)
	{
		bool bchecked = lv_obj_get_state(obj) & LV_STATE_CHECKED;
		gui.f_bfo_adjust = bchecked;
	}
}

void event_button_save(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
		save_vfo();
		SaveEEPROM();
	}
}

void event_button_wifi_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED) {
		gui.show_wifi_gui(true);
	}
}

void wifi_button_eh(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED) {
		//printf("Clicked\n");
	}
	else if (code == LV_EVENT_VALUE_CHANGED) {
		lv_state_t state = lv_obj_get_state(obj);
		if (state == LV_STATE_CHECKED || state == LV_EVENT_RELEASED)
		{
			uint8_t	message = 1;

			R.wifi_onoff = 1;
			xQueueSend(net_queue, &message, 0);
		}
		else
		{
			uint8_t	message = 0;

			R.wifi_onoff = 0;
			xQueueSend(net_queue, &message, 0);
		}
	}
}

void event_button_setting_cb(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED)
	{
		bool bchecked = lv_obj_get_state(obj) & LV_STATE_CHECKED;
		gui.f_bfo_adjust = false;
		gui.hide_setting(!bchecked);
	}
}

void Gui::hide_buttons(bool hide)
{
	if (hide)
	{
		lv_obj_add_flag(usb_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(lsb_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(vfo1_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(vfo2_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(band_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(setting_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(bfo_label, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(vfo2_frequency, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(vfo2_unit, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_clear_flag(usb_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(lsb_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(vfo1_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(vfo2_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(band_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(setting_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(bfo_label, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(vfo2_frequency, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(vfo2_unit, LV_OBJ_FLAG_HIDDEN);
	}
}

void Gui::hide_setting(bool hide)
{
	//smeter->hide(!hide);
	if (hide)
	{
		lv_obj_add_flag(save_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(wifi1_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(si5351_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(ad8307_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(reset_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(bfo_button, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		lv_obj_clear_flag(save_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(wifi1_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(si5351_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(ad8307_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(reset_button, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(bfo_button, LV_OBJ_FLAG_HIDDEN);
	}
	lv_obj_clear_state(bfo_button, LV_STATE_CHECKED);
}

// this function is used to set the display frequency, it is not using semaphore because this is done from main loop
void Gui::setfrequencylabel(long freq, long freq2, uint8_t sem) {
	if (sem) xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	char str[80];

	sprintf(str, "%3d.%03d,%02d", freq / 1000000, (freq / 1000) % 1000, (freq / 10) % 100);
	lv_label_set_text(vfo_frequency, str);

	sprintf(str, "%3d.%03d,%02d", freq2 / 1000000, (freq2 / 1000) % 1000, (freq2 / 10) % 100);
	lv_label_set_text(vfo2_frequency, str);
	if (sem) xSemaphoreGive(GuiBinarySemaphore);
}

void Gui::setbfolabel(long freq, uint8_t sem) {
	if (sem) xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	char str[80];

	sprintf(str, "Bfo:%2d.%03d,%02d Khz", freq / 1000000, (freq / 1000) % 1000, (freq / 10) % 100);
	lv_label_set_text(bfo_label, str);
	if (sem) xSemaphoreGive(GuiBinarySemaphore);
}

void Gui::updateBottomStatus(lv_color_t color, String text, uint8_t sem) {
	if (sem) xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	lv_obj_set_style_bg_color(bg_top, color,0);
	lv_label_set_text(label_status, text.c_str());
	if (sem) xSemaphoreGive(GuiBinarySemaphore);
}

void Gui::show_ota_gui(int show, uint8_t sem)
{
	if (sem)
		xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	if (show)
	{
		lv_obj_move_foreground(bg_wifiota);
		//xTaskCreate(wifi_scan_network, "wifi_scan", 4096, NULL, 1, &hWifiTask);
	}
	else
	{
		//vTaskDelete(hWifiTask);
		lv_obj_move_background(bg_wifiota);
		lv_obj_move_foreground(bg_middle);
	}
	if (sem)
		xSemaphoreGive(GuiBinarySemaphore);
}

void Gui::set_ota_label(String str, uint8_t sem)
{
	if (sem)
		xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	lv_label_set_text(ota_label, str.c_str());
	if (sem)
		xSemaphoreGive(GuiBinarySemaphore);
}

void Gui::set_ota_label1(String str, uint8_t sem)
{
	if (sem)
		xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	lv_label_set_text(ota_label1, str.c_str());
	if (sem)
		xSemaphoreGive(GuiBinarySemaphore);
}

void Gui::hide_matrix()
{
	lv_obj_add_flag(btnm1, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(pwd_label1, LV_OBJ_FLAG_HIDDEN);
	lv_obj_move_background(bg_wifigui);
	lv_obj_move_foreground(bg_middle);
}

void matrix_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	uint32_t id = lv_btnmatrix_get_selected_btn(obj);
	const char* txt = lv_btnmatrix_get_btn_text(obj, id);

	uint8_t i = atoi(txt) - 1;
	if (i > 5) i = 0;

	gui.ssidName.toCharArray(R.ssid[i], 30);
	gui.password.toCharArray(R.password[i], 60);
	gui.hide_matrix();

	// Start Wifi Network
	uint8_t	message = 1;

	R.wifi_onoff = 1;
	xQueueSend(net_queue, &message, 0);
}

void Gui::keyboard_ready()
{
	password = String(lv_textarea_get_text(ta_password));
	lv_obj_clear_flag(ta_password, LV_OBJ_FLAG_HIDDEN);
	lv_obj_clear_flag(pwd_label, LV_OBJ_FLAG_HIDDEN);
	lv_obj_clear_flag(btnm1, LV_OBJ_FLAG_HIDDEN);
	lv_obj_clear_flag(pwd_label1, LV_OBJ_FLAG_HIDDEN);
}

void keyboard_event_cb(lv_event_t* e) {
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_READY) {
		{
			lv_obj_move_background(obj);
			gui.keyboard_ready();
		}

	}
	else if (code == LV_EVENT_CANCEL) {
		lv_obj_move_background(obj);
	}
}

void close_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED) {
		gui.show_wifi_gui(false);
	}
}

void ota_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED) {
		gui.show_ota_gui(false);
	}
}

void Gui::clean_wifi_list()
{
	xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	lv_obj_clean(wifi_scan_list);
	xSemaphoreGive(GuiBinarySemaphore);
}

void Gui::popupPW(String ssidName) {
	if (ssidName == NULL || ssidName.length() == 0) {
		return;
	}
	char str[80];

	lv_textarea_set_text(ta_password, "");
	// lv_msgbox_set_text(mbox_connect, ssidName.c_str());
	strcpy(str, "Password for ");
	strcat(str, ssidName.c_str());
	lv_label_set_text(pwd_label, str);
	lv_obj_clear_flag(ta_password, LV_OBJ_FLAG_HIDDEN);
	lv_obj_clear_flag(pwd_label, LV_OBJ_FLAG_HIDDEN);
	lv_obj_move_foreground(kb);
	lv_keyboard_set_textarea(kb, ta_password);
}

void Gui::Save_wifi(const char* buf)
{
	char	str[80];

	memset(str, 0, 80);
	strncpy(str, buf, 79);
	char* ptr = strrchr(str, '(');
	if (ptr) {
		ptr--;
		*ptr = '\0';
	}
	lv_obj_add_flag(wifi_scan_list, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(cancel_btn, LV_OBJ_FLAG_HIDDEN);
	ssidName = String(str);
	gui.popupPW(String(str));
}

void wifilist_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED) {
		lv_obj_t* parent = lv_obj_get_parent(obj);
		const char* txt = lv_list_get_btn_text(parent, obj);
		gui.Save_wifi(txt);
	}
}

void Gui::add_wifi_list(String item)
{
	xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	lv_obj_t* list_btn = lv_list_add_btn(wifi_scan_list, LV_SYMBOL_WIFI, item.c_str());
	lv_obj_add_event_cb(list_btn, wifilist_event_handler, LV_EVENT_CLICKED, NULL);
	xSemaphoreGive(GuiBinarySemaphore);
}

void wifi_scan_network(void* pvParameters)
{
	vTaskDelay(1000);
	while (1)
	{
		gui.updateBottomStatus(LV_COLOR_ORANGE, "Searching Available WIFI ...");
		int n = WiFi.scanNetworks();
		if (n <= 0) {
			gui.updateBottomStatus(LV_COLOR_RED, "Sorry no networks found!");
			vTaskDelay(5);
		}
		else
		{
			gui.clean_wifi_list();
			vTaskDelay(10);
			for (int i = 0; i < n; ++i)
			{
				String item = WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ") ";
				switch (WiFi.encryptionType(i))
				{
				case WIFI_AUTH_OPEN:
					break;
				case WIFI_AUTH_WEP:
					item = item + String(" WEP");
					break;
				case WIFI_AUTH_WPA_WPA2_PSK:
				case WIFI_AUTH_WPA_PSK:
					item = item + String(" WPA");
					break;
				case WIFI_AUTH_WPA2_PSK:
				case WIFI_AUTH_WPA2_ENTERPRISE:
					item = item + String(" WPA2");
					break;
				default:
					item = item + String(" ***");
					break;
				}
				gui.add_wifi_list(item);
				vTaskDelay(10);
			}
			gui.updateBottomStatus(LV_COLOR_GREEN, String(n) + " networks found!");
		}
		vTaskDelete(hWifiTask);
		hWifiTask = nullptr;
	}
}

void Gui::show_wifi_gui(int show)
{
	if (show)
	{
		lv_obj_move_foreground(bg_wifigui);
		lv_indev_set_group(encoder_indev_t, wifi_group);
		lv_obj_clear_flag(wifi_scan_list, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(cancel_btn, LV_OBJ_FLAG_HIDDEN);
		xTaskCreate(wifi_scan_network, "wifi_scan", 4096, NULL, 1, &hWifiTask);
	}
	else
	{
		lv_obj_move_background(bg_wifigui);
		lv_obj_move_foreground(bg_middle);
		lv_indev_set_group(encoder_indev_t, vfo_group);
	}
}

void Gui::showWifilabel(bool show) {
	xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	if (show)
		lv_obj_clear_flag(label_wifi, LV_OBJ_FLAG_HIDDEN);
	else
		lv_obj_add_flag(label_wifi, LV_OBJ_FLAG_HIDDEN);
	xSemaphoreGive(GuiBinarySemaphore);
}

void Gui::ToggleSetup(bool show, uint8_t sem)
{
	if (sem) xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	if (show)
	{
		lv_obj_move_foreground(bg_middle2);
		lv_indev_set_group(encoder_indev_t, setup_group);
		lv_group_focus_obj(btn_matrix);
	}
	else
	{
		lv_obj_move_background(bg_middle2);
		lv_indev_set_group(encoder_indev_t, vfo_group);
		lv_group_focus_obj(lsb_button);
	}
	if (sem) xSemaphoreGive(GuiBinarySemaphore);
}

void Gui::BfoLabel(uint32_t num, uint8_t sem) {
	if (sem)
		xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	char str[32];
	sprintf(str, "Bfo: %ld.%ld Khz", num / 10, num % 10);
	lv_label_set_text(bfo_label, str);
	//lv_spinbox_set_value(bfo_spinbox, num);
	if (sem)
		xSemaphoreGive(GuiBinarySemaphore);
}

void Gui::Togglemode(int mode, uint8_t sem)
{
	if (sem)
		xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	if (mode == MODE_LSB)
	{
		lv_obj_add_state(lsb_button, LV_STATE_CHECKED);
		lv_obj_clear_state(usb_button, LV_STATE_CHECKED);
	}
	if (mode == MODE_USB)
	{
		lv_obj_add_state(usb_button, LV_STATE_CHECKED);
		lv_obj_clear_state(lsb_button, LV_STATE_CHECKED);
	}
	if (sem)
		xSemaphoreGive(GuiBinarySemaphore);
}

void Gui::ToggleVfo(int active_vfo, uint8_t sem)
{
	if (sem) xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	if (active_vfo)
	{
		lv_obj_add_state(vfo2_button, LV_STATE_CHECKED);
		lv_obj_clear_state(vfo1_button, LV_STATE_CHECKED);
	}
	else
	{
		lv_obj_add_state(vfo1_button, LV_STATE_CHECKED);
		lv_obj_clear_state(vfo2_button, LV_STATE_CHECKED);
	}
	if (sem) xSemaphoreGive(GuiBinarySemaphore);
}

void Gui::ToggleTX(uint8_t show) {
	xSemaphoreTake(GuiBinarySemaphore, portMAX_DELAY);
	if (show)
		lv_led_on(Tx_led1);
	else
		lv_led_off(Tx_led1);
	xSemaphoreGive(GuiBinarySemaphore);
}


void event_button_reset_cb(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED) {
		memset(&R, 0, sizeof(R));
		init_vfo();
		SaveEEPROM();
		ESP.restart();
	}
}

void mode_button_vfo(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED /*LV_EVENT_VALUE_CHANGED*/) {

		bool bchecked = lv_obj_get_state(obj) & LV_STATE_CHECKED;
		//Serial.println(String("state vfo checked:") + String(bchecked));

		if (obj == gui.get_vfo1_button())
		{
			if (bchecked)
			{
				lv_obj_clear_state(gui.get_vfo2_button(), LV_STATE_CHECKED);
				gui.active_vfo = 0;
			}
			else
			{
				lv_obj_add_state(gui.get_vfo2_button(), LV_STATE_CHECKED);
				gui.active_vfo = 1;
			}
		}
		if (obj == gui.get_vfo2_button())
		{
			if (bchecked)
			{
				lv_obj_clear_state(gui.get_vfo1_button(), LV_STATE_CHECKED);
				gui.active_vfo = 1;
			}
			else
			{
				lv_obj_add_state(gui.get_vfo1_button(), LV_STATE_CHECKED);
				gui.active_vfo = 0;
			}
		}
		long frq = switch_vfo(gui.active_vfo);
		gui.setfrequencylabel(frq, get_vfo_frequency(1 - gui.active_vfo));
	}
}

void event_band_button(lv_event_t *e)
	{
		lv_event_code_t code = lv_event_get_code(e);
		lv_obj_t* obj = lv_event_get_target(e); 
		
		if (code == LV_EVENT_CLICKED) {
			char	buf[80];
			long	frq;

			strcpy(buf, lv_label_get_text(gui.get_Band_btn_label()));
			buf[strlen(buf) - 1] = '\0';
			frq = band_select_next(buf, gui.active_vfo);
			gui.setfrequencylabel(frq, get_vfo_frequency(1 - gui.active_vfo));
			set_vfo_frequency(frq, gui.active_vfo);
			lv_label_set_text(gui.get_Band_btn_label(), buf);
		}
	}

void Gui::si_setcalfrequency(long freq)
{
	char	str[80];

	if (freq < 0)
		sprintf(str, "-%ld,%03ld", abs(freq) / 1000L, (abs(freq)) % 1000L);
	else
		sprintf(str, "%ld,%03ld", abs(freq) / 1000L, (abs(freq)) % 1000L);
	lv_label_set_text(si_frequency, str);
}

void event_button_si5351_cb(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED) {
		vTaskDelay(100);
		gui.show_cal_screen_si(true);
		start_cal();
	}
}

void Gui::show_cal_screen_ad(bool show)
{
	if (show)
	{
		lv_obj_move_foreground(bg_calgui);
		lv_obj_move_foreground(bg_top_cal);
		lv_indev_set_group(encoder_indev_t, cal_group);
		f_cal_ad8307 = true;  // check if this is correct
	}
	else
	{
		lv_obj_move_background(bg_calgui);
		lv_obj_move_foreground(bg_middle);
		lv_obj_move_background(bg_top_cal);
		lv_obj_move_foreground(bg_top);
		f_cal_ad8307 = false;
		lv_indev_set_group(encoder_indev_t, vfo_group);
	}
}

void event_button_ad8307_cb(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED) {
		gui.show_cal_screen_ad(true);
		start_ad8307_cal();
	}
}

void Gui::set_si_label(char* str, bool state)
{
	lv_label_set_text(si_label1, str);
	if (state)
	{
		lv_obj_add_state(si_bfo_button, LV_STATE_CHECKED);
		f_cal_si5351_vfo = true;
	}
	else
	{
		lv_obj_clear_state(si_bfo_button, LV_STATE_CHECKED);
		f_cal_si5351_vfo = false;
	}
}

void si_vfo_button_cb(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED)
	{
		bool bchecked = lv_obj_get_state(obj) & LV_STATE_CHECKED;
		if (obj == gui.get_si_vfo_button())
		{
			if (bchecked)
			{
				gui.set_si_label("Calibrating si5351 VFO", true);
				gui.si_setcalfrequency(R.correction_si5351_no1);
				cal_vfo();
			}
			else
			{
				
				gui.set_si_label("Calibrating si5351 BFO", false);
				gui.si_setcalfrequency(R.correction_si5351_no2);
				cal_bfo();
			}
		}
	}
}

void si_bfo_button_cb(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED)
	{
		bool bchecked = lv_obj_get_state(obj) & LV_STATE_CHECKED;
		if (obj == gui.get_si_bfo_button())
		{
			if (bchecked)
			{
				gui.set_si_label("Calibrating si5351 BFO", false);
				gui.si_setcalfrequency(R.correction_si5351_no2);
				cal_bfo();
			}
			else
			{
				gui.set_si_label("Calibrating si5351 VFO", true);
				gui.si_setcalfrequency(R.correction_si5351_no1);
				cal_vfo();
			}
		}
	}
}

void mode_button_cal_cancel(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED) {
		gui.show_cal_screen_ad(false);
		start_measurement();
	}
}

void mode_button_cal_save(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_CLICKED) {
		gui.show_cal_screen_ad(false);
		Serial.println("db10m[0]: " + String(R.cal_AD[0].db10m) + " Fwd[0]: " + String(R.cal_AD[0].Fwd) + " Rev[0]: " + String(R.cal_AD[0].Rev));
		Serial.println("db10m[1]: " + String(R.cal_AD[1].db10m) + " Fwd[1]: " + String(R.cal_AD[1].Fwd) + " Rev[1]: " + String(R.cal_AD[1].Rev));

		DebugServer.print(0, "db10m[0]: " + String(R.cal_AD[0].db10m) + " Fwd[0]: " + String(R.cal_AD[0].Fwd) + " Rev[0]: " + String(R.cal_AD[0].Rev) + "\n\r");
		DebugServer.print(0, "db10m[1]: " + String(R.cal_AD[1].db10m) + " Fwd[1]: " + String(R.cal_AD[1].Fwd) + " Rev[1]: " + String(R.cal_AD[1].Rev) + "\n\r");

		start_measurement();
		SaveEEPROM();
	}
}
