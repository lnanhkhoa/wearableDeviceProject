#include "Board.h"
void AFE_Init();
void AFE_Reg_Init();
void AFE_Enable_Read ();
void AFE_Disable_Read ();
void AFE_RESET_Init();
void AFE_Trigger_HWReset();
void  AFE_Enable_HWPDN();
void  AFE_Disable_HWPDN();
void AFE_CLK_Init();
void AFE_Reg_Write (int reg_address, unsigned long data);
signed long AFE_Reg_Read(int reg_address);

#define RESET_AFE  Board_LED0
