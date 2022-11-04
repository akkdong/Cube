#if 0
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

#ifndef LV_ATTRIBUTE_IMG_VOLUME
#define LV_ATTRIBUTE_IMG_VOLUME
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_VOLUME uint8_t volume_map[] = {
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 0*/
  0xff, 0xff, 0xff, 0x07, 	/*Color of index 1*/
  0xff, 0xff, 0xff, 0x16, 	/*Color of index 2*/
  0xff, 0xff, 0xff, 0x27, 	/*Color of index 3*/
  0xff, 0xff, 0xff, 0x39, 	/*Color of index 4*/
  0xff, 0xff, 0xff, 0x47, 	/*Color of index 5*/
  0xff, 0xff, 0xff, 0x57, 	/*Color of index 6*/
  0xff, 0xff, 0xff, 0x6f, 	/*Color of index 7*/
  0xff, 0xff, 0xff, 0x91, 	/*Color of index 8*/
  0xff, 0xff, 0xff, 0xa8, 	/*Color of index 9*/
  0xff, 0xff, 0xff, 0xbe, 	/*Color of index 10*/
  0xff, 0xff, 0xff, 0xcc, 	/*Color of index 11*/
  0xff, 0xff, 0xff, 0xdd, 	/*Color of index 12*/
  0xff, 0xff, 0xff, 0xed, 	/*Color of index 13*/
  0xff, 0xff, 0xff, 0xf5, 	/*Color of index 14*/
  0xff, 0xff, 0xff, 0xfe, 	/*Color of index 15*/

  0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xaf, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x3c, 0xff, 0x09, 0xeb, 0x71, 0x00, 
  0x00, 0x05, 0xdb, 0x7f, 0x04, 0x68, 0xdb, 0x30, 
  0x9f, 0xff, 0x91, 0x6f, 0x04, 0x41, 0x2a, 0xb1, 
  0xe8, 0x65, 0x00, 0x6f, 0x09, 0xfc, 0x42, 0xd7, 
  0xf6, 0x00, 0x00, 0x6f, 0x00, 0x29, 0xc1, 0x8b, 
  0xf6, 0x00, 0x00, 0x6f, 0x00, 0x02, 0xf4, 0x6e, 
  0xf6, 0x00, 0x00, 0x6f, 0x00, 0x02, 0xf5, 0x6e, 
  0xf6, 0x00, 0x00, 0x6f, 0x00, 0x29, 0xc1, 0x8b, 
  0xf8, 0x65, 0x00, 0x6f, 0x09, 0xfc, 0x42, 0xd7, 
  0x8f, 0xff, 0x91, 0x6f, 0x04, 0x41, 0x2a, 0xb1, 
  0x00, 0x05, 0xdb, 0x7f, 0x04, 0x68, 0xdb, 0x20, 
  0x00, 0x00, 0x3c, 0xff, 0x09, 0xeb, 0x71, 0x00, 
  0x00, 0x00, 0x11, 0xaf, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t volume = {
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 16,
  .header.h = 16,
  .data_size = 192,
  .data = volume_map,
};