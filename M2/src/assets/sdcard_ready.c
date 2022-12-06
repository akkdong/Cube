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

#ifndef LV_ATTRIBUTE_IMG_SDCARD_READY
#define LV_ATTRIBUTE_IMG_SDCARD_READY
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_SDCARD_READY uint8_t sdcard_ready_map[] = {
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 0*/
  0xff, 0xff, 0xff, 0x09, 	/*Color of index 1*/
  0xff, 0xff, 0xff, 0xc1, 	/*Color of index 2*/
  0xff, 0xff, 0xff, 0x75, 	/*Color of index 3*/
  0xff, 0xff, 0xff, 0xf3, 	/*Color of index 4*/
  0xff, 0xff, 0xff, 0x39, 	/*Color of index 5*/
  0xff, 0xff, 0xff, 0x9c, 	/*Color of index 6*/
  0xff, 0xff, 0xff, 0xe8, 	/*Color of index 7*/
  0xff, 0xff, 0xff, 0x5f, 	/*Color of index 8*/
  0xff, 0xff, 0xff, 0x24, 	/*Color of index 9*/
  0xff, 0xff, 0xff, 0x88, 	/*Color of index 10*/
  0xff, 0xff, 0xff, 0xd6, 	/*Color of index 11*/
  0xff, 0xff, 0xff, 0xb0, 	/*Color of index 12*/
  0xff, 0xff, 0xff, 0x4d, 	/*Color of index 13*/
  0xff, 0xff, 0xff, 0xfe, 	/*Color of index 14*/
  0xff, 0xff, 0xff, 0x16, 	/*Color of index 15*/

  0x00, 0x00, 0x00, 0x00, 0x16, 0x4e, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0x46, 0xf0, 0x00, 
  0x00, 0x00, 0x00, 0x09, 0xbe, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xb1, 0x00, 
  0x00, 0x00, 0x00, 0x97, 0xe7, 0x33, 0xb2, 0x33, 0xe6, 0x36, 0x43, 0x3b, 0x23, 0x34, 0xe8, 0x00, 
  0x00, 0x00, 0x09, 0x7e, 0xeb, 0x00, 0xca, 0x00, 0xe5, 0x0d, 0x70, 0x06, 0x60, 0x07, 0xea, 0x00, 
  0x00, 0x00, 0xfb, 0xee, 0xeb, 0x00, 0xca, 0x00, 0xe5, 0x0d, 0x70, 0x06, 0x60, 0x07, 0xea, 0x00, 
  0x00, 0x00, 0x2e, 0xee, 0xeb, 0x00, 0xca, 0x00, 0xe5, 0x0d, 0x70, 0x06, 0x60, 0x07, 0xea, 0x00, 
  0x00, 0x05, 0xee, 0xd9, 0x3b, 0x00, 0xca, 0x00, 0xe5, 0x0d, 0x70, 0x06, 0x60, 0x07, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0x90, 0x84, 0x66, 0x7b, 0x66, 0xec, 0x62, 0x46, 0x6b, 0xb6, 0x64, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0x90, 0x8e, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0x90, 0x8e, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0x90, 0x8e, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0x47, 0x4e, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x0d, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xea, 0x00, 
  0x00, 0x09, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xe8, 0x00, 
  0x00, 0x00, 0x6e, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xb1, 0x00, 
  0x00, 0x00, 0x1a, 0x7e, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0x46, 0xf0, 0x00, 
};

const lv_img_dsc_t sdcard_ready = {
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 32,
  .header.h = 32,
  .data_size = 576,
  .data = sdcard_ready_map,
};
