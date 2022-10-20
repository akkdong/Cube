// rm68120.h
//

#ifndef __RM68120_H__
#define __RM68120_H__

#include <stdlib.h>
#include <sys/cdefs.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_commands.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"


/* Define all screen direction */
typedef enum {
    /* @---> X
       |
       Y
    */
    SCR_DIR_LRTB,   /**< From left to right then from top to bottom, this consider as the original direction of the screen */

    /*  Y
        |
        @---> X
    */
    SCR_DIR_LRBT,   /**< From left to right then from bottom to top */

    /* X <---@
             |
             Y
    */
    SCR_DIR_RLTB,   /**< From right to left then from top to bottom */

    /*       Y
             |
       X <---@
    */
    SCR_DIR_RLBT,   /**< From right to left then from bottom to top */

    /* @---> Y
       |
       X
    */
    SCR_DIR_TBLR,   /**< From top to bottom then from left to right */

    /*  X
        |
        @---> Y
    */
    SCR_DIR_BTLR,   /**< From bottom to top then from left to right */

    /* Y <---@
             |
             X
    */
    SCR_DIR_TBRL,   /**< From top to bottom then from right to left */

    /*       X
             |
       Y <---@
    */
    SCR_DIR_BTRL,   /**< From bottom to top then from right to left */

    SCR_DIR_MAX,
} scr_dir_t;

typedef struct {
    esp_lcd_panel_t base;
    esp_lcd_panel_io_handle_t io;
    int reset_gpio_num;
    bool reset_level;
    uint16_t width;          // Current screen width, it may change when apply to rotate
    uint16_t height;         // Current screen height, it may change when apply to rotate
    uint8_t dir;             // Current screen direction
    int x_gap;
    int y_gap;
    unsigned int bits_per_pixel;
    uint8_t madctl_val;     // MADCTL register addr:3600H
    uint8_t colmod_cal;     // Color Format: COLMOD register addr:3A00H
} lcd_panel_t;


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t esp_lcd_new_panel_rm68120(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel);

#ifdef __cplusplus
}
#endif

#endif // __RM68120_H__
