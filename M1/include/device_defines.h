// device_defines.h
//

#ifndef __DEVICE_DEFINES_H__
#define __DEVICE_DEFINES_H__

// peripheral-pin
#define POWER_ON		4

#define BTN_SEL			26
#define BTN_LEFT		34
#define BTN_RIGHT		35
#define BTN_UP			5
#define BTN_DOWN		2

#define UART1_RX        32
#define UART1_TX        -1

#define I2C0_SDA        18
#define I2C0_SCL        19

#define LCD_RST         22
#define LCD_BL          23
#define LCD_RS          21
#define LCD_CS          15
#define LCD_CLK         14
#define LCD_DO          13
#define LCD_DI          -1

// LCD dimension : 270 rotated
#define LCD_WIDTH       480
#define LCD_HEIGHT      320


#endif // __DEVICE_DEFINES_H__