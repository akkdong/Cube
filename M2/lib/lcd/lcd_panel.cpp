// lcd_panel.cpp
//

#include "board.h"
#include "lcd_panel.h"


//////////////////////////////////////////////////////////////////////////////////////
//

LcdPanel::LcdPanel() : lcd_panel(nullptr)
{

}


void LcdPanel::begin()
{
    /* Init TCA9554 -> turn on backlight */
    #if 0
    ESP_ERROR_CHECK(tca9554_init());
    ext_io_t io_conf = BSP_EXT_IO_DEFAULT_CONFIG();
    ext_io_t io_level = BSP_EXT_IO_DEFAULT_LEVEL();
    ESP_ERROR_CHECK(tca9554_set_configuration(io_conf.val));
    ESP_ERROR_CHECK(tca9554_write_output_pins(io_level.val));
    #endif

    ESP_LOGI(TAG, "Initialize LCD Intel 8080 bus");
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = GPIO_LCD_RS,
        .wr_gpio_num = GPIO_LCD_WR,
        .data_gpio_nums = {
            GPIO_LCD_D00, GPIO_LCD_D01, GPIO_LCD_D02, GPIO_LCD_D03, 
            GPIO_LCD_D04, GPIO_LCD_D05, GPIO_LCD_D06, GPIO_LCD_D07, 
            GPIO_LCD_D08, GPIO_LCD_D09, GPIO_LCD_D10, GPIO_LCD_D11,
            GPIO_LCD_D12, GPIO_LCD_D13, GPIO_LCD_D14, GPIO_LCD_D15,
        },
        .bus_width = LCD_BIT_WIDTH,
        .max_transfer_bytes = LCD_WIDTH * 40 * sizeof(uint16_t) // maximum 32640 *2, limited by DMA 
    };
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = GPIO_LCD_CS,
        .pclk_hz = 10000000, // test over 14 MHz and above 10MHZ=26.18fps 13MHZ=34.04fps
        .trans_queue_depth = 10,
        .on_color_trans_done = nullptr, // example_notify_lvgl_flush_ready,  //Callback invoked when color data transfer has finished
        .user_ctx = nullptr, // &disp_drv,                                   //User private data, passed directly to on_color_trans_done’s user_ctx
        .lcd_cmd_bits = 16,
        .lcd_param_bits = 16,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        //.flags {
        //    .cs_active_high = 0,
        //    .reverse_color_bits = 0,
        //    .swap_color_bytes = 0,
        //    .pclk_active_neg = 0,
        //    .pclk_idle_low = 0,
        //}
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    ESP_LOGI(TAG, "Install LCD driver of rm68120");    
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = GPIO_LCD_RST,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_rm68120(io_handle, &panel_config, &lcd_panel));

    esp_lcd_panel_reset(lcd_panel);   // LCD Reset
    esp_lcd_panel_init(lcd_panel);    // LCD init
    // esp_lcd_panel_swap_xy(lcd_panel, true);
    // esp_lcd_panel_mirror(lcd_panel, true, false);

    // lcd_clear_fast(lcd_panel, COLOR_WHITE);
    // lcd_draw_picture_test(lcd_panel);
    // lcd_speed_test(lcd_panel);
}
