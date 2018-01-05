#include <stdbool.h>

#define AFE_CONTROL0                0x00
#define AFE_LED2STC                 0x01
#define AFE_LED2ENDC                0x02
#define AFE_LED1LEDSTC              0x03
#define AFE_LED1LEDENDC             0x04
#define AFE_ALED2STC                0x05
#define AFE_ALED2ENDC               0x06
#define AFE_LED1STC                 0x07
#define AFE_LED1ENDC                0x08
#define AFE_LED2LEDSTC              0x09
#define AFE_LED2LEDENDC             0x0a
#define AFE_ALED1STC                0x0b
#define AFE_ALED1ENDC               0x0c
#define AFE_LED2CONVST              0x0d
#define AFE_LED2CONVEND             0x0e
#define AFE_ALED2CONVST             0x0f
#define AFE_ALED2CONVEND            0x10
#define AFE_LED1CONVST              0x11
#define AFE_LED1CONVEND             0x12
#define AFE_ALED1CONVST             0x13
#define AFE_ALED1CONVEND            0x14
#define AFE_ADCRSTSTCT0             0x15
#define AFE_ADCRSTENDCT0            0x16
#define AFE_ADCRSTSTCT1             0x17
#define AFE_ADCRSTENDCT1            0x18
#define AFE_ADCRSTSTCT2             0x19
#define AFE_ADCRSTENDCT2            0x1a
#define AFE_ADCRSTSTCT3             0x1b
#define AFE_ADCRSTENDCT3            0x1c
#define AFE_PRPCOUNT                0x1d
#define AFE_CONTROL1                0x1e
#define AFE_TIA_GAIN_SEP            0x20
#define AFE_TIA_GAIN                0x21
#define AFE_LEDCNTRL                0x22
#define AFE_CONTROL2                0x23
#define AFE_ALARM                   0x29
#define AFE_LED2VAL                 0x2a
#define AFE_ALED2VAL                0x2b
#define AFE_LED1VAL                 0x2c
#define AFE_ALED1VAL                0x2d
#define AFE_LED2_ALED2VAL           0x2e
#define AFE_LED1_ALED1VAL           0x2f
#define AFE_CONTROL3                0x31
#define AFE_PDNCYCLESTC             0x32
#define AFE_PDNCYCLEENDC            0x33
#define AFE_PROG_TG_STC             0x34
#define AFE_PROG_TG_ENDC            0x35
#define AFE_LED3LEDSTC              0x36
#define AFE_LED3LEDENDC             0x37
#define AFE_CLKDIV_PRF              0x39
#define AFE_OFFDAC                  0x3a
#define AFE_DEC                     0x3d
#define AFE_AVG_LED2_ALED2VAL       0x3f
#define AFE_AVG_LED1_ALED1VAL       0x40
#define AFE_ADDR 0x58

//Diagnosis
#define DIAGNOSIS       0x00
#define DIAG_SW_RST        3    /**< For software reset, write 1              */
#define DIAG_EN            2    /**< 0: No action 1: Diagnostic mode enabled  */
#define DIAG_TM_CNT_RST    1    /**< Used to suspend count/keep counter reset */
#define DIAG_REG_READ      0    /**< 0: Register Write Mode 1: Enable readout */

//PRPCT ( timer counter )
#define PRPCT            0x1D   /**< Bits 0-15 for writing counter value      */

//Timer Module enable / NUMAV ( # of times to sample and average )
#define TIM_NUMAV        0x1E
#define NUMAV               0   /**< Starts at bit 0, no bit shifting needed  */
#define TIMEREN             0   /**< 0: Module Disabled 1: Enables Module     */
                                // TIMEREN is on byte 2 and NUMAV is on byte 1

//TIA Gains 1
#define TIA_GAINS1       0x21
#define TIA_PROG_TG_EN      1   /**< Replaces ADC_RDY with programmable signal*/
#define TIA_CF              3   /**< Control of C(1) Setting                  */
#define TIA_GAIN            0   /**< Control of R(1) Setting                  */
                                //If ENSEPGAIN = 0, TIA_CF and TIA_GAIN control
                                //TIA_CF controls C1 and C2 Settings and
                                //TIA_GAIN controls R1 and R2 Settings

//TIA Gains 2
#define TIA_GAINS2       0x20
#define TIA_ENSEPGAIN       8   /**< 0: Single TIA Gain 1: Two sets TIA gains */
#define TIA_CF_SEP          3   /**< Control of C(2) Setting                  */
#define TIA_GAIN_SEP        0   /**< Control of R(2) Setting                  */

//LED1 Start / End
#define LED1_ST          0x03
#define LED1_END         0x04

//Sample LED1 Start / End
#define SMPL_LED1_ST     0x07
#define SMPL_LED1_END    0x08

//LED1 Convert Start / End
#define LED1_CONV_ST     0x11
#define LED1_CONV_END    0x12

//Sample Ambient 1 Start / End
#define SMPL_AMB1_ST     0x0B
#define SMPL_AMB1_END    0x0C

//Ambient 1 Convert Start / End
#define AMB1_CONV_ST     0x13
#define AMB1_CONV_END    0x14

//LED2 Start / End
#define LED2_ST          0x09
#define LED2_END         0x0A

//Sample LED2 Start / End
#define SMPL_LED2_ST     0x01
#define SMPL_LED2_END    0x02

//LED2 Convert Start / End
#define LED2_CONV_ST     0x0D
#define LED2_CONV_END    0x0E

//Sample Ambient 2 ( or LED3 ) Start / End
#define SMPL_LED3_ST     0x05
#define SMPL_LED3_END    0x06

//Ambient 2 ( or LED3 ) Convert Start / End
#define LED3_CONV_ST     0x0F
#define LED3_CONV_END    0x10

//ADC Reset Phase 0 Start / End
#define ADC_RST_P0_ST    0x15
#define ADC_RST_P0_END   0x16

//ADC Reset Phase 1 Start / End
#define ADC_RST_P1_ST    0x17
#define ADC_RST_P1_END   0x18

//ADC Reset Phase 2 Start / End
#define ADC_RST_P2_ST    0x19
#define ADC_RST_P2_END   0x1A

//ADC Reset Phase 3 Start / End
#define ADC_RST_P3_ST    0x1B
#define ADC_RST_P3_END   0x1C

//LED Current Control
#define LED_CONFIG       0x22
/* ********************************************************************
 * LED current control is a 24 bit register where                     *
 * LED1: bits 0-5 LED2: bits 6-11 LED3: bits 12-17 and the rest are 0 *
 * ****************************************************************** *
 * LED1, LED2, LED3 Register Values | LED Current Setting (mA)        *
 *                   0              |              0                  *
 *                   1              |             0.8                 *
 *                   2              |             1.6                 *
 *                   3              |             2.4                 *
 *                  ...             |             ...                 *
 *                   63             |              50                 *
 **********************************************************************/

//Dynamic Settings Address + registers

#define SETTINGS          0x23  /**< Settings Address */
#define STT_DYNMC1          20  /**< 0: Transmitter not pwrd dwn 1: pwrd dwn  */
#define STT_ILED_2X         17  /**< 0: LED crrnt range 0-50 1: range 0-100   */
#define STT_DYNMC2          14  /**< 0: ADC not pwrd dwn 1: ADC pwrd dwn      */
#define STT_OSC_EN           9  /**< 0: External Clock 1: Internal Clock      */
#define STT_DYNMC3           4  /**< 0: TIA not pwrd dwn 1: TIA pwrd dwn      */
#define STT_DYNMC4           3  /**< 0: Rest of ADC ! pwrd dwn 1: Is pwrd dwn */
#define STT_PDNRX            1  /**< 0: Normal Mode 1: RX of AFE pwrd dwn     */
#define STT_PDNAFE           0  /**< 0: Normal Mode 1: Entire AFE pwrd dwn    */

//Clockout Settings
#define CLKOUT            0x29  /**< Clockout Address */
#define CLKOUT_EN            1  /**< 0: Disable clock output 1: Enable output */
#define CLKOUT_DIV           1  /**< Frequency of clock output on CLK pin     */
/* *****************************************************************************
  *  CLKOUT_DIV Register Settings Graph
  * ****************************************************************************
  * CLKOUT_DIV Register Settings | Division Ratio | Output Clock Freq. ( MHz ) *
  *             0                |         1      |             4              *
  *             1                |         2      |             2              *
  *             2                |         4      |             1              *
  *             3                |         8      |            0.5             *
  *             4                |        16      |           0.25             *
  *             5                |        32      |          0.125             *
  *             6                |        64      |         0.0625             *
  *             7                |       128      |        0.03125             *
  *         8..15                |    Do not use  |      Do not use            *
  *****************************************************************************/

//LED1 Output Value
#define LED1VAL           0x2C  /**< LED1 Output code in twos complement      */

//LED2 Output Value
#define LED2VAL           0x2A  /**< LED2 Output code in twos complement      */

//Ambient 1 Value
#define ALED1VAL          0x2D  /**< Ambient 1 value in twos complement       */

//LED1-Ambient 1 Value
#define LED1_ALED1VAL     0x2F  /**< LED1-ambient1 in twos complement         */

//LED2-Ambient 2 Value
#define LED2_ALED2VAL     0x2E  /**< LED2-ambient2 in twos complement         */

//LED3 or Ambient 2 Value
#define LED3VAL           0x2B  /**< LED3 / Ambient 2 value in twos complement*/

//Diagnostics Flag
#define PD_SHORT_FLAG     0x30  /**< 0: No short across PD 1: Short across PD */

//PD disconnect / INP, INN settings / EXT clock Division settings
#define PD_INP_EXT        0x31
#define PD_DISCONNECT        2  /**< Disconnects PD signals (INP, INM)        */
#define ENABLE_INPUT_SHORT   5  /**< INP, INN are shorted to VCM when TIA dwn */
#define CLKDIV_EXTMODE       0  /**< Ext Clock Div Ration bits 0-2            */

/* *****************************************************************************
  *  CLKDIV_EXTMODE Register Settings Graph
  * ****************************************************************************
  * CLKDIV_EXTMODE Settings      | Division Ratio | Allowed Clock Freq ( MHz ) *
  *             0                |         2      |           8-12             *
  *             1                |         8      |          32-48             *
  *             2                |    Do Not Use  |       Do Not Use           *
  *             3                |         8      |          48-60             *
  *             4                |        16      |          16-24             *
  *             5                |        32      |            4-6             *
  *             6                |        64      |          24-36             *
  *             7                |    Do Not Use  |       Do Not Use           *
  *****************************************************************************/

//PDN_CYCLE Start / End
#define PDNCYCLESTC       0x32  /**< Bits 0-15                                */
#define PDNCYCLEENDC      0x33  /**< Bits 0-15                                */

//Programmable Start / End time for ADC_RDY replacement
#define PROG_TG_STC       0x34  /**< Bits 0-15 Define Start Time              */
#define PROG_TG_ENDC      0x35  /**< Bits 0-15 Define End Time                */

//LED3C Start / End
#define LED3LEDSTC        0x36  /**< LED3 Start, if not used set to 0         */
#define LED3LEDENDC       0x37  /**< LED3 End, if not used set to 0           */

//PRF Clock Division settings
#define CLKDIV_PRF        0x39  /**< Clock Division Ratio for timing engine   */
/* ****************************************************************************
  *   CLKDIV_PRF Register Settings Graph
  * ****************************************************************************
  * CLKDIV_PRF Settings|Division Ratio|Clock Freq(MHz)| Lowest PRF Setting (Hz)*
  *          0         |       1      |       4       |           61           *
  *          1         |  Do Not Use  |  Do Not Use   |      Do Not Use        *
  *          2         |  Do Not Use  |  Do Not Use   |      Do Not Use        *
  *          3         |  Do Not Use  |  Do Not Use   |      Do Not Use        *
  *          4         |       2      |       2       |           31           *
  *          5         |       4      |       1       |           15           *
  *          6         |       8      |     0.5       |            8           *
  *          7         |      16      |    0.25       |            4           *
  *****************************************************************************/

//DAC Settings
#define DAC_SETTING       0x3A  /**< DAC Settings Address                     */
#define POL_OFFDAC_LED2     19  /**< Polarity for LED2                        */
#define I_OFFDAC_LED2       15  /**< Setting for LED2                         */
#define POL_OFFDAC_AMB1     14  /**< Polarity for Ambient 1                   */
#define I_OFFDAC_AMB1       10  /**< Setting for Ambient 1                    */
#define POL_OFFDAC_LED1      9  /**< Polarity for LED1                        */
#define I_OFFDAC_LED1        5  /**< Setting for LED1                         */
#define POL_OFFDAC_LED3      4  /**< Polarity for LED3                        */
#define I_OFFDAC_LED3        0  /**< Setting for LED3                         */
/* **************************************************************
 *  I_OFFDAC Register Settings                                  *
 * **************************************************************
 * * Reg. Settings | Offset Cancellation | Offset Cancellation  |
 * *               |   POL_OFFDAC = 0    |   POL_OFFDAC = 1     |
 * **************************************************************
 *        0        |          0          |           0          *
 *        1        |       0.47          |       –0.47          *
 *        2        |       0.93          |       –0.93          *
 *        3        |        1.4          |        –1.4          *
 *        4        |       1.87          |       –1.87          *
 *        5        |       2.33          |       –2.33          *
 *        6        |        2.8          |        –2.8          *
 *        7        |       3.27          |       –3.27          *
 *        8        |       3.73          |       –3.73          *
 *        9        |        4.2          |        –4.2          *
 *       10        |       4.67          |       –4.67          *
 *       11        |       5.13          |       –5.13          *
 *       12        |        5.6          |        –5.6          *
 *       13        |       6.07          |       –6.07          *
 *       14        |       6.53          |       –6.53          *
 *       15        |          7          |          –7          *
 ****************************************************************/

/******************************************************************************
* Macros
*******************************************************************************/



/******************************************************************************
* Typedefs
*******************************************************************************/

typedef enum
{
    sw_reset_dis = 0,
    sw_reset_en
}sw_reset_t;

typedef enum
{
    diag_mode_dis = 0,
    diag_mode_en
}diag_mode_t;

typedef enum
{
    susp_count_dis = 0,
    susp_count_en
}susp_count_t;

typedef enum
{
    reg_read_dis = 0,
    reg_read_en
}reg_read_t;

typedef enum
{
    trans_dis = 0,
    trans_en
}transmitter_t;

typedef enum
{
    led_norm = 0,
    led_double
}led_current_range_t;

typedef enum
{
    adc_off = 0,
    adc_on
}adc_pwer_t;

typedef enum
{
    ext_clk_mode = 0,
    osc_mode
}clk_mode_t;

typedef enum
{
    tia_on = 0,
    tia_off
}tia_pwer_t;

typedef enum
{
    rest_of_adc_off = 0,
    rest_of_adc_on
}rest_of_adc_t;

typedef enum
{
    afe_rx_normal = 0,
    afe_rx_off
}afe_rx_mode_t;

typedef enum
{
    afe_normal = 0,
    afe_off
}afe_mode_t;

typedef enum
{
    pd_connect = 0,
    pd_disconnect
}pd_setting_t;

typedef enum
{
    no_inm_short = 0,
    inm_short
}input_short_t;

typedef struct
{
    transmitter_t transmit;
    led_current_range_t curr_range;
    adc_pwer_t adc_power;
    clk_mode_t clk_mode;
    tia_pwer_t tia_power;
    rest_of_adc_t rest_of_adc;
    afe_rx_mode_t afe_rx_mode;
    afe_mode_t afe_mode;
}dynamic_modes_t;

typedef struct
{
    pd_setting_t pd_setting;
    input_short_t short_setting;
    uint8_t ext_div;
}inm_inn_t;

typedef struct
{
    uint8_t offdac_pol_led2;
    uint8_t offdac_set_led2;
    uint8_t offdac_pol_amb1;
    uint8_t offdac_set_amb1;
    uint8_t offdac_pol_led1;
    uint8_t offdac_set_led1;
    uint8_t offdac_pol_amb2;
    uint8_t offdac_set_amb2;
}offdac_settings_t;

void heartrate_3_init();
int8_t hr3_set_timer_and_average_num( bool enable, uint8_t av_num );
int8_t hr3_set_settings( sw_reset_t sw_reset, diag_mode_t diag_mode, susp_count_t susp_counter, reg_read_t reg_read );
int8_t hr3_set_led2_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_led2_sample_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_led2_sample_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_adc_reset0_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_led2_convert_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_led3_start_stop( uint16_t start, uint16_t end );
int8_t hr3_set_led3_sample_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_adc_reset1_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_led3_convert_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_led1_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_led1_sample_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_adc_reset2_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_led1_convert_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_amb1_sample_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_adc_reset3_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_amb1_convert_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_pdn_cycle_start_end( uint16_t start, uint16_t end );
int8_t hr3_set_prpct_count( uint16_t count );
int8_t hr3_set_timer_and_average_num( bool enable, uint8_t av_num );
int8_t hr3_set_seperate_tia_gain( bool seperate, uint8_t cf_setting, uint8_t gain_setting );
int8_t hr3_set_tia_gain( bool replace, uint8_t cf_setting, uint8_t gain_setting );
int8_t hr3_set_led_currents( uint8_t led1_current, uint8_t led2_current, uint8_t led3_current );
int8_t hr3_set_dynamic_settings( dynamic_modes_t* modes );
int8_t hr3_set_clkout_div( bool enable, uint8_t div );
int8_t hr3_set_int_clk_div( uint8_t div );
void AFE_Reg_Write (int reg_address, unsigned long data);
long get_GR_data();
long get_RED_data();
long get_IR_data();
