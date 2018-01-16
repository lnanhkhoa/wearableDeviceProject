
#ifndef AFE4404_H_
#define AFE4404_H_

#define RESET_AFE  Board_LED0

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

#endif
