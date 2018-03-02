#include "stdint.h"
#include "stdbool.h"
#include <math.h>

#include "Board.h"
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>

#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>
// #include <ti/drivers/UART.h>

//Library for read data from optical sensor
#include "devices/heart_rate_3click/AFE4404.h"
#include "devices/heart_rate_3click/HeartRate3Click.h"

//Library for signal processing
#include "signalProcessing/DCRemove.h"
#include "signalProcessing/lowPassFilter.h"
#include "signalProcessing/kalman.h"

#include "measurementModule.h"

#define NUM_SUM_GREEN 20
#define NUM_AVERAGE_GREEN 30
#define NUM_TIME 20
#define NUM_BLE_ARRAY 20
#define SPEED_UPDATE_NEW_VALUE 3  // 4 speed level 1 - 2 - 3 - 4

extern I2C_Handle i2c;
extern I2C_Params i2cParams;
extern I2C_Transaction i2cTransaction;

extern Semaphore_Handle txSendSem;
// UART_Handle uart;
// UART_Params uartParams;

int8_t arrayForBLETransmitter[NUM_BLE_ARRAY];

/////Variable for HR and SpO2
uint16_t heartRateNumber = 0;
float redACValueSqSum = 0;
float irACValueSqSum = 0;
uint32_t samplesRecorded;
uint8_t spO2 = 0;
int doDai = 0;
const char endLine[] = "\n";
uint8_t count =0;
static uint8_t countBleElement;
long averageGreenValue = 0;
long greenSumXValue = 0;
long greenSumXSum = 0;
static long dataLedGreen,dataLedGreenDCRemove,dataLedGreenLPF,dataLedGreenKalman;
static long dataLedRed;
static long dataLedIR;
static long dataSum;
static uint32_t time;
static long signalGreenArray[NUM_SUM_GREEN], signalSumGreenArray[NUM_AVERAGE_GREEN];
static long timeDiff = 0;
const uint8_t spO2LUT[43] = {100,100,100,100,99,99,99,99,99,99,98,98,98,98,
                              98,97,97,97,97,97,97,96,96,96,96,96,96,95,95,
                              95,95,95,95,94,94,94,94,94,93,93,93,93,93};

// variable for heart rate processing
uint32_t timeArray[NUM_TIME];
uint32_t timeSum = 0;
static uint8_t timeSavedCount = 0, pulseCount,numPulseToUpdate,timeValidCount;
static float averageTime =0;
bool pulseUp = false, pulseDw = true;
bool hrStatus = true;
/////////////////////////////////////////

// static char *long_to_string_convert_for_test(long dataLong)
// {
//     static char substring1[10] ,substring2[10] ;
//     int xoamang = 0;
//     for(xoamang = 0 ; xoamang < 10 ; xoamang ++){
//         substring1[xoamang] = 0;
//         substring2[xoamang] = 0;
//     }
//     int dem = 0;
//     long dataLongTemp = dataLong;
//     if(dataLongTemp<0){
//         dem++;
//         substring2[0] = '-';
//         dataLongTemp = abs(dataLongTemp);
//     }
//     while(dataLongTemp){
//         substring1[dem] = dataLongTemp %10+48;
//         dem++;
//         dataLongTemp /=10;
//     }
//     doDai = dem;
//     while(dem){
//         if(dataLong >= 0){
//             substring2[dem-1] = substring1[doDai - dem];
//         }else{
//             substring2[dem-1] = substring1[doDai - dem+1];
//             if(dem == 2 ) break;
//         }
//         dem--;
//     }
//     return substring2;
// }

// void UART_init_setup(){
//     //-----------------------UART BEGIN------------------//
//     UART_Params_init(&uartParams);
//     uartParams.writeDataMode = UART_DATA_BINARY;
//     uartParams.readDataMode = UART_DATA_BINARY;
//     uartParams.readReturnMode = UART_RETURN_FULL;
//     uartParams.readEcho = UART_ECHO_OFF;
//     uartParams.baudRate = 115200;
//     uart = UART_open(Board_UART0, &uartParams);
//     if (uart == NULL) {

//     }
//     //-----------------------UART END--------------------//
// }

void i2cStartup(){
    I2C_init();
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(Board_I2C_TMP, &i2cParams);
    if (i2c == NULL){
        while (1);
    }
}

/////////////////Reset value
void SpO2_Reset()
{
    redACValueSqSum = 0;
    irACValueSqSum = 0;
    spO2 = 0;
    samplesRecorded = 0;
}
//////////////////////////////////////

void HR_measurementInit(){
    // i2cStartup();
    AFE_RESET_Init();
    AFE_Trigger_HWReset();
    AFE_Init();
    heartrate_3_init();
    // Board_initUART();
    // UART_init_setup();
    SimpleKalmanFilter(2, 2, 0.0008);
    for(count = 0 ; count < NUM_TIME  ; count++){
        timeArray[count] = 0;
    }
}

void HR_measurementTask(){
    countBleElement ++;
    timeSum = 0;
    timeSavedCount = 0;
    timeValidCount = 0;
    greenSumXValue = 0;
    greenSumXSum = 0;
    HR_getRawData();
    HR_calc();
    HR_filterModule();
    HR_TurnONOFF_HRModule(true);
}

void HR_getRawData(){
    dataLedGreen = get_GR_data();
    dataLedRed = get_RED_data();
    dataLedIR = get_IR_data();
}

void HR_filterModule(){
    //Filter
    dataLedGreenDCRemove = dcRemoval(dataLedGreen, 0.9);
    dataLedGreenLPF = LowPass_filter(dataLedGreenDCRemove,dataLedGreenLPF);
    dataLedGreenKalman = updateEstimate(dataLedGreenDCRemove);
}

void HR_calc(){
    dataSum = dataLedGreen + dataLedRed + dataLedIR;
    //Calculator Sum X Green Value
    for(count = 0 ; count < NUM_SUM_GREEN - 1 ; count++){
        signalGreenArray[count] = signalGreenArray[count+1];
        greenSumXValue += signalGreenArray[count];
    }
    signalGreenArray[NUM_SUM_GREEN - 1] = dataLedGreenKalman;
    greenSumXValue += signalGreenArray[NUM_SUM_GREEN - 1];
    //////////////////////////////////////////////

    //Calculator Average X Green Value
    for(count = 0 ; count < NUM_AVERAGE_GREEN - 1 ; count++){
        signalSumGreenArray[count] = signalSumGreenArray[count+1];
        greenSumXSum += signalSumGreenArray[count];
    }
    signalSumGreenArray[NUM_AVERAGE_GREEN - 1] = greenSumXValue;
    //greenSumXSum += signalSumGreenArray[NUM_AVERAGE_GREEN - 1];
    averageGreenValue = (greenSumXSum + signalSumGreenArray[NUM_AVERAGE_GREEN - 1] * 5)/(NUM_AVERAGE_GREEN+4);
    //////////////////////////////////////////////

    //Save to BLE array
    if(countBleElement >= 20){
        countBleElement = 0;

        //Here  ------------------------------>  Send Data to smartphone
        Semaphore_post(txSendSem);
        /////////////////////////End///////////////////////////////////

        for(count = 0 ; count < NUM_BLE_ARRAY ; count++){
            arrayForBLETransmitter[count] = 0;
        }
    }
    arrayForBLETransmitter[countBleElement] = averageGreenValue>>8;
    //////////////////////////////////////////////

    greenSumXValue = dcRemoval1(greenSumXValue, 0.9);
    averageGreenValue = dcRemoval2(averageGreenValue, 0.9);

    //Processing HR and SpO2
    if(dataSum> 30000 && dataSum < 300000 && dataLedGreen > 10000 && dataLedGreen < 100000){
        irACValueSqSum += dataLedRed * dataLedRed;
        redACValueSqSum += dataLedIR * dataLedIR;
        ++samplesRecorded;
        if(greenSumXValue > averageGreenValue && pulseDw){
            pulseUp = true;
            pulseDw = false;
            pulseCount++;
            time = Clock_getTicks();
            for(count = 0 ; count < NUM_TIME - 1 ; count++){
                if(timeArray[count+1] > timeArray[count] && timeArray[count] != 0){
                    timeSum += timeArray[count+1] - timeArray[count];
                    timeSavedCount++;
                }
                timeArray[count] = timeArray[count+1];
            }
            timeArray[NUM_TIME-1] = time;
            averageTime = timeSum/timeSavedCount;
            for(count = 0 ; count < NUM_TIME - 1 ; count++){
                if(timeArray[count+1] > timeArray[count] && timeArray[count] != 0){
                    timeDiff = timeArray[count+1] - timeArray[count];
                    if(timeDiff > averageTime*0.7 && timeDiff < averageTime*1.3){
                        timeValidCount++;
                    }else{
                        timeValidCount = 0;
                    }
                }
            }
            switch (SPEED_UPDATE_NEW_VALUE) {
                    case 1:
                        numPulseToUpdate = 10;
                        break;
                    case 2:
                        numPulseToUpdate = 6;
                        break;
                    case 3:
                        numPulseToUpdate = 3;
                        break;
                    default:
                        numPulseToUpdate = 1;
                        break;
            }
            if(pulseCount > numPulseToUpdate){
                if(timeValidCount > NUM_TIME / 2){
                    heartRateNumber = 6000000/averageTime;
                    float acSqRatio = 100.0 * log(redACValueSqSum/samplesRecorded) / log(irACValueSqSum/samplesRecorded);
                    static uint8_t index = 0;

                    if (acSqRatio > 66) {
                        index = (uint8_t)acSqRatio - 56;
                    } else if (acSqRatio > 50) {
                        index = (uint8_t)acSqRatio - 40;
                    }
                    SpO2_Reset();
                    spO2 = spO2LUT[index];
                    if(index > 42){
                        spO2 = 99;
                    }
                }
                pulseCount = 0;
            }
        }
        if(greenSumXValue < averageGreenValue && pulseUp){
            pulseUp = false;
            pulseDw = true;
        }
    }
    uint32_t i = 0 ;
    for(i = 0 ; i < 1000 ; i++){

    }
    //////////////////Show data UART
    // UART_write(uart, long_to_string_convert_for_test(averageGreenValue), doDai);
    // UART_write(uart, "  ", 2);
    // UART_write(uart, long_to_string_convert_for_test(greenSumXValue), doDai);
    // UART_write(uart, endLine, sizeof(endLine)-1);
}

uint8_t HR_getRealData(){
    if(heartRateNumber> 40 && heartRateNumber < 200){
        return heartRateNumber;
    }else{
        return 0;
    }

}

void HR_TurnONOFF_HRModule(bool turn){
    hrStatus =  turn;
    if(turn) hr3_set_led_currents( 10,0,0 );
    else hr3_set_led_currents( 0,0,0 );
}
