
#ifndef TASKS_WEARABLEDEVICE_H_
#define TASKS_WEARABLEDEVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define EVENT_ALL                           0xFFFFFFFF
#define EVENT_UPDATE_OLED                   (uint32_t)(1 << 0)

#define EVENT_SLEEPMODE_START               (uint32_t)(1 << 1)
#define EVENT_SLEEPMODE_COMPLETE            (uint32_t)(1 << 2)
#define EVENT_TURNON_DISPLAY                (uint32_t)(1 << 3)

#define OLED_RESET 4
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2
#define Display_ON true
#define Display_OFF false
#define BLACK 0
#define WHITE 1
#define INVERSE 2

#define TASKSTACKSIZE           1024
#define _TASKSTACKSIZE          1024
#define MAINTASK_PRIORITY       2
#define PRIMARYTASK_PRIORITY    2
#define COUNTSLEEPTASK_PRIORITY 4


extern void wearableDevice_init(void);

/*************API*******************
*/
// extern void updateTime(uint8_t *newDateTime);

// extern void updateConfigProfile(uint8_t *newConfigProfile);


#ifdef __cplusplus
}
#endif

#endif
