// gui.h

#ifndef _gui_h
#define _gui_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <TFT_eSPI.h>
#include <RotaryEncoder.h>
#include <lvgl.h>
#include <ESP32Encoder.h>
#include "SwrBarClass.h"

#define LV_COLOR_WHITE   LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)
#define LV_COLOR_SILVER  LV_COLOR_MAKE(0xC0, 0xC0, 0xC0)
#define LV_COLOR_GRAY    LV_COLOR_MAKE(0x80, 0x80, 0x80)
#define LV_COLOR_BLACK   LV_COLOR_MAKE(0x00, 0x00, 0x00)
#define LV_COLOR_RED     LV_COLOR_MAKE(0xFF, 0x00, 0x00)
#define LV_COLOR_MAROON  LV_COLOR_MAKE(0x80, 0x00, 0x00)
#define LV_COLOR_YELLOW  LV_COLOR_MAKE(0xFF, 0xFF, 0x00)
#define LV_COLOR_OLIVE   LV_COLOR_MAKE(0x80, 0x80, 0x00)
#define LV_COLOR_LIME    LV_COLOR_MAKE(0x00, 0xFF, 0x00)
#define LV_COLOR_GREEN   LV_COLOR_MAKE(0x00, 0x80, 0x00)
#define LV_COLOR_CYAN    LV_COLOR_MAKE(0x00, 0xFF, 0xFF)
#define LV_COLOR_AQUA    LV_COLOR_CYAN
#define LV_COLOR_TEAL    LV_COLOR_MAKE(0x00, 0x80, 0x80)
#define LV_COLOR_BLUE    LV_COLOR_MAKE(0x00, 0x00, 0xFF)
#define LV_COLOR_NAVY    LV_COLOR_MAKE(0x00, 0x00, 0x80)
#define LV_COLOR_MAGENTA LV_COLOR_MAKE(0xFF, 0x00, 0xFF)
#define LV_COLOR_PURPLE  LV_COLOR_MAKE(0x80, 0x00, 0x80)
#define LV_COLOR_ORANGE  LV_COLOR_MAKE(0xFF, 0xA5, 0x00)

class Gui
{
protected:


public:
	void	init();
	void	gui_loop();
	void	set_time_label();
	void	hide_buttons(bool hide);
	void	hide_setting(bool hide);
	void	setfrequencylabel(long freq, long freq2, uint8_t sem = 0);
	void	setbfolabel(long freq, uint8_t sem = 0);
	void	updateBottomStatus(lv_color_t color, String text, uint8_t sem = 1);
	void	operator()(void* arg);
	void	gui_setup();
	void	show_ota_gui(int show, uint8_t sem = 1);
	void	set_ota_label(String str, uint8_t sem = 1);
	void	set_ota_label1(String str, uint8_t sem = 1);
	void	hide_matrix();
	void	keyboard_ready();
	void	show_wifi_gui(int show);
	void	clean_wifi_list();
	void	add_wifi_list(String item);
	void	Save_wifi(const char* buf);
	void	popupPW(String ssidName);
	void	showWifilabel(bool show);
	void	ToggleVfo(int active_vfo, uint8_t sem = 1);
	void	Togglemode(int mode, uint8_t sem = 1);
	void	BfoLabel(uint32_t num, uint8_t sem);
	void	ToggleSetup(bool show, uint8_t sem = 1);
	void	gui_setup(lv_obj_t* scr);
	void	ToggleTX(uint8_t show);
	lv_obj_t* get_lsb_button() { return lsb_button; }
	lv_obj_t* get_usb_button() { return usb_button; }
	lv_obj_t* get_vfo1_button() { return vfo1_button; }
	lv_obj_t* get_vfo2_button() { return vfo2_button; }
	lv_obj_t* get_Band_btn_label() { return Band_btn_label; }
	void	cal_foreground();
	void	si_setcalfrequency(long freq);
	void	set_si_label(char* str, bool state);
	lv_obj_t* get_si_vfo_button() { return si_vfo_button; }
	lv_obj_t* get_si_bfo_button() { return si_bfo_button; }
	void	show_cal_screen_ad(bool show);
	void	show_cal_screen_si(bool show);
	String	ssidName, password;

	bool f_cal_ad8307{ false };
	bool f_cal_si5351{ false };
	bool f_cal_si5351_vfo{ true };
	bool show_setting{ false };
	bool f_bfo_adjust{ false };
	int		active_vfo;

	lv_obj_t* ad_spinbox1;
	lv_obj_t* ad_spinbox2;
	lv_obj_t* ad_spinbox3;
	lv_obj_t* ad_spinbox4;
	lv_obj_t* ad_spinbox5;


private:
	lv_style_t	top_style;
	lv_style_t	mid_style;
	lv_style_t	style_btn;
	lv_style_t	text_style;
	lv_style_t	text_style12;
	lv_obj_t*	scr;
	lv_obj_t*	bg_top;
	lv_obj_t*	bg_middle;
	lv_obj_t*	bg_wifigui;
	lv_obj_t*	bg_middle3;
	lv_obj_t*	label_date_time;
	lv_group_t* vfo_group;
	lv_indev_t* encoder_indev_t;
	lv_obj_t*	lsb_button;
	lv_obj_t* usb_button;
	lv_obj_t* vfo1_button;
	lv_obj_t* vfo2_button;
	lv_obj_t* band_button;
	lv_obj_t* setting_button;
	lv_obj_t* wifi1_button;
	lv_obj_t* si5351_button;
	lv_obj_t* ad8307_button;
	lv_obj_t* reset_button;
	lv_obj_t* bfo_button;
	lv_obj_t* save_button;
	SemaphoreHandle_t GuiBinarySemaphore {nullptr};
	lv_obj_t* vfo_frequency;
	lv_obj_t* vfo_unit;
	lv_obj_t* vfo2_frequency;
	lv_obj_t* vfo2_unit;
	lv_obj_t* bfo_label;
	lv_obj_t* label_wifi;
	lv_obj_t* Tx_led1;
	lv_obj_t* label_status;
	lv_obj_t* wifi_scan_list;
	lv_obj_t* cancel_btn;
	lv_obj_t* wifi_button;
	lv_obj_t* btnm1;
	lv_obj_t* pwd_label1;
	lv_obj_t* kb;
	lv_obj_t* ta_password;
	lv_obj_t* pwd_label;
	lv_obj_t* ota_label;
	lv_obj_t* ota_label1;
	lv_obj_t* bg_wifiota;
	lv_group_t* wifi_group;
	lv_obj_t* ddlist;
	lv_obj_t* Band_btn_label;
	lv_obj_t* si_frequency;
	lv_obj_t* si_vfo_button;
	lv_obj_t* si_bfo_button;
	lv_obj_t* si_save_button;
	lv_obj_t* si_cancel_button;
	lv_obj_t* si_unit;
	lv_obj_t* si_label1;
	lv_group_t* si_5351_group;

	lv_obj_t* smeterLabel;
	SwrBarClass SmeterBar;

	lv_obj_t* bg_middle2;
	lv_obj_t* btn_matrix;
	lv_group_t* setup_group;

	lv_obj_t* bg_top_cal;
	lv_obj_t* bg_calgui;
	lv_obj_t* label_cal;
	lv_group_t* cal_group;
	

	ESP32Encoder    Enc_vfo;
	long			freq{ 3600000 };
	int				freq_step{ 100 };
	void			init_wifi();
	void			init_cal();
	

};

extern Gui		gui;
extern TFT_eSPI tft;

extern void SaveEEPROM();

#endif
