
#include <stdint.h>
#include "i2ctransfer/i2ctransfer.h"
#include "dvMMA9553.h"

void pedometer_init(void){
  // pedometer_disable();
  // pedometer_enable();
  pedometer_reset();
  pedometer_write_config_default();
  pedometer_enable();         
  pedometer_int0_enable();    
  pedometer_active();        
  pedometer_wakeup();
  pedometer_afe_config();
  pedometer_cmd_readstatus();
  pedometer_afe_config_read();        
}


void pedometer_reset(void){
  uint8_t Buf[5] = {0x17, 0x20, 0x01, 0x01, 0x20};
  dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 5);
}


void pedometer_enable(void){
  uint8_t Buf[5]={0x17,0x20,0x05,0x01,0x00};
  dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 5);
}


void pedometer_disable(void){
 uint8_t Buf[5]={0x17,0x20,0x05,0x01,0x20};
 dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 5);
}


void pedometer_active(void){
  uint8_t Buf[5]={0x15,0x20,0x06,0x01,0x80};
  dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 5);
}


void pedometer_write_config_default(void){
  uint8_t Buf[20]={ 0x15,0x20,0x00,0x10,
          0x0C,0xE0,
          0x13,0x20,
          0x00,0x96,
          0x60,0x32,
          0xAA,0x3C,  // height/weight
          0x04,0x03,
          0x05,0x01,
          0x00,0x00};

  dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 20);
}

void pedometer_write_config(uint8_t *info){
  uint8_t Buf[20]={ 0x15,0x20,0x00,0x10,
          0x0C,0xE0,  // sleep min
          0x13,0x20,  // sleep max
          0x00,0x96,  // sleep count threshold
          0x60,0x32,  // config/step length 
          0xAA,0x3C,  // height/weight
          0x04,0x03,  // Filter Register
          0x05,0x01,  // speed period
          0x00,0x00}; // activity count
  Buf[12] = *(info); // height
  Buf[13] = *(info+1); // weight
  dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 20);
}


void pedometer_afe_config(void){
 uint8_t Buf[5]={0x06,0x20,0x00,0x01,0x40}; // 2g mode FS=01    1g--16393
 // uint8_t Buf[]={0x06,0x20,0x00,0x01,0x80}; // 4g mode FS=10    4g--8196
 // uint8_t Buf[]={0x06,0x20,0x00,0x01,0x00}; // 8g mode FS=00    8g--4098
  dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 5);
}


void pedometer_afe_config_read(void){
   uint8_t Buf[4]={0x06,0x10,0x00,0x01};
   dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf,4);
}


void pedometer_wakeup(void){
    uint8_t Buf[5]={0x12,0x20,0x06,0x01,0x00};
    dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 5);
}


//void pedometer_cmd_readwakeup(void){
//    uint8_t Buf[4]={0x12,0x10,0x06,0x01};
//    dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 4 );
//}


void pedometer_int0_enable(void){
    uint8_t Buf[5]={0x18,0x20,0x00,0x01,0xC0};
    dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 5);
}


void pedometer_cmd_readstatus(void){
    uint8_t Buf[4]={0x15,0x30,0x00,0x0C};
    dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 4);
}


//void pedometer_cmd_readconfig(void){
//    uint8_t Buf[4]={0x15,0x10,0x00,0x10};
//    dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 4 );
//}


//void pedometer_cmd_readrawxyz(void){
//    uint8_t Buf[4]={0x06,0x30,0x12,0x06};
//    dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 4 );
//}


//void pedometer_cmd_readlpfxyz(void){
//    uint8_t Buf[4]={0x06,0x30,0x18,0x06};
//    dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 4 );
//}


void pedometer_Read(uint8_t *buffer, uint8_t len){
  dvMMA9553_Read(MMA9553_Slave_Addr, MMA9553_Sub_Addr, buffer, len);
}


void pedometer_Write(uint8_t *buffer, uint8_t len){
  dvMMA9553_Write(MMA9553_Slave_Addr, MMA9553_Sub_Addr, buffer, len);   
}

void dvMMA9553_Read(uint8_t deviceAddr, uint8_t regAddr, uint8_t *data, uint8_t len){
  I2c_ReadRegister( deviceAddr, regAddr, data, len);
}

void dvMMA9553_Write( uint8_t deviceAddr, uint8_t regAddr, uint8_t *data, uint8_t len){
  I2c_WriteRegister(deviceAddr, regAddr, data, len);
}


void delay_mma9553(void){
  int i;
  for (i = 0; i < 40; ++i)
  {
    /* code */
  }
}

// void pedometer_mainrawxyz(void){
//     uint8_t Buf[20];
//     int x, y, z; 
   
//     pedometer_cmd__readrawxyz(); //
//     while(1)
//     {
//        dvMMA9553_Read(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 2);
//        Serial.println("uint8_t Buf[1]==%02x\r\n",uint8_t Buf[1]);
//        delay_wwx();
//         if(uint8_t Buf[1]==0x80)
//         {
//             dvMMA9553_Read(MMA9553_Slave_Addr, MMA9553_Sub_Addr, Buf, 10);  
//             break;
//         }
//     }
//     // wenxue      
//     for(int i=0;i<10;i++)
//         Serial.print("uint8_t Buf[%d]=%02x\r\n",i,uint8_t Buf[i]);       
//     x = uint8_t Buf[4] * 256 + uint8_t Buf[5];
//     y = uint8_t Buf[6] * 256 + uint8_t Buf[7];
//     z = uint8_t Buf[8] * 256 + uint8_t Buf[9];

//     Serial.print("x=%d\r\n",x);
//     Serial.print("y=%d\r\n",y);
//     Serial.println("z=%d\r\n",z);
// }



/******************************************************************** 
*                Complete source dvMMA9553.h
********************************************************************/
