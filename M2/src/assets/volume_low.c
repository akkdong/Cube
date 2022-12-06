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

#ifndef LV_ATTRIBUTE_IMG_VOLUME_LOW
#define LV_ATTRIBUTE_IMG_VOLUME_LOW
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_VOLUME_LOW uint8_t volume_low_map[] = {
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 0*/
  0xff, 0xff, 0xff, 0x03, 	/*Color of index 1*/
  0xff, 0xff, 0xff, 0x8b, 	/*Color of index 2*/
  0xff, 0xff, 0xff, 0x46, 	/*Color of index 3*/
  0xff, 0xff, 0xff, 0xd2, 	/*Color of index 4*/
  0xff, 0xff, 0xff, 0xf1, 	/*Color of index 5*/
  0xff, 0xff, 0xff, 0x1e, 	/*Color of index 6*/
  0xff, 0xff, 0xff, 0x68, 	/*Color of index 7*/
  0xff, 0xff, 0xff, 0xa0, 	/*Color of index 8*/
  0xff, 0xff, 0xff, 0x34, 	/*Color of index 9*/
  0xff, 0xff, 0xff, 0xe3, 	/*Color of index 10*/
  0xff, 0xff, 0xff, 0x0e, 	/*Color of index 11*/
  0xff, 0xff, 0xff, 0xb7, 	/*Color of index 12*/
  0xff, 0xff, 0xff, 0x57, 	/*Color of index 13*/
  0xff, 0xff, 0xff, 0xfc, 	/*Color of index 14*/
  0xff, 0xff, 0xff, 0x76, 	/*Color of index 15*/

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3c, 0x8b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbf, 0x5e, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xce, 0xee, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x7a, 0xe4, 0x7a, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0xe5, 0x86, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1d, 0x4e, 0xa7, 0xb0, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x16, 0x66, 0x66, 0x25, 0xec, 0x90, 0x00, 0x6a, 0xa6, 0x00, 0xbf, 0x30, 0x00, 0x00, 0x00, 
  0x06, 0x24, 0xaa, 0xa5, 0xe5, 0xfb, 0x00, 0x00, 0x6a, 0xa6, 0x00, 0x95, 0xa3, 0x00, 0x00, 0x00, 
  0x12, 0xe5, 0xaa, 0xae, 0x53, 0x10, 0x00, 0x00, 0x6a, 0xa6, 0x00, 0xb8, 0xec, 0xb0, 0x00, 0x00, 
  0x64, 0x5d, 0x66, 0x9a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xa6, 0xb3, 0x16, 0x4e, 0xd0, 0x00, 0x00, 
  0x6a, 0xa6, 0x00, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xa6, 0xf5, 0xd0, 0xfe, 0x81, 0x00, 0x00, 
  0x6a, 0xa6, 0x00, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xa6, 0xfe, 0xc1, 0x95, 0x4b, 0x00, 0x00, 
  0x6a, 0xa6, 0x00, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xa6, 0x65, 0x46, 0x6a, 0xa6, 0x00, 0x00, 
  0x6a, 0xa6, 0x00, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xa6, 0x9a, 0xa6, 0x6a, 0xa6, 0x00, 0x00, 
  0x6a, 0xa6, 0x00, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xa6, 0x7e, 0xc1, 0x95, 0x4b, 0x00, 0x00, 
  0x6a, 0xa6, 0x00, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0x52, 0xae, 0xd0, 0xfe, 0x81, 0x00, 0x00, 
  0x64, 0x5d, 0x66, 0x9a, 0xa6, 0x00, 0x00, 0x00, 0x6a, 0xee, 0xe2, 0x16, 0xce, 0xd0, 0x00, 0x00, 
  0x12, 0xe5, 0xaa, 0xae, 0x5d, 0x10, 0x00, 0x00, 0x6a, 0xec, 0xdb, 0x12, 0xe4, 0xb0, 0x00, 0x00, 
  0x06, 0x24, 0xaa, 0xaa, 0xea, 0xfb, 0x00, 0x00, 0x6a, 0xa6, 0x00, 0x35, 0xa3, 0x00, 0x00, 0x00, 
  0x00, 0x16, 0x66, 0x69, 0x25, 0xec, 0x90, 0x00, 0x6a, 0xa6, 0x00, 0x62, 0xd1, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1d, 0x4e, 0xa7, 0xb0, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x5e, 0x86, 0x6a, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x7a, 0xe4, 0x7a, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xce, 0xee, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbf, 0xae, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3c, 0x8b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t volume_low = {
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 32,
  .header.h = 32,
  .data_size = 576,
  .data = volume_low_map,
};
