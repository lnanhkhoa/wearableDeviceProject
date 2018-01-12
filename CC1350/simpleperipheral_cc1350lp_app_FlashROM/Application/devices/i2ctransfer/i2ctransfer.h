#ifndef I2CTRANSFER_H
#define I2CTRANSFER_H


void I2c_ReadRegister(int8_t SlaveID, int8_t RegisterAddress, uint8_t *Data, uint8_t len);

void I2c_WriteRegister(int8_t SlaveID, int8_t RegisterAddress, uint8_t *Data, uint8_t len);

void I2c_WriteByte( int8_t SlaveID, int8_t RegisterAddress, uint8_t Data);

#endif
