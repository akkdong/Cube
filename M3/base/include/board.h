// board.h
//

#ifndef __BOARD_H__
#define __BOARD_H__

#define GPIO_I2C_SDA        (10)
#define GPIO_I2C_SCL        (13)

#define UART_HOST_RX        (39)    // device-side rx (host-side tx)
#define UART_HOST_TX        (37)    // device-side tx (host-side rx)

#define UART_GPS_RX         (2)     // device-side rx (gps-side tx)
#define UART_GPS_TX         (1)     // device-side tx (gps-side rx)

#define GPIO_KEY_UP         (38)
#define GPIO_KEY_DOWN       (34)
#define GPIO_KEY_LEFT       (36)
#define GPIO_KEY_RIGHT      (40)


#endif // __BOARD_H__
