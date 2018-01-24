/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty_min.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <math.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/hal/Seconds.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>

/* TI-RTOS Header files */
#include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
// #include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>

//Library for read data from optical sensor
#include "devices/i2ctransfer.h"
#include "devices/AFE4404.h"
#include "devices/HeartRate3Click.h"

//Library for signal processing
#include "signalProcessing/DCRemove.h"
#include "signalProcessing/lowPassFilter.h"
#include "signalProcessing/kalman.h"

/* Board Header files */
#include "Board.h"

#define TASKSTACKSIZE   1024
#define Board_I2C_TMP           CC1350_LAUNCHXL_I2C0

#define NUM_SUM_GREEN 20
#define NUM_AVERAGE_GREEN 30
#define NUM_TIME 20
#define NUM_BLE_ARRAY 20
#define SPEED_UPDATE_NEW_VALUE 3  // 4 speed level 1 - 2 - 3 - 4

const uint8_t spO2LUT[43] = {100,100,100,100,99,99,99,99,99,99,98,98,98,98,
                              98,97,97,97,97,97,97,96,96,96,96,96,96,95,95,
                              95,95,95,95,94,94,94,94,94,93,93,93,93,93};

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

I2C_Handle i2c;
I2C_Params i2cParams;
I2C_Transaction i2cTransaction;

UART_Handle uart;
UART_Params uartParams;

Clock_Struct mainClockStruct;
static Clock_Handle mainClockHandle;

Event_Struct operationEvent;
static Event_Handle operationEventHandle;

int8_t arrayForBLETransmitter[NUM_BLE_ARRAY];

/////Variable for HR and SpO2
static uint8_t heartRate = 0;
float redACValueSqSum = 0;
float irACValueSqSum = 0;
uint32_t samplesRecorded;
uint8_t spO2 = 0;
int doDai = 0;

static char *long_to_string_convert_for_test(long dataLong)
{
    static char substring1[10] ,substring2[10] ;
    int xoamang = 0;
    for(xoamang = 0 ; xoamang < 10 ; xoamang ++){
        substring1[xoamang] = 0;
        substring2[xoamang] = 0;
    }
    int dem = 0;
    long dataLongTemp = dataLong;
    if(dataLongTemp<0){
        dem++;
        substring2[0] = '-';
        dataLongTemp = abs(dataLongTemp);
    }
    while(dataLongTemp){
        substring1[dem] = dataLongTemp %10+48;
        dem++;
        dataLongTemp /=10;
    }
    doDai = dem;
    while(dem){
        if(dataLong >= 0){
            substring2[dem-1] = substring1[doDai - dem];
        }else{
            substring2[dem-1] = substring1[doDai - dem+1];
            if(dem == 2 ) break;
        }
        dem--;
    }
    return substring2;
}

void UART_init_setup(){
    //-----------------------UART BEGIN------------------//
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;
    uart = UART_open(Board_UART0, &uartParams);
    if (uart == NULL) {
        System_abort("Error opening the UART");
    }
    //-----------------------UART END--------------------//
}

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

void SFH7050Function(UArg arg0, UArg arg1)
{
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

    // variable for heart rate processing
    uint32_t timeArray[NUM_TIME];
    for(count = 0 ; count < NUM_TIME  ; count++){
        timeArray[count] = 0;
    }
    uint32_t timeSum = 0;
    static uint8_t timeSavedCount = 0, pulseCount,numPulseToUpdate,timeValidCount;
    static float averageTime =0;
    bool pulseUp = false, pulseDw = true;
    /////////////////////////////////////////

    i2cStartup();
    AFE_RESET_Init();
    AFE_Trigger_HWReset();
    AFE_Init();
    heartrate_3_init();
    //Setup kalman param
    SimpleKalmanFilter(2, 2, 0.0008);
    while (1) {
        uint32_t events = Event_pend(operationEventHandle, 0, 0xffffffff, BIOS_WAIT_FOREVER);
        if(events == 1){
            countBleElement ++;
            timeSum = 0;
            timeSavedCount = 0;
            timeValidCount = 0;
            dataLedGreen = get_GR_data();
            dataLedRed = get_RED_data();
            dataLedIR = get_IR_data();
            greenSumXValue = 0;
            greenSumXSum = 0;
            dataSum = dataLedGreen + dataLedRed + dataLedIR;

            //Filter
            dataLedGreenDCRemove = dcRemoval(dataLedGreen, 0.9);
            dataLedGreenLPF = LowPass_filter(dataLedGreenDCRemove,dataLedGreenLPF);
            dataLedGreenKalman = updateEstimate(dataLedGreenDCRemove);
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
                            heartRate = 6000000/averageTime;
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
//            if(dataSum< 110000 || dataSum > 250000 || dataLedGreen < 40000 || dataLedGreen > 90000){
//                for(count = 0 ; count < NUM_TIME  ; count++){
//                    timeArray[count] = 0;
//                }
//            }

            //////////////////Show data UART
            UART_write(uart, long_to_string_convert_for_test(averageGreenValue), doDai);
            UART_write(uart, "  ", 2);
                    UART_write(uart, long_to_string_convert_for_test(greenSumXValue), doDai);
                    UART_write(uart, "  ", 2);
                    UART_write(uart, long_to_string_convert_for_test(averageGreenValue), doDai);
            //        //Show Space
            //        UART_write(uart, "  ", 2);
                    //Show Raw Data
            //        UART_write(uart, long_to_string_convert_for_test(dataLedGreen), doDai);
            //        UART_write(uart, "  ", 2);
            //        UART_write(uart, long_to_string_convert_for_test(dataLedRed), doDai);
            //        UART_write(uart, "  ", 2);
            //        UART_write(uart, long_to_string_convert_for_test(dataLedIR), doDai);
            //        UART_write(uart, "  ", 2);
            //        UART_write(uart, long_to_string_convert_for_test(signalSum), doDai);
            //        UART_write(uart, "  ", 2);
            //        if(signalSum > 250000 && signalSum < 400000 && dataLedGreen > 50000 && dataLedGreen < 150000){
            //            UART_write(uart, long_to_string_convert_for_test(dataLedGreenKalman), doDai);
            //        }
                    UART_write(uart, endLine, sizeof(endLine)-1);
        }
    }
}

void mainClockTaskStructFunction(){
    Event_post(operationEventHandle, 1);
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();

    // Board_initI2C();
    // Board_initSPI();
    Board_initUART();
    UART_init_setup();
    // Board_initWatchdog();
    /* Construct heartBeat Task  thread */

    /* Create event used internally for state changes */
    Event_Params eventParam;
    Event_Params_init(&eventParam);
    Event_construct(&operationEvent, &eventParam);
    operationEventHandle = Event_handle(&operationEvent);

    Clock_Params clkParams;
    Clock_Params_init(&clkParams);
    clkParams.period = 1000;
    clkParams.startFlag = true;
    Clock_construct(&mainClockStruct, (Clock_FuncPtr)mainClockTaskStructFunction, 1, &clkParams);
    mainClockHandle = Clock_handle(&mainClockStruct);

    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)SFH7050Function, &taskParams, NULL);

    /* Start BIOS */
    BIOS_start();

    return (0);
}

