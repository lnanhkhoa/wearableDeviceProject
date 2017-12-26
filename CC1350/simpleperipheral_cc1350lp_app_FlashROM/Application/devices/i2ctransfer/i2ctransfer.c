

/********************************************************************
*                File arduino_i2c.h
********************************************************************/
#include <ti/drivers/I2C.h>
#include "i2ctransfer.h"


extern I2C_Handle i2c;
extern I2C_Params i2cParams;
extern I2C_Transaction i2cTransaction;



void I2c_ReadRegister(int8_t SlaveID, int8_t RegisterAddress, uint8_t *Data, uint8_t len){
    uint8_t writeReg = RegisterAddress;
    i2cTransaction.slaveAddress = SlaveID;
    i2cTransaction.writeCount = 1;
    i2cTransaction.writeBuf = &writeReg;
    i2cTransaction.readCount = len;
    i2cTransaction.readBuf = Data;
    if(I2C_transfer(i2c, &i2cTransaction));
}



void I2c_WriteRegister( int8_t SlaveID, int8_t RegisterAddress, uint8_t *Data, uint8_t len){
    uint8_t writeBuffer[20];
    uint8_t i;
    writeBuffer[0] = RegisterAddress;
    for(i=0;i<len;i++)
        writeBuffer[i+1] = *(Data+i);
    i2cTransaction.slaveAddress = SlaveID;
    i2cTransaction.writeCount = len+1;
    i2cTransaction.writeBuf = writeBuffer;
    i2cTransaction.readCount = 0;
    i2cTransaction.readBuf = NULL;
    if(I2C_transfer(i2c, &i2cTransaction));
}

void I2c_WriteByte( int8_t SlaveID, int8_t RegisterAddress, uint8_t Data){
    uint8_t writeBuffer[20];
    writeBuffer[0] = RegisterAddress;
    writeBuffer[1] = Data;
    i2cTransaction.slaveAddress = SlaveID;
    i2cTransaction.writeCount = 2;
    i2cTransaction.writeBuf = writeBuffer;
    i2cTransaction.readCount = 0;
    i2cTransaction.readBuf = NULL;
    if(I2C_transfer(i2c, &i2cTransaction));
}


/********************************************************************
*                Complete arduino_i2c.h
********************************************************************/
