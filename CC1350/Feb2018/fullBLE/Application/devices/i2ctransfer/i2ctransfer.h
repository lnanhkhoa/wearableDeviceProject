#ifndef I2CTRANSFER_H_
#define I2CTRANSFER_H_


#ifdef __cplusplus
extern "C"
{
#endif

void I2c_ReadRegister(int8_t SlaveID, int8_t RegisterAddress, uint8_t *Data, uint8_t len);

void I2c_WriteRegister(int8_t SlaveID, int8_t RegisterAddress, uint8_t *Data, uint8_t len);

void I2c_WriteByte( int8_t SlaveID, int8_t RegisterAddress, uint8_t Data);


#ifdef __cplusplus
}
#endif

#endif
