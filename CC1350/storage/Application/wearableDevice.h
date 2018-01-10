
#ifndef TASKS_WEARABLEDEVICE_H_
#define TASKS_WEARABLEDEVICE_H_

#define EVENT_ALL                           0xFFFFFFFF
#define EVENT_UPDATE_OLED                   (uint32_t)(1 << 0)
#define EVENT_SLEEPMODE_START               (uint32_t)(1 << 1)
#define EVENT_SLEEPMODE_COMPLETE            (uint32_t)(1 << 2)
#define EVENT_TURNON_DISPLAY                (uint32_t)(1 << 3)

#define TASKSTACKSIZE           1024
#define MAINTASK_PRIORITY       4
#define PRIMARYTASK_PRIORITY    3
#define COUNTSLEEPTASK_PRIORITY 2

#define OLED_RESET 4
#define Display_ON true
#define Display_OFF false
#define BLACK 0
#define WHITE 1
#define INVERSE 2

void wearableDevice_init();

/* API for any header files need */
bool checkCorrectTime(uint8_t *newdatetime);
extern void updateTime(uint8_t *newdatetime);
extern void updateConfigProfile(uint8_t *newconfigprofile);
extern void testing (uint8_t number);
void updateBatteryInDevice(uint8_t percent);
uint32_t getVoltage();
/*********************************************************************/

#endif
