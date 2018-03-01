#include <xdc/std.h>

#ifndef I2CTRANSFER_H
#define I2CTRANSFER_H


void I2C_read(int8_t SlaveID, int8_t RegisterAddress, uint8_t *Data, uint8_t len);

void I2C_write(int8_t SlaveID, int8_t RegisterAddress, uint8_t *Data, uint8_t len);

#endif
