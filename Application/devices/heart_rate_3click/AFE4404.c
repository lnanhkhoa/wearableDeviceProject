#include "i2ctransfer/i2ctransfer.h"
#include "HeartRate3Click.h"
#include <ti/drivers/PIN.h>
#include <ti/sysbios/knl/Task.h>
#include "AFE4404.h"

PIN_Config ledPinTable[] = {
    RESET_AFE | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

 static PIN_Handle ledPinHandle;
 static PIN_State ledPinState;

void AFE_Init()
{
  AFE_RESET_Init();
  AFE_Enable_HWPDN();
  AFE_Disable_HWPDN();
  AFE_Trigger_HWReset();
  //AFE_CLK_Init();
  AFE_Reg_Init();

}

/**********************************************************************************************************/
/*                              AFE4404_Registerinitialisierung                                           */
/**********************************************************************************************************/
void AFE_Reg_Init()
{
  AFE_Reg_Write(34, 0x0033C3);
}


/**********************************************************************************************************/
/*                              AFE4404_Enable_Read                                                       */
/**********************************************************************************************************/
void AFE_Enable_Read ()         //Prohibit writing to registers
{
  uint8_t configData[3];
  configData[0]=0x00;
  configData[1]=0x00;
  configData[2]=0x01;
  I2C_write (AFE_ADDR, AFE_CONTROL0, configData, 3);
}

/**********************************************************************************************************/
/*                              AFE4404_Disable_Read                                                      */
/**********************************************************************************************************/
void AFE_Disable_Read ()        //Permitt writing to registers
{
  uint8_t configData[3];
  configData[0]=0x00;
  configData[1]=0x00;
  configData[2]=0x00;
  I2C_write (AFE_ADDR, AFE_CONTROL0, configData, 3);
}

/**********************************************************************************************************/
/*                  RESETZ des AFE4404 wird Initialisiert                                                 */
/**********************************************************************************************************/
void AFE_RESET_Init()
{
    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
    PIN_setOutputValue(ledPinHandle, RESET_AFE, 1);
}

/**********************************************************************************************************/
/*                  Reset internal registers by setting RESETZ = LOW for 25 - 50 us                       */
/**********************************************************************************************************/
void AFE_Trigger_HWReset()
{
    PIN_setOutputValue(ledPinHandle, RESET_AFE, 0);
    Task_sleep(1000);
    PIN_setOutputValue(ledPinHandle, RESET_AFE, 1);
    Task_sleep(1000);
}

/**********************************************************************************************************/
/*                             AFE4404 Power Down                                                         */
/**********************************************************************************************************/
void  AFE_Enable_HWPDN()
{
    PIN_setOutputValue(ledPinHandle, RESET_AFE, 0);
    Task_sleep(10000);
}

/**********************************************************************************************************/
/*                                AFE4404 Power Up                                                        */
/**********************************************************************************************************/
void  AFE_Disable_HWPDN()
{
    PIN_setOutputValue(ledPinHandle, RESET_AFE, 1);
    Task_sleep(10000);
}

/**********************************************************************************************************/
/*                                AFE4404 Set Clock Mode to Internal                                      */
/**********************************************************************************************************/
void AFE_CLK_Init()
{
  AFE_Reg_Write(35, 0x104218);        //Set CLK Mode to internal clock (default Wert: 124218 mit interner CLK)
  AFE_Reg_Write(41, 0x2);             //Don´t set the internal clock to the CLK pin for outside usage
  AFE_Reg_Write(49, 0x000021);        //Division ratio for external clock mode set to fit the Arduino Mega 16MHz
  AFE_Reg_Write(57, 0);               //Set the lowes sampling rate to 61Hz (~17 ms)
}

/*********************************************************************************************************/
/*                                   AFE4404_Reg_Write                                                   */
/*********************************************************************************************************/
void AFE_Reg_Write (int reg_address, unsigned long data)
{
  uint8_t configData[3];
  configData[0]=(uint8_t)(data >>16);
  configData[1]=(uint8_t)(((data & 0x00FFFF) >>8));
  configData[2]=(uint8_t)(((data & 0x0000FF)));
  I2C_write(AFE_ADDR, reg_address, configData, 3);
}

/*********************************************************************************************************/
/*                                   AFE4404_Reg_Read                                                    */
/*********************************************************************************************************/
signed long AFE_Reg_Read(int reg_address)
{
    uint8_t configData[3];
  signed long retVal;
  I2C_read (AFE_ADDR, reg_address, configData, 3);
  retVal = configData[0];
  retVal = (retVal << 8) | configData[1];
  retVal = (retVal << 8) | configData[2];
  if (reg_address >= 0x2A && reg_address <= 0x2F)
  {
    if (retVal & 0x00200000)  // check if the ADC value is positive or negative
    {
      retVal &= 0x003FFFFF;   // convert it to a 22 bit value
      return (retVal^0xFFC00000);
    }
  }
  return retVal;
}
