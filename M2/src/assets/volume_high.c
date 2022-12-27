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

#ifndef LV_ATTRIBUTE_IMG_VOLUME_HIGH
#define LV_ATTRIBUTE_IMG_VOLUME_HIGH
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_VOLUME_HIGH uint8_t volume_high_map[] = {
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 0*/
  0xff, 0xff, 0xff, 0x03, 	/*Color of index 1*/
  0xff, 0xff, 0xff, 0x8f, 	/*Color of index 2*/
  0xff, 0xff, 0xff, 0x44, 	/*Color of index 3*/
  0xff, 0xff, 0xff, 0xd2, 	/*Color of index 4*/
  0xff, 0xff, 0xff, 0xf0, 	/*Color of index 5*/
  0xff, 0xff, 0xff, 0x1e, 	/*Color of index 6*/
  0xff, 0xff, 0xff, 0x69, 	/*Color of index 7*/
  0xff, 0xff, 0xff, 0xa1, 	/*Color of index 8*/
  0xff, 0xff, 0xff, 0x33, 	/*Color of index 9*/
  0xff, 0xff, 0xff, 0xe2, 	/*Color of index 10*/
  0xff, 0xff, 0xff, 0x7a, 	/*Color of index 11*/
  0xff, 0xff, 0xff, 0x56, 	/*Color of index 12*/
  0xff, 0xff, 0xff, 0x0d, 	/*Color of index 13*/
  0xff, 0xff, 0xff, 0xfd, 	/*Color of index 14*/
  0xff, 0xff, 0xff, 0xb5, 	/*Color of index 15*/

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3f, 0xf1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdb, 0x5e, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xfe, 0xee, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x7a, 0xe4, 0x7a, 0xa6, 0x00, 0x00, 0x06, 0x76, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0xe5, 0x86, 0x6a, 0xa6, 0x00, 0x00, 0x0b, 0xef, 0x60, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x4e, 0xa7, 0xd0, 0x6a, 0xa6, 0x00, 0x00, 0x09, 0x4e, 0x21, 0x00, 
  0x00, 0x16, 0x66, 0x66, 0x25, 0xef, 0x90, 0x00, 0x6a, 0xa6, 0x00, 0xdb, 0x30, 0x35, 0x5c, 0x00, 
  0x06, 0x24, 0xaa, 0xa5, 0xe5, 0xbd, 0x00, 0x00, 0x6a, 0xa6, 0x00, 0x95, 0xa3, 0x0b, 0xef, 0xd0, 
  0x12, 0xe5, 0xaa, 0xae, 0x53, 0x10, 0x00, 0x00, 0x6a, 0xa6, 0x00, 0xd8, 0xef, 0xd6, 0xa5, 0x30, 
  0x64, 0x5c, 0x66, 0x3a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xa6, 0xd3, 0x16, 0x4e, 0xc0, 0x2e, 0xb0, 
  0x6a, 0xa6, 0x00, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xa6, 0xb5, 0xc0, 0xbe, 0x80, 0xce, 0xf1, 
  0x6a, 0xa6, 0x00, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xa6, 0xbe, 0xf1, 0x95, 0x4d, 0x95, 0x46, 
  0x6a, 0xa6, 0x00, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xa6, 0x65, 0xa6, 0x6a, 0xa6, 0x6a, 0xa6, 
  0x6a, 0xa6, 0x00, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xa6, 0x65, 0x46, 0x6a, 0xa6, 0x6a, 0xa6, 
  0x6a, 0xa6, 0x00, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xa6, 0xbe, 0xf1, 0x95, 0x4d, 0x95, 0x46, 
  0x6a, 0xa6, 0x00, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0x52, 0xae, 0xc0, 0x7e, 0x80, 0xce, 0xf1, 
  0x64, 0x5c, 0x66, 0x95, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xee, 0xe2, 0x16, 0x4e, 0xc0, 0x2e, 0xb0, 
  0x12, 0xe5, 0xaa, 0xae, 0x5c, 0x10, 0x00, 0x00, 0x6a, 0xef, 0xcd, 0x12, 0xef, 0xd6, 0xa5, 0x30, 
  0x06, 0x24, 0xaa, 0xaa, 0xea, 0xbd, 0x00, 0x00, 0x6a, 0xa6, 0x00, 0x35, 0x53, 0x02, 0xef, 0xd0, 
  0x00, 0x16, 0x66, 0x69, 0x25, 0xef, 0x90, 0x00, 0x6a, 0xa6, 0x00, 0x62, 0x30, 0x3a, 0x53, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x4e, 0xa7, 0xd0, 0x6a, 0xa6, 0x00, 0x00, 0x09, 0x4e, 0x2d, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x5e, 0x86, 0x6a, 0xa6, 0x00, 0x00, 0x0b, 0xef, 0x60, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x7a, 0xe4, 0x75, 0xa6, 0x00, 0x00, 0x06, 0x76, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xfe, 0xee, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdb, 0xae, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3f, 0x8d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t volume_high = {
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 32,
  .header.h = 32,
  .data_size = 576,
  .data = volume_high_map,
};