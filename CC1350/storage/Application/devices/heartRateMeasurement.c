#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/UART.h>
//Library for signal processing
#include "signalProcessing/DCRemove.h"
#include "signalProcessing/lowPassFilter.h"
#include "signalProcessing/kalman.h"

//Library for read data from optical sensor
#include "heart_rate_3click/AFE4404.h"
#include "heart_rate_3click/HeartRate3Click.h"

#include "heartRateMeasurement.h"



/***** Variable declarations *****/
long dataLedGreen,dataLedGreenDCRemove,dataLedGreenLPF,dataLedGreenKalman;
long dataLedRed;
long dataLedIR;
int doDai = 0;

// extern UART_Handle uart;
// extern UART_Params uartParams;


static char *long_to_string_convert_for_test(long dataLong, int *doDai)
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
    *doDai = dem;
    while(dem){
        if(dataLong >= 0){
            substring2[dem-1] = substring1[*doDai - dem];
        }else{
            substring2[dem-1] = substring1[*doDai - dem+1];
            if(dem == 2 ) break;
        }
        dem--;
    }
    return substring2;
}

void mainFunction(void){
    const char endLine[] = "\n";
    // i2cStartup();
    AFE_RESET_Init();
    AFE_Trigger_HWReset();
    AFE_Init();
    heartrate_3_init();
    //Setup kalman param
    SimpleKalmanFilter(2, 2, 0.001);
    while (1) {
        dataLedGreen = get_GR_data();
        dataLedRed = get_RED_data();
        dataLedIR = get_IR_data();
        dataLedGreenDCRemove = dcRemoval(dataLedGreen, 0.9);
        dataLedGreenLPF = LowPass_filter(dataLedGreenDCRemove,dataLedGreenLPF);
        dataLedGreenKalman = updateEstimate(dataLedGreenDCRemove);
        // UART_write(uart, long_to_string_convert_for_test(dataLedGreenDCRemove, &doDai), doDai);
        // UART_write(uart, endLine, sizeof(endLine)-1);
    }
}
