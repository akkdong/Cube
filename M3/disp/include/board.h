// board.h
//

#ifndef __BOARD_H__
#define __BOARD_H__

#define UART_DEVICE_RX          (18)
#define UART_DEVICE_TX          (19)

#define EXT_KEY_LEFT            (91)
#define EXT_KEY_RIGHT           (92)
#define EXT_KEY_UP              (93)
#define EXT_KEY_DOWN            (94)

#define KEY_POWER_MAIN          (2)
#define KEY_POWER_EXT           (5)
#define KEY_POWER_EPD           (23)

#define KEY_LEFT                (37)
#define KEY_PUSH                (38)
#define KEY_RIGHT               (39)

#define SPI_SCLK                (14)
#define SPI_MISO                (13)
#define SPI_MOSI                (12)
#define SPI_CS_SD               (4)

#define I2C_SDA                 (21)
#define I2C_SCL                 (22)

#define TOUCH_SDA               (21)
#define TOUCH_SCL               (22)
#define TOUCH_INT               (36)

#define EPD_SCLK                (14)
#define EPD_MISO                (13)
#define EPD_MOSI                (12)
#define EPD_CS                  (15)
#define EPD_BUSY                (27)
#define EPD_RESET               (-1)

#define LCD_WIDTH               (960)
#define LCD_HEIGHT              (540)


#endif // __BOARD_H__
