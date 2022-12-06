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

#ifndef LV_ATTRIBUTE_IMG_BLUETOOTH_DISABLED
#define LV_ATTRIBUTE_IMG_BLUETOOTH_DISABLED
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_BLUETOOTH_DISABLED uint8_t bluetooth_disabled_map[] = {
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 0*/
  0xff, 0xff, 0xff, 0x04, 	/*Color of index 1*/
  0xff, 0xff, 0xff, 0xb2, 	/*Color of index 2*/
  0xff, 0xff, 0xff, 0x54, 	/*Color of index 3*/
  0xff, 0xff, 0xff, 0xe4, 	/*Color of index 4*/
  0xff, 0xff, 0xff, 0x1d, 	/*Color of index 5*/
  0xff, 0xff, 0xff, 0x66, 	/*Color of index 6*/
  0xff, 0xff, 0xff, 0xfe, 	/*Color of index 7*/
  0xff, 0xff, 0xff, 0x90, 	/*Color of index 8*/
  0xff, 0xff, 0xff, 0xc7, 	/*Color of index 9*/
  0xff, 0xff, 0xff, 0x2c, 	/*Color of index 10*/
  0xff, 0xff, 0xff, 0xd6, 	/*Color of index 11*/
  0xff, 0xff, 0xff, 0x6c, 	/*Color of index 12*/
  0xff, 0xff, 0xff, 0xf1, 	/*Color of index 13*/
  0xff, 0xff, 0xff, 0x6f, 	/*Color of index 14*/
  0xff, 0xff, 0xff, 0x3c, 	/*Color of index 15*/

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa6, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc7, 0x72, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xa3, 0x10, 0x00, 0x00, 0x67, 0x77, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x0a, 0xbd, 0xe1, 0x00, 0x00, 0xc7, 0x77, 0x72, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xd7, 0xde, 0x10, 0x00, 0x67, 0x77, 0x77, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xed, 0x7d, 0xe1, 0x00, 0xc7, 0x78, 0x97, 0x72, 0x50, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1e, 0xd7, 0xde, 0x10, 0xf4, 0x76, 0xa9, 0x77, 0x25, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xed, 0x7d, 0xe1, 0x03, 0x4c, 0x08, 0x77, 0xd3, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1e, 0xd7, 0xde, 0x10, 0x33, 0xcd, 0x7d, 0xe1, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xed, 0x7d, 0xe1, 0x0a, 0xb7, 0xde, 0x10, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0xd7, 0xde, 0x10, 0x3b, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xed, 0x7d, 0xe1, 0x15, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x77, 0xde, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0xd7, 0x7d, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xed, 0x77, 0x77, 0xde, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0xd7, 0x77, 0x77, 0x7d, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xed, 0x7d, 0x97, 0x79, 0xd7, 0xde, 0x10, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1e, 0xd7, 0xd6, 0xe7, 0x7c, 0xcd, 0x7d, 0xe1, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xed, 0x7d, 0xe0, 0x67, 0x7c, 0x08, 0x77, 0xde, 0x10, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1e, 0xd7, 0xde, 0x10, 0xc7, 0x76, 0xa9, 0x77, 0x7d, 0xe1, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x5b, 0x7d, 0xe1, 0x00, 0x67, 0x78, 0x97, 0x7b, 0xd7, 0xde, 0x10, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x13, 0xbe, 0x10, 0x00, 0xc7, 0x77, 0x77, 0x25, 0xcd, 0x7d, 0x30, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xa1, 0x00, 0x00, 0x67, 0x77, 0x72, 0x50, 0x1e, 0xd9, 0xa0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc7, 0x77, 0x25, 0x00, 0x01, 0x3a, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x72, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc7, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa6, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t bluetooth_disabled = {
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 32,
  .header.h = 32,
  .data_size = 576,
  .data = bluetooth_disabled_map,
};
