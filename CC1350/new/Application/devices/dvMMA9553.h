
#ifndef __DVMMA9553_H__
#define __DVMMA9553_H__

#define MMA9553_Slave_Addr  0x4C
#define MMA9553_Sub_Addr    0x00

extern void pedometer_init(void);

static void pedometer_reset(void);
static void pedometer_enable(void);
// static void pedometer_disable(void);
static void pedometer_active(void);
static void pedometer_write_config_default(void);
static void pedometer_write_config(uint8_t *info);
static void pedometer_afe_config(void);
static void pedometer_afe_config_read(void);
static void pedometer_wakeup(void);
static void pedometer_int0_enable(void);
// static void pedometer_cmd_readwakeup(void);
// static void pedometer_cmd_readconfig(void);
// static void pedometer_cmd_readrawxyz(void);
// static void pedometer_cmd_readlpfxyz(void);

extern void pedometer_cmd_readstatus(void);
extern void pedometer_Read(uint8_t *buffer, uint8_t len);
extern void pedometer_Write(uint8_t *buffer, uint8_t len);
extern void delay_mma9553(void);

static void dvMMA9553_Read(uint8_t deviceAddr, uint8_t regAddr, uint8_t *data, uint8_t len);
static void dvMMA9553_Write(uint8_t deviceAddr, uint8_t regAddr, uint8_t *data, uint8_t len);

#endif
