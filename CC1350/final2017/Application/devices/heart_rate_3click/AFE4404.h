
#ifndef AFE4404_H_
#define AFE4404_H_


#ifdef __cplusplus
extern "C"
{
#endif

	void AFE_Init(void);
	void AFE_Reg_Init(void);
	void AFE_Enable_Read (void);
	void AFE_Disable_Read (void);
	void AFE_RESET_Init(void);
	void AFE_Trigger_HWReset(void);
	void AFE_Enable_HWPDN(void);
	void AFE_Disable_HWPDN(void);
	void AFE_CLK_Init(void);
	void AFE_Reg_Write (int reg_address, unsigned long data);
	long AFE_Reg_Read(int reg_address);
	long get_GR_data(void);
	long get_RED_data(void);
	long get_IR_data(void);
	void AFE_TurnLED(bool turn);


#ifdef __cplusplus
}
#endif


#endif
