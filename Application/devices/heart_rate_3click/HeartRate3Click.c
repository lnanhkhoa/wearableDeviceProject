#include "i2ctransfer/i2ctransfer.h"
#include "HeartRate3Click.h"
#include <xdc/runtime/System.h>

void heartrate_3_init(){
  dynamic_modes_t dynamic_modes;
  dynamic_modes.transmit = trans_dis;                      //Transmitter disabled
  dynamic_modes.curr_range = led_double;                   //LED range 0 - 100
  dynamic_modes.adc_power = adc_on;                        //ADC on
  dynamic_modes.clk_mode = osc_mode;                       //Use internal Oscillator
  dynamic_modes.tia_power = tia_off;                       //TIA off
  dynamic_modes.rest_of_adc = rest_of_adc_off;             //Rest of ADC off
  dynamic_modes.afe_rx_mode = afe_rx_normal;               //Normal Receiving on AFE
  dynamic_modes.afe_mode = afe_normal;                     //Normal AFE functionality


  hr3_set_settings( sw_reset_en,diag_mode_dis,susp_count_dis,reg_read_dis );
  //led 2 start - end
  hr3_set_led2_start_end( 0, 399 );
  // led 2 sample start - end
  hr3_set_led2_sample_start_end( 80, 399 );
  // adc reset 0  start - end
  hr3_set_adc_reset0_start_end( 401, 407 );
  // led 2 convert start - end
  hr3_set_led2_convert_start_end( 408, 1467 );

  // led 3 convert start - stop
  hr3_set_led3_start_stop( 400, 799 );
  // led 3 sample start end
  hr3_set_led3_sample_start_end( 480, 799 );
  // adc reset 1 start end
   hr3_set_adc_reset1_start_end( 1469, 1475 );
  // // led 3 convert start end
   hr3_set_led3_convert_start_end( 1476, 2535 );

  // led 1 start end
  hr3_set_led1_start_end( 800, 1199 );
  // led 1 sample start end
  hr3_set_led1_sample_start_end( 880, 1199 );
  // adc reset 2 start end
  hr3_set_adc_reset2_start_end( 2537, 2543 );
  // led 1 convert start end
  hr3_set_led1_convert_start_end( 2544, 3603 );
  // amb1 sample start - end
  hr3_set_amb1_sample_start_end( 1279, 1598 );
  // adc reset 3 start - end
  hr3_set_adc_reset3_start_end( 3605, 3611 );
  // amb1 convert start - end
  hr3_set_amb1_convert_start_end( 3612, 4671 );
  // pdn cycle start - end
  hr3_set_pdn_cycle_start_end( 5471, 39199  );
  // prpct count
  hr3_set_prpct_count( 39999  );                              //Clock timing for CLKDIV_PRF  = 1
  // // timer and average num
   hr3_set_timer_and_average_num( true, 3 );
  // // seperate tia gain
   hr3_set_seperate_tia_gain( true, 0, 4 );
  // // tia gain
   hr3_set_tia_gain( false, 0, 3 );
  // // led currents
   hr3_set_led_currents( 20,0,0 );
  // // dynamic settings
   hr3_set_dynamic_settings( &dynamic_modes );
  // // clkout div
   hr3_set_clkout_div( false, 2 );
  // // int clk div
  hr3_set_int_clk_div( 1 );
  AFE_Reg_Write(0x39, 0x010000);
}

/////////////////////////////////////////////////Get Data From Sensor///////////////////////////////////////////////////////
/////////////////////////////////////////////////Modified - RED DATA////////////////////////////////////////////////////////
/////////////////////////////////////////////////Modified - IR DATA/////////////////////////////////////////////////////////
long get_GR_data(){
  uint8_t dataLedGreenArray[3] = {0};
  I2C_read(AFE_ADDR,0x2C, dataLedGreenArray,3);
  long dataLongLed1 = 0;
  long old_data;
  //show_bit_data(0);
  return ((dataLongLed1 | dataLedGreenArray[0]) << 16) | ((dataLongLed1 | dataLedGreenArray[1]) << 8) | dataLedGreenArray[2];

}

long get_RED_data(){
  uint8_t dataLedRedArray[3] = {0};
  I2C_read(AFE_ADDR,0x2C, dataLedRedArray,3);
  long dataLongLed1 = 0;
  long old_data;
  //show_bit_data(0);
  return ((dataLongLed1 | dataLedRedArray[0]) << 16) | ((dataLongLed1 | dataLedRedArray[1]) << 8) | dataLedRedArray[2];

}

long get_IR_data(){
  uint8_t dataIRRedArray[3] = {0};
  I2C_read(AFE_ADDR,0x2C, dataIRRedArray,3);
  long dataLongLed1 = 0;
  long old_data;
  //show_bit_data(0);
  return ((dataLongLed1 | dataIRRedArray[0]) << 16) | ((dataLongLed1 | dataIRRedArray[1]) << 8) | dataIRRedArray[2];

}
/////////////////////////////////////////////////----------End---------///////////////////////////////////////////////////////

int8_t hr3_set_settings( sw_reset_t sw_reset, diag_mode_t diag_mode, susp_count_t susp_counter, reg_read_t reg_read )
{
  uint8_t reg_address = DIAGNOSIS;
  uint8_t temp[3] = { 0 };

  temp[2] |= ( sw_reset << DIAG_SW_RST );
  temp[2] |= ( diag_mode << DIAG_EN );
  temp[2] |= ( susp_counter << DIAG_TM_CNT_RST );
  temp[2] |= ( reg_read << DIAG_REG_READ );

  I2C_write(AFE_ADDR, reg_address, temp, 3);

  return 0;
}


int8_t hr3_set_led2_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = LED2_ST;
  uint8_t reg_end = LED2_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_led2_sample_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = SMPL_LED2_ST;
  uint8_t reg_end = SMPL_LED2_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_adc_reset0_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = ADC_RST_P0_ST;
  uint8_t reg_end = ADC_RST_P0_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_led2_convert_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = LED2_CONV_ST;
  uint8_t reg_end = LED2_CONV_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_led3_start_stop( uint16_t start, uint16_t end )
{
  uint8_t reg_st = LED3LEDSTC;
  uint8_t reg_end = LED3LEDENDC;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;


}

int8_t hr3_set_led3_sample_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = SMPL_LED3_ST;
  uint8_t reg_end = SMPL_LED3_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_adc_reset1_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = ADC_RST_P1_ST;
  uint8_t reg_end = ADC_RST_P1_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_led3_convert_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = LED3_CONV_ST;
  uint8_t reg_end = LED3_CONV_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_led1_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = LED1_ST;
  uint8_t reg_end = LED1_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_led1_sample_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = SMPL_LED1_ST;
  uint8_t reg_end = SMPL_LED1_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_adc_reset2_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = ADC_RST_P2_ST;
  uint8_t reg_end = ADC_RST_P2_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535  )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_led1_convert_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = LED1_CONV_ST;
  uint8_t reg_end = LED1_CONV_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);
  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_amb1_sample_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = SMPL_AMB1_ST;
  uint8_t reg_end = SMPL_AMB1_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_adc_reset3_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = ADC_RST_P3_ST;
  uint8_t reg_end = ADC_RST_P3_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_amb1_convert_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = AMB1_CONV_ST;
  uint8_t reg_end = AMB1_CONV_END;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_pdn_cycle_start_end( uint16_t start, uint16_t end )
{
  uint8_t reg_st = PDNCYCLESTC;
  uint8_t reg_end = PDNCYCLEENDC;
  uint8_t temp_st[3] = { 0 };
  uint8_t temp_end[3] = { 0 };

  if( start > 65535 || end > 65535 )
    return -1;

  temp_st[1] = start >> 8;
  temp_st[2] = (uint8_t)start;

  I2C_write(AFE_ADDR, reg_st, temp_st, 3);

  temp_end[1] = end >> 8;
  temp_end[2] = (uint8_t)end;

  I2C_write(AFE_ADDR, reg_end, temp_end, 3);

  return 0;

}

int8_t hr3_set_prpct_count( uint16_t count )
{
  uint8_t reg = PRPCT;
  uint8_t temp[3] = { 0 };

  if( count > 65535 )
    return -1;

  temp[1] = count >> 8;
  temp[2] = (uint8_t)count;

  I2C_write(AFE_ADDR, reg, temp, 3);

  return 0;

}

int8_t hr3_set_timer_and_average_num( bool enable, uint8_t av_num )
{
  uint8_t reg = TIM_NUMAV;
  uint8_t temp[3] = { 0 };

  if( av_num > 15 || av_num < 0 )
    return -1;

  if( enable )
  {
    temp[1] |= ( 1 << TIMEREN );
    temp[2] |= ( av_num << NUMAV );
    I2C_write(AFE_ADDR, reg, temp, 3);
  }
  else
  {
    temp[2] |= ( av_num << NUMAV );
    I2C_write(AFE_ADDR, reg, temp, 3);
  }

  return 0;

}

int8_t hr3_set_seperate_tia_gain( bool seperate, uint8_t cf_setting,
                   uint8_t gain_setting )
{
  uint8_t reg = TIA_GAINS2;
  uint8_t temp[3] = { 0 };

  if( cf_setting > 7 || gain_setting > 7 )
    return -1;

  if( seperate )
  {
    temp[1] = TIA_ENSEPGAIN;
    temp[2] |= ( cf_setting << TIA_CF_SEP );
    temp[2] |= ( gain_setting << TIA_GAIN_SEP );
    I2C_write(AFE_ADDR, reg, temp, 3);
  }
  else
  {
    temp[2] |= ( cf_setting << TIA_CF_SEP );
    temp[2] |= ( gain_setting << TIA_GAIN_SEP );
    I2C_write(AFE_ADDR, reg, temp, 3);
  }

  return 0;

}

int8_t hr3_set_tia_gain( bool replace, uint8_t cf_setting,
               uint8_t gain_setting )
{
  uint8_t reg = TIA_GAINS1;
  uint8_t temp[3] = { 0 };

  if( cf_setting > 7 || gain_setting > 7 )
    return -1;

  if( replace )
  {
    temp[1] = TIA_PROG_TG_EN;
    temp[2] |= ( cf_setting << TIA_CF );
    temp[2] |= ( gain_setting << TIA_GAIN );
    I2C_write(AFE_ADDR, reg, temp, 3);
  }
  else
  {
    temp[1] = 0;
    temp[2] |= ( cf_setting << TIA_CF_SEP );
    temp[2] |= ( gain_setting << TIA_GAIN_SEP );
    I2C_write(AFE_ADDR, reg, temp, 3);
  }

  return 0;

}

int8_t hr3_set_led_currents( uint8_t led1_current, uint8_t led2_current,
                uint8_t led3_current )
{
  uint8_t reg = LED_CONFIG;
  uint8_t temp[3] = { 0 };
  unsigned long currents = 0;

  if( led1_current > 63 ||
    led2_current > 63 ||
    led3_current > 63 )
    return -1;

  currents |= ( led3_current << 12 );
  currents |= ( led2_current << 6 );
  currents |= led1_current;

  temp[2] |= currents;
  temp[1] |= currents >> 8;
  temp[0] |= currents >> 16;


  I2C_write(AFE_ADDR, reg, temp, 3);

  return 0;

}

int8_t hr3_set_dynamic_settings( dynamic_modes_t* modes )
{
  uint8_t reg = SETTINGS;
  uint8_t temp[3] = { 0 };
  unsigned long buffer = 0;

  buffer |= ( modes->transmit << STT_DYNMC1 );
  buffer |= ( modes->curr_range << STT_ILED_2X );
  buffer |= ( modes->adc_power << STT_DYNMC2 );
  buffer |= ( modes->clk_mode << STT_OSC_EN );
  buffer |= ( modes->tia_power << STT_DYNMC3 );
  buffer |= ( modes->rest_of_adc << STT_DYNMC4 );
  buffer |= ( modes->afe_rx_mode << STT_PDNRX );
  buffer |= ( modes->afe_mode << STT_PDNAFE );

  temp[2] |= buffer;
  temp[1] |= buffer >> 8;
  temp[0] |= buffer >> 16;


  I2C_write(AFE_ADDR, reg, temp, 3);

  return 0;
}

int8_t hr3_set_clkout_div( bool enable, uint8_t div )
{
  uint8_t reg = CLKOUT;
  uint8_t temp[3] = { 0 };

  if( div > 15 )
    return -1;

  if( enable )
  {
    temp[1] = ( 1 << CLKOUT_EN );
    temp[2] = ( div << CLKOUT_DIV );
    I2C_write(AFE_ADDR, reg, temp, 3);
  }
  else
  {
    temp[2] = ( div << CLKOUT_DIV );
    I2C_write(AFE_ADDR, reg, temp, 3);
  }

  return 0;

}

int8_t hr3_set_int_clk_div( uint8_t div )
{
  uint8_t reg = CLKDIV_PRF;
  uint8_t temp[3] = { 0 };

  if( div > 7 )
    return -1;

  temp[2] = div;
  I2C_write(AFE_ADDR, reg, temp, 3);

  return 0;

}
