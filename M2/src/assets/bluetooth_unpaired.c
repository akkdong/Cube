#if 0
#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif
#else
#include <lvgl.h>
#endif

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_BLUETOOTH_UNPAIRED
#define LV_ATTRIBUTE_IMG_BLUETOOTH_UNPAIRED
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_BLUETOOTH_UNPAIRED uint8_t bluetooth_unpaired_map[] = {
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 0*/
  0xff, 0xff, 0xff, 0x03, 	/*Color of index 1*/
  0xff, 0xff, 0xff, 0xb2, 	/*Color of index 2*/
  0xff, 0xff, 0xff, 0x4f, 	/*Color of index 3*/
  0xff, 0xff, 0xff, 0xf1, 	/*Color of index 4*/
  0xff, 0xff, 0xff, 0x1c, 	/*Color of index 5*/
  0xff, 0xff, 0xff, 0x89, 	/*Color of index 6*/
  0xff, 0xff, 0xff, 0xc7, 	/*Color of index 7*/
  0xff, 0xff, 0xff, 0xfe, 	/*Color of index 8*/
  0xff, 0xff, 0xff, 0x66, 	/*Color of index 9*/
  0xff, 0xff, 0xff, 0xda, 	/*Color of index 10*/
  0xff, 0xff, 0xff, 0x28, 	/*Color of index 11*/
  0xff, 0xff, 0xff, 0x69, 	/*Color of index 12*/
  0xff, 0xff, 0xff, 0x6e, 	/*Color of index 13*/
  0xff, 0xff, 0xff, 0x97, 	/*Color of index 14*/
  0xff, 0xff, 0xff, 0x5c, 	/*Color of index 15*/

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb9, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x82, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x88, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xb1, 0x00, 0x00, 0xc8, 0x88, 0x82, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1f, 0xad, 0x10, 0x00, 0xc8, 0x88, 0x88, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xba, 0x84, 0xd1, 0x00, 0xc8, 0x8e, 0x78, 0x82, 0x50, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1d, 0x48, 0x4d, 0x10, 0xc8, 0x89, 0xb7, 0x88, 0x25, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xd4, 0x84, 0xd0, 0xc8, 0x8c, 0x06, 0x88, 0x43, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1d, 0x48, 0x4c, 0xd8, 0x8d, 0xc4, 0x84, 0xd1, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xd4, 0x84, 0x78, 0x87, 0x48, 0x4d, 0x10, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1d, 0x48, 0x88, 0x88, 0x84, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xd4, 0x88, 0x88, 0x4d, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1d, 0x88, 0x88, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1d, 0x48, 0x84, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xd4, 0x88, 0x88, 0x4d, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1d, 0x48, 0x88, 0x88, 0x84, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xd4, 0x84, 0x78, 0x87, 0x48, 0x4d, 0x10, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1d, 0x48, 0x4c, 0xd8, 0x8d, 0xc4, 0x84, 0xd1, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xd4, 0x84, 0xd0, 0xc8, 0x8c, 0x06, 0x88, 0x43, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1d, 0x48, 0x4d, 0x10, 0xc8, 0x8c, 0xb7, 0x88, 0x25, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xba, 0x84, 0xd1, 0x00, 0xc8, 0x8e, 0x78, 0x82, 0x50, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1f, 0xad, 0x10, 0x00, 0xc8, 0x88, 0x88, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xb1, 0x00, 0x00, 0xc8, 0x88, 0x82, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x88, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x82, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb9, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t bluetooth_unpaired = {
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 32,
  .header.h = 32,
  .data_size = 576,
  .data = bluetooth_unpaired_map,
};
