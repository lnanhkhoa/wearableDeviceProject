
#ifndef __DVMMA9553_H__
#define __DVMMA9553_H__

#define MMA9553_Slave_Addr  0x4C
#define MMA9553_Sub_Addr    0x00

void pedometer_init(void);
void pedometer_reset(void);
void pedometer_enable(void);
void pedometer_disable(void);
void pedometer_active(void);
void pedometer_write_config(void);
void pedometer_afe_config(void);
void pedometer_afe_config_read(void);
void pedometer_wakeup(void);
void pedometer_cmd_readwakeup(void);
void pedometer_int0_enable(void);
void pedometer_cmd_readstatus(void);
void pedometer_cmd_readconfig(void);
void pedometer_cmd_readrawxyz(void);
void pedometer_cmd_readlpfxyz(void);
void pedometer_Read(uint8_t *buffer, uint8_t len);
void pedometer_Write(uint8_t *buffer, uint8_t len);

void dvMMA9553_Read(uint8_t deviceAddr, uint8_t regAddr, uint8_t *data, uint8_t len);
void dvMMA9553_Write(uint8_t deviceAddr, uint8_t regAddr, uint8_t *data, uint8_t len);
void delay_mma9553(void);

#endif
