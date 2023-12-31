#include <arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include "measurement.h"
#include "network.h"

volatile adbuffer_t measure;// Two 256 byte circular buffers carrying the sampled adc-inputs
                            // from the interrupt function to the main loop.
                            
double      adc_ref;        // ADC reference (Teensy or external AD7991)
uint16_t      fwd;            // AD input - 12 bit value, v-forward
uint16_t      rev;            // AD input - 12 bit value, v-reverse
double      f_inst;         // Calculated Forward voltage
double      r_inst;         // Calculated Reverse voltagedouble      
double      ad8307_FdBm;    // Measured AD8307 forward voltage in dBm
double      ad8307_RdBm;    // Measured AD8307 reverse current in dBm
double      fwd_power_mw;   // Calculated forward power in mW
double      ref_power_mw;   // Calculated reflected power in mW
double      power_mw;       // Calculated power in mW
double      power_mw_pk;    // Calculated 100ms peak power in mW
double      power_mw_pep;   // Calculated PEP power in mW (1s, 2.5s or 5s)
double      power_mw_long;  // Calculated MAX power in mW, 30 sec or longer window
double      power_mw_avg;   // Calculated AVG power in mW, 100ms
double      power_mw_1savg; // Calculated AVG power in mW, 1 second
double      power_db;       // Calculated power in dBm
double      power_db_pk;    // Calculated 100ms peak power in dBm
double      power_db_pep;   // Calculated PEP power in dBm
double      power_db_long;  // Calculated MAX power in dBm, 30 sec or longer window
double      swr=1.0;        // SWR as an absolute value
double      swr_avg=1.0;    // SWR average over 10 ms (smoothed value)
double      rev_power_db = 0.0;

bool        Reverse;        // BOOL: True if reverse power is greater than forward power
char        lcd_buf[82];    // Used to process data to be passed to LCD and USB Serial

var_t  R;

void adc_init(void)
{ 
  analogSetAttenuation(ADC_11db);
  analogReadResolution(12);
  adc_ref = ADC_RES;  
}

TaskHandle_t    Task_poll;

void start_measurement()
{
    adc_init(); 
    memset((void *)&measure, 0, sizeof(adbuffer_t));
    memset((void*)lcd_buf, 0, 82);
    xTaskCreatePinnedToCore(adc_poll_and_feed_circular, "adc_poll", 4096, NULL, 1, &Task_poll, 0);
}

void stop_measurement()
{
    vTaskDelete(Task_poll);
}

int lastRandomEncoding = 0;
void adc_poll_and_feed_circular(void* pvParameters)
{
  //----------------------------------------------------------------------------
  // use builtin A/D converters (12 bit resolution)
  // Each poll of the builtin ADC is measured to take 130us if averaging set at 8.
    while(1)
    {
        int16_t result_fwd;
        int16_t result_ref;

        int currMillis = millis();
        if (currMillis - lastRandomEncoding > 1500)
        {

            result_fwd = (int16_t)random(1000, 2800); // analogRead(Pfwd);
            result_ref = (int16_t)random(1500, 2000); // analogRead(Pref);  // ref=ADC0, fwd=ADC1
            lastRandomEncoding = currMillis;
        }
        //result_fwd = analogRead(Pfwd);
        //result_ref = analogRead(Pref);  // ref=ADC0, fwd=ADC1
        xSemaphoreTake(swrBinarySemaphore, portMAX_DELAY);
        measure.fwd[measure.incount] = result_fwd; //analogRead(FWD_METER);
        measure.rev[measure.incount] = result_ref; // analogRead(REV_METER);
        measure.incount++;                        // 8 bit value, rolls over at 256
        xSemaphoreGive(swrBinarySemaphore);
        //Serial.println("fwd:" + String(measure.fwd[measure.incount]) + " rev:" + String(measure.rev[measure.incount]));
        //DebugServer.print(0,"fwd:" + String(measure.fwd[measure.incount]) + " rev:" + String(measure.rev[measure.incount]) + "\n\r");
        vTaskDelay(1);
    }
}

void pswr_sync_from_interrupt(void)
{
  uint8_t in;

  xSemaphoreTake( swrBinarySemaphore, portMAX_DELAY );
  in = measure.incount;

  while (measure.outcount != in)              // Read from circular buffer, while new input available
  {
    fwd = measure.fwd[measure.outcount];      // Transfer data from circular buffers
    rev = measure.rev[measure.outcount];
    measure.outcount++;                       // 8 bit value, rolls over at 256
    
    determine_power_pep_pk();                 // Determine Instantaneous power, pep, pk and avg

    in = measure.incount;
  }
 /* char str[82];
  sprintf(str,"ad8307_FdBm %f  ad8307_RdBm %f\r\n", ad8307_FdBm,ad8307_RdBm );
  DebugServer.writeAll((const uint8_t*)str, strlen(str));
  sprintf(str,"fwd_power_mw %f ref_power_mw %f power_mw %f\r\n", fwd_power_mw, ref_power_mw, power_mw );
  DebugServer.writeAll((const uint8_t*)str, strlen(str));
  sprintf(str,"rev_power_db %f power_db %f power_db_pk %f\r\n", rev_power_db, power_db , power_db_pk);
  DebugServer.writeAll((const uint8_t*)str, strlen(str));
  */
  xSemaphoreGive( swrBinarySemaphore );
}


//
//-----------------------------------------------------------------------------------------
//                Convert Voltage Reading into Power when using AD8307
//-----------------------------------------------------------------------------------------
//
void pswr_determine_dBm(void)
{
  double  delta_db;
  double  delta_F, delta_R;
  double  delta_Fdb, delta_Rdb;
  double  temp;

  // Calculate the slope gradient between the two calibration points:
  //
  // (dB_Cal1 - dB_Cal2)/(V_Cal1 - V_Cal2) = slope_gradient
  //
  delta_db = (double)((R.cal_AD[1].db10m - R.cal_AD[0].db10m)/10.0);
  delta_F = R.cal_AD[1].Fwd - R.cal_AD[0].Fwd;
  delta_Fdb = delta_db/delta_F;
  delta_R = R.cal_AD[1].Rev - R.cal_AD[0].Rev;
  delta_Rdb = delta_db/delta_R;
  //
  // measured dB values are: (V - V_Cal1) * slope_gradient + dB_Cal1
  //char str[82];
  //sprintf(str, "R.cal_AD[0].Fwd %f  R.cal_AD[0].Rev %f R.cal_AD[0].db10m %d\r\n", R.cal_AD[0].Fwd, R.cal_AD[0].Rev, R.cal_AD[0].db10m);
 // DebugServer.writeAll((const uint8_t*)str, strlen(str));
 // sprintf(str, "R.cal_AD[1].Fwd %f  R.cal_AD[1].Rev R.cal_AD[1].db10m %d \r\n", R.cal_AD[1].Fwd, R.cal_AD[1].Rev, R.cal_AD[1].db10m);
  //DebugServer.writeAll((const uint8_t*)str, strlen(str));

  //Serial.println (str);
  //sprintf(str, "delta_db %f  delta_F %f delta_R %f\r\n", delta_db, delta_F, delta_R);
  //DebugServer.writeAll((const uint8_t*)str, strlen(str));


  //sprintf(str,"fwd %d  rev %d\r\n", fwd,rev );
  //DebugServer.writeAll((const uint8_t*)str, strlen(str));
  //Serial.println (str);
  
  ad8307_FdBm = (adc_ref * (fwd / 4096.0) - R.cal_AD[0].Fwd) * delta_Fdb + R.cal_AD[0].db10m/10.0;
  ad8307_RdBm = (adc_ref * (rev / 4096.0) - R.cal_AD[0].Rev) * delta_Rdb + R.cal_AD[0].db10m/10.0;

  //sprintf(str,"ad8307_FdBm %f  ad8307_RdBm %f\r\n", ad8307_FdBm,ad8307_RdBm );
  //Serial.println (str);
  //DebugServer.writeAll((const uint8_t*)str, strlen(str));

  // Test for direction of power - Always designate the higher power as "forward"
  // while setting the "Reverse" flag on reverse condition.
  if (ad8307_FdBm > ad8307_RdBm)        // Forward direction
  {
    Reverse = false;
  }
  else                                  // Reverse direction
  {
    temp = ad8307_RdBm;
    ad8307_RdBm = ad8307_FdBm;
    ad8307_FdBm = temp;
    Reverse = true;
  }
}


//
//-----------------------------------------------------------------------------------------
//                Determine Forward and Reflected power
//                from 2x AD8307 or from Diode Detectors.
//                Also determine PEP, 100ms peak and Average, 100ms and 1s.
//-----------------------------------------------------------------------------------------
//
void determine_power_pep_pk(void)
{
  // For measurement of peak and average power
  static int32_t  buff_short[BUF_SHORT];        // voltage db information in a 100 ms window
  static int32_t  pepbuff[PEP_BUFFER];          // voltage db information in a 1, 2.5 or 5 second window
  static int32_t  longbuff[LONG_BUFFER];        // voltage db information in a 30 second window
  int32_t pk;                                   // Keep track of Max (1s) and Peak (100ms) dB voltage
  static double   p_avg_buf[AVG_BUFSHORT];      // a short buffer of all instantaneous power measurements, for short average
  static double   p_avg_buf1s[AVG_BUF1S];       // a one second buffer of instantanous power measurements, for 1s average
  static double   p_1splus;                     // averaging: all power measurements within a 1s window added together
  static double   p_plus;                       // averaging: all power measurements within a shorter window added together
  static uint16_t a;                            // Pk:  100ms ring buffer counter
  static uint16_t b;                            // PEP: ring buffer counter
  static uint16_t c;                            // Long: ring buffer counter
  static uint16_t d;                            // avg: short ring buffer counter
  static uint16_t e;                            // avg: 1s ring buffer counter
   
  //---------------------------------------------------------------------------------
  // Process Forward and Reflected power measurements from 2x AD8307
  //---------------------------------------------------------------------------------

  pswr_determine_dBm();                         // Determine dBm from AD8307 voltages
  
  // Instantaneous forward voltage and power, milliwatts and dBm
  f_inst = pow(10,ad8307_FdBm/20.0);		        // (We use voltage later on, for SWR calc)
  fwd_power_mw = SQR(f_inst);			              // P_mw = (V*V) (current and resistance have already been factored in)
  
  // Instantaneous reverse voltage and power
  r_inst = pow(10,(ad8307_RdBm)/20.0);
  ref_power_mw = SQR(r_inst);
  
  //char str[180];

  //sprintf(str, "ad8307_FdBm %f fwd_power_mw %f  ad8307_RdBm %f ref_power_mw %f\r\n", ad8307_FdBm, fwd_power_mw, ad8307_RdBm, ref_power_mw);
  //Serial.println (str);
  //DebugServer.writeAll((const uint8_t*)str, strlen(str));

  // We need some sane boundaries (4kW) to determine reasonable variable defs for further calculations
  if (fwd_power_mw > 4000000) fwd_power_mw = 4000000;
  if (ref_power_mw > 4000000) ref_power_mw = 4000000;
  
  // Instantaneous Real Power Output
  power_mw = fwd_power_mw - ref_power_mw;
  power_db = 10 * log10(power_mw);
  rev_power_db = 10 * log10(ref_power_mw);

  //sprintf(str, "power_mw %f  ref_power_mw %f\r\n", power_mw, ref_power_mw);
  //Serial.println (str);
 // DebugServer.writeAll((const uint8_t*)str, strlen(str));

  //------------------------------------------
  // Find peaks and averages

  // Multiply by 100 to make suitable for integer value
  // Store dB value in a ring buffer
  buff_short[a++] = 100 * power_db;             // Get precision of two subdecimals
  if (a == BUF_SHORT)    a = 0;

  //------------------------------------------
  // Find Peak value within a 100ms sliding window
  pk =-0x7fffffff;
  for (uint8_t x = 0; x < BUF_SHORT; x++)
  {
    if (pk < buff_short[x]) pk = buff_short[x];
  }
  power_db_pk = pk / 100.0;

  //------------------------------------------
  // Feed and Retrieve Max Value within a 1 to 5 second sliding window, 
  // while using a resolution granularity of BUF_SHORT (100ms)
  if (a == 0)                                   // Once every BUF_SHORT*SAMPLE_TIMER time
  {
    pepbuff[b++] = 100 * power_db_pk;           // Feed the PEP buffer with Pk values, precision of two subdecimals
    if (b >= R.PEP_period) b = 0;
   
    pk =-0x7fffffff;
    for (uint16_t x = 0; x < R.PEP_period; x++) // Retrieve a new PEP value
    {
      if (pk < pepbuff[x]) pk = pepbuff[x];
    }
    power_db_pep = pk / 100.0;
  }
  if (power_db_pep < power_db_pk)               // Correct for slow reaction of the above
    power_db_pep = power_db_pk;
  
  //------------------------------------------
  // Feed and Retrieve Max Value within a 30 second (or longer) sliding window
  if (b == 0)                                   // Once every PEP (1s, 2.5s or 5s) time
  {
    longbuff[c++] = 100 *power_db_pep;          // Feed the long buffer with PEP values, precision of two subdecimals
    if (c >= (LONG_BUFFER/(R.PEP_period/10))) c = 0;

    pk =-0x7fffffff;
    for (uint16_t x = 0; x < (LONG_BUFFER/(R.PEP_period/10)); x++)  // Retrieve a new LONG value
    {
      if (pk < longbuff[x]) pk = longbuff[x];
    }
    power_db_long = pk / 100.0;
  }
  if (power_db_long < power_db_pep)             // Correct for slow reaction of the above
    power_db_long = power_db_pep;
  
  //------------------------------------------
  // Determine averages, using a FIFO + add and subtract buffer methology

  //------------------------------------------
  // Average power buffers, short period (100 ms)
  p_avg_buf[d++] = power_mw;                    // Add the newest value onto ring buffer
  p_plus = p_plus + power_mw;                   // Add latest value to the total sum of all measurements in [100ms]
  if (d == AVG_BUFSHORT) d = 0;                 // wrap around
  p_plus = p_plus - p_avg_buf[d];               // and subtract the oldest value in the ring buffer from the total sum
  power_mw_avg = p_plus / (AVG_BUFSHORT-1);     // And finally, find the short period average

  //------------------------------------------
  // Average power buffers, 1 second
  p_avg_buf1s[e++] = power_mw;                  // Add the newest value onto ring buffer
  p_1splus = p_1splus + power_mw;               // Add latest value to the total sum of all measurements in 1s
  if (e == AVG_BUF1S) e = 0;                    // wrap around
  p_1splus = p_1splus - p_avg_buf1s[e];         // and subtract the oldest value in the ring buffer from the total sum
  power_mw_1savg = p_1splus / (AVG_BUF1S-1);    // And finally, find the one second period average
}

//
//-----------------------------------------------------------------------------
//      Print Power, input value is in milliWatts
//-----------------------------------------------------------------------------
//
uint16_t print_p_mw_value(double pwr, std::string &value)
{
    int8_t r = 3;                   // Start in the mW range
    uint8_t offs = 0;
    const char* indicator[] = { "pW", "nW", "uW", "mW", "W", "kW", "MW", "GW" /* hah! */ };
    double p;
    char buffer[82];

    if (pwr < 0) pwr *= -1;         // ABS() for float
    p = pwr;

#if AD8307_INSTALLED            // Only relevant when not diode detectors
    // If lowpower print threshold has NOT been set at 10 uW and power is below 1mW
    if (R.low_power_floor != FLOOR_TEN_uW)
    {
        while ((p < 1.0) && (r > 0))  // Power levels below one milliwatt, down to pW
        {
            p *= 1000.0;
            r -= 1;
        }
    }
#endif

    while ((p >= 1000) && (r < 7))  // Power levels above one watt, up to GW
    {
        p /= 1000.0;
        r += 1;
    }

    if (r == 4)                     // Different format if "W"
    {
        lcd_buf[0] = ' ';
        offs = 1;
    }

#if AD8307_INSTALLED            // These are only relevant when not diode detectors
    // If lowpower print threshold has been set at 1 uW and power is below that threshold
    if ((R.low_power_floor == FLOOR_ONE_uW) && (pwr < 0.001))
    {
        value = "0uW";
    }
    // If lowpower print threshold has been set at 10 uW and power is below that threshold
    else if ((R.low_power_floor == FLOOR_TEN_uW) && (pwr < 0.01))
    {
        value = "0uW";
    }
    // If lowpower print threshold has been set at 100 uW and power is below that threshold
    else if ((R.low_power_floor == FLOOR_100_uW) && (pwr < 0.1))
    {
        value = "0uW";
    }
    // If lowpower print threshold has been set at 1 mW and power is below that threshold
    else if ((R.low_power_floor == FLOOR_ONE_mW) && (pwr < 1.0))
    {
        value = "0mW";
    }
    // If lowpower print threshold has been set at 1 mW and power is below that threshold
    else if ((R.low_power_floor == FLOOR_TEN_mW) && (pwr < 10.0))
    {
        value = "0mW";
    }
    else
#endif
        // 9.995 rather than 10.00 for correct round-up when two subdecimal formatting
    {
        if (p >= 99.95) {
            sprintf(buffer, "%3u", (uint16_t)p);
            value = std::string(buffer) + std::string(indicator[r]);
        }
        else if (p >= 9.995)
        {
            if (r > 3)
                sprintf(buffer, "%2.01f", p);
            else
                sprintf(buffer, "%2.0f", p);
            value = std::string(buffer) + std::string(indicator[r]);
        }
        else if (p < 9.995) 
        {
            if (r > 3)
                sprintf(buffer, "%1.02f", p);
            else
                sprintf(buffer, "%1.0f", p);
            value = std::string(buffer) + std::string(indicator[r]);
        }
    }
    //char str[180];

   // sprintf(str,"value %s p %f \n\r", value.c_str(), p);
    //Serial.println (str);
    //DebugServer.writeAll((const uint8_t*)str, strlen(str));

    return floor(p);
}


//
//-----------------------------------------------------------------------------------------
//                Prepare various types of power and calculate SWR
//                (these are things that can be run outside of the
//                 interrupt function, to optimize time management)
//-----------------------------------------------------------------------------------------
//
void calc_SWR_and_power(void)
{
  static double  swr_avg_buf[AVG_BUFSWR];        // a buffer of instantaneous swr measurements, for swr smoothing
  static double  swr_plus;                       // averaging: all swr measurements within a short window added together
  static uint16_t a;                             // swr avg: short ring buffer counter
  
  power_mw_pk   = pow(10,power_db_pk/10.0);
  power_mw_pep  = pow(10,power_db_pep/10.0);   
  power_mw_long = pow(10,power_db_long/10.0);   
  
 // char str[82];
//  sprintf(str, "power_mw_pep %f power_db_pep %f \r\n", power_mw_pep, power_db_pep);
  //Serial.println (str);
//  DebugServer.writeAll((const uint8_t*)str, strlen(str));


  // Only calculate SWR if meaningful power

  if ((power_mw > MIN_PWR_FOR_SWR_CALC) || (power_mw < -MIN_PWR_FOR_SWR_CALC))
  {
    // Calculate SWR
    swr = (1+(r_inst/f_inst))/(1-(r_inst/f_inst));

    // Check for high SWR and set alarm flag if trigger value is exceeded
    // If trigger is 40 (4:1), then Alarm function is Off
/*    if ((R.SWR_alarm_trig != 40) && ((swr*10) >= R.SWR_alarm_trig) && (power_mw > R.SWR_alarm_pwr_thresh))
    {
      if (flag.swr_alarm)                        // This is the second time in a row, raise the SWR ALARM
      {
        digitalWrite(R_Led,flag.swr_alarm);      // Turn on the Alarm LED
      }
      flag.swr_alarm = true;                     // Set SWR flag - may be a one shot fluke
    }
    //else if (flag.swr_alarm)                   // Clear SWR Flag if High SWR condition does not exist
    //{
    //  flag.swr_alarm = false;                  // Alarm is cleared with a Touch Screen Push Action
    //}
*/  
  }
  // If some power present, but not enough for an accurate SWR reading, then use
  // recent measured value
  else if ((power_mw > MIN_PWR_FOR_SWR_SHOW) || (power_mw < -MIN_PWR_FOR_SWR_SHOW))
  {
    // Do nothing, in other words, swr remains the same
  }
  else
  {
    // No power present, set SWR to 1.0
    swr = 1.0;
  }

  //------------------------------------------
  // Average swr reading over short period (100 ms)
  swr_avg_buf[a++] = swr;                       // Add the newest value onto ring buffer
  swr_plus = swr_plus + swr;                    // Add latest value to the total sum of all measurements in [100ms]
  if (a == AVG_BUFSWR) a = 0;                   // wrap around
  swr_plus = swr_plus - swr_avg_buf[a];         // and subtract the oldest value in the ring buffer from the total sum
  swr_avg = swr_plus / (AVG_BUFSWR-1);          // And finally, find the average
}

//
//-----------------------------------------------------------------------------
//      Print SWR, returns string in lcd_buf
//-----------------------------------------------------------------------------
//
uint16_t  print_swr(void)
{
  uint16_t swr_sub;
  uint16_t swr_sup;
   
  // Format for 2 sub-decimals
  swr_sub = swr_avg * 100;
  swr_sup = swr_sub / 100;
  swr_sub = swr_sub % 100;

  if (swr_avg < 2.0)
  {
    sprintf(lcd_buf,"%u.%02u",swr_sup,swr_sub);  
  }
  else if (swr_avg <= 10.0)
  {
    swr_sub = swr_sub / 10;
    sprintf(lcd_buf," %u.%01u",swr_sup,swr_sub);  
  }
  else if (swr_avg <= 1000.0)
  {
    sprintf(lcd_buf,"%4u",(uint16_t) swr_avg);
  }
  else
  {
    //sprintf(lcd_buf," N/A");
    sprintf(lcd_buf,"9999");
  }
return swr_avg * 100;
}

uint8_t check_input_cal()
{
    double fwd, rev;
    uint8_t   cal_sig_direction_quality = 0, cal_sig_direction_quality_old = 3;

    fwd = (adc_ref * ((double)analogRead(FWD_METER) / 4096.0)) ;
    rev = (adc_ref * ((double)analogRead(REV_METER) / 4096.0)) ;
    
    DebugServer.print(0,"Fwd: " + String(fwd) + " Rev: " + String(rev) + "slope: " + String(((fwd - rev) * 1000.0 / LOGAMP2_SLOPE)) + "\r\n");
    
    if ((((fwd - rev) * 1000.0 / LOGAMP2_SLOPE) > CAL_INP_QUALITY) && (cal_sig_direction_quality != CAL_FWD))
    {
        cal_sig_direction_quality = CAL_FWD;
    }
    // Check reverse direction and sufficient level
    else if ((((rev - fwd) * 1000.0 / LOGAMP1_SLOPE) > CAL_INP_QUALITY)
        && (cal_sig_direction_quality != CAL_REV))
    {
        cal_sig_direction_quality = CAL_REV;
    }
    // Check insufficient level
    else if (((ABS((fwd - rev)) * 1000.0 / LOGAMP2_SLOPE) <= CAL_INP_QUALITY)
        && (cal_sig_direction_quality != CAL_BAD))
    {
        cal_sig_direction_quality = CAL_BAD;
    }
    return cal_sig_direction_quality;
}

void init_measurement(bool upgrade)
{
    long  current_frq1[] = { 1800000,3500000,7000000,14000000,21000000,28000000 };
    long  current_frq2[] = { 1800000,3500000,7000000,14000000,21000000,28000000 };

    // default settings
    memset(&R, 0, sizeof(var_t));
    memcpy(R.current_frq1, current_frq1, sizeof(current_frq1));
    memcpy(R.current_frq2, current_frq2, sizeof(current_frq2));
    R.band[0] = 1;
    R.band[1] = 1;
    R.active_vfo = 0;
    R.cal_AD[0].db10m = CAL1_NOR_VALUE;
    R.cal_AD[0].Fwd = CALFWD1_DEFAULT;
    R.cal_AD[0].Rev = CALREV1_DEFAULT;
    R.cal_AD[1].db10m = CAL2_NOR_VALUE;
    R.cal_AD[1].Fwd = CALFWD2_DEFAULT;
    R.cal_AD[1].Rev = CALREV2_DEFAULT;
    R.PEP_period = PEP_PERIOD;
    R.AVG_period = 0;
    R.low_power_floor = FLOOR_TEN_mW;
    R.correction_si5351_no1 = 0;
    R.correction_si5351_no2 = 0;
    R.xtal_si5351_no1 = SI5351_XTAL_FREQ;
    R.xtal_si5351_no2 = SI5351_XTAL_FREQ;
    R.wifi_onoff = 1;
    R.scale_watt = CAL_SCALE_WATT;
}