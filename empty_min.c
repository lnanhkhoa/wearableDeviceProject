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

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/hal/Seconds.h>
#include <ti/sysbios/knl/Clock.h>

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

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

long dataLedGreen,dataLedGreenDCRemove,dataLedGreenLPF,dataLedGreenKalman;
long dataLedRed;
long dataLedIR;

I2C_Handle i2c;
I2C_Params i2cParams;
I2C_Transaction i2cTransaction;

UART_Handle uart;
UART_Params uartParams;

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
    uartParams.baudRate = 9600;
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

Void SFH7050Function(UArg arg0, UArg arg1)
{
    const char endLine[] = "\n";
    i2cStartup();
    AFE_RESET_Init();
    AFE_Trigger_HWReset();
    AFE_Init();
    heartrate_3_init();
    //Setup kalman param
    SimpleKalmanFilter(2, 2, 0.001);
    while (1) {
        Task_sleep((UInt)arg0);
        dataLedGreen = get_GR_data();
        dataLedRed = get_RED_data();
        dataLedIR = get_IR_data();
        dataLedGreenDCRemove = dcRemoval(dataLedGreen, 0.9);
        dataLedGreenLPF = LowPass_filter(dataLedGreenDCRemove,dataLedGreenLPF);
        dataLedGreenKalman = updateEstimate(dataLedGreenDCRemove);
        UART_write(uart, long_to_string_convert_for_test(dataLedGreenDCRemove), doDai);
        UART_write(uart, endLine, sizeof(endLine)-1);
    }
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
    Task_Params_init(&taskParams);
    taskParams.arg0 = 1000 / Clock_tickPeriod;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)SFH7050Function, &taskParams, NULL);

    /* Start BIOS */
    BIOS_start();

    return (0);
}
