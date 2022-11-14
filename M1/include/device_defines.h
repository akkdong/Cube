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


#ifndef PI
#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#endif


#define VARIOMETER_LXNAV_SENTENCE				(1)
#define VARIOMETER_LK8_SENTENCE					(2)
#define VARIOMETER_DEFAULT_NMEA_SENTENCE		VARIOMETER_LK8_SENTENCE

#define VARIOMETER_SENTENCE_DELAY				(1000)   // tick = 1000/25 = 40, 25 tick = 1000
#define SENSOR_UPDATE_FREQUENCY                 (25)    // 25Hz


#define KEY_ENTER           (0xB0)
#define KEY_LEFT            (0xD8)
#define KEY_RIGHT           (0xD7)
#define KEY_UP              (0xDA)
#define KEY_DOWN            (0xD9)


#endif // __DEVICE_DEFINES_H__