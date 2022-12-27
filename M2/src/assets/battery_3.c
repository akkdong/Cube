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

#ifndef LV_ATTRIBUTE_IMG_BATTERY_3
#define LV_ATTRIBUTE_IMG_BATTERY_3
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_BATTERY_3 uint8_t battery_3_map[] = {
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 0*/
  0xff, 0xff, 0xff, 0x08, 	/*Color of index 1*/
  0xff, 0xff, 0xff, 0xa0, 	/*Color of index 2*/
  0xff, 0xff, 0xff, 0x50, 	/*Color of index 3*/
  0xff, 0xff, 0xff, 0xeb, 	/*Color of index 4*/
  0xff, 0xff, 0xff, 0x28, 	/*Color of index 5*/
  0xff, 0xff, 0xff, 0x77, 	/*Color of index 6*/
  0xff, 0xff, 0xff, 0xc5, 	/*Color of index 7*/
  0xff, 0xff, 0xff, 0x17, 	/*Color of index 8*/
  0xff, 0xff, 0xff, 0xd9, 	/*Color of index 9*/
  0xff, 0xff, 0xff, 0xfe, 	/*Color of index 10*/
  0xff, 0xff, 0xff, 0xba, 	/*Color of index 11*/
  0xff, 0xff, 0xff, 0x37, 	/*Color of index 12*/
  0xff, 0xff, 0xff, 0x88, 	/*Color of index 13*/
  0xff, 0xff, 0xff, 0x64, 	/*Color of index 14*/
  0xff, 0xff, 0xff, 0x20, 	/*Color of index 15*/

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x0f, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xc1, 0x00, 0x00, 
  0x6a, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xa4, 0xf0, 0x00, 
  0xa6, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0xf9, 0x60, 0x00, 
  0xa3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xd0, 0x00, 
  0xa3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xd0, 0x00, 
  0xa3, 0x00, 0xe9, 0x99, 0x1f, 0x99, 0x93, 0x0b, 0x99, 0xd0, 0x00, 0x00, 0x00, 0x07, 0x7d, 0x61, 
  0xa3, 0x00, 0x6a, 0xaa, 0x15, 0xaa, 0xa3, 0x09, 0xaa, 0x20, 0x00, 0x00, 0x00, 0x02, 0x77, 0x4b, 
  0xa3, 0x00, 0x6a, 0xaa, 0x15, 0xaa, 0xa3, 0x09, 0xaa, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 
  0xa3, 0x00, 0x6a, 0xaa, 0x15, 0xaa, 0xa3, 0x09, 0xaa, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 
  0xa3, 0x00, 0x6a, 0xaa, 0x15, 0xaa, 0xa3, 0x09, 0xaa, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 
  0xa3, 0x00, 0x6a, 0xaa, 0x15, 0xaa, 0xa3, 0x09, 0xaa, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 
  0xa3, 0x00, 0x6a, 0xaa, 0x15, 0xaa, 0xa3, 0x09, 0xaa, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 
  0xa3, 0x00, 0x6a, 0xaa, 0x15, 0xaa, 0xa3, 0x09, 0xaa, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 
  0xa3, 0x00, 0x6a, 0xaa, 0x15, 0xaa, 0xa3, 0x09, 0xaa, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 
  0xa3, 0x00, 0x6a, 0xaa, 0x15, 0xaa, 0xa3, 0x09, 0xaa, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 
  0xa3, 0x00, 0x6a, 0xaa, 0x15, 0xaa, 0xa3, 0x09, 0xaa, 0x20, 0x00, 0x00, 0x00, 0x02, 0x77, 0x4b, 
  0xa3, 0x00, 0xe9, 0x99, 0x1f, 0x99, 0x93, 0x0b, 0x99, 0xd0, 0x00, 0x00, 0x00, 0x07, 0x7d, 0x61, 
  0xa3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xd0, 0x00, 
  0xa3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xd0, 0x00, 
  0xa6, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x84, 0x60, 0x00, 
  0x6a, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xa4, 0xf0, 0x00, 
  0x0f, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xc1, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t battery_3 = {
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 32,
  .header.h = 32,
  .data_size = 576,
  .data = battery_3_map,
};