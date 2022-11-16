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

#ifndef LV_ATTRIBUTE_IMG_LETTER_N
#define LV_ATTRIBUTE_IMG_LETTER_N
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_LETTER_N uint8_t letter_n_map[] = {
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0x09, 	/*Color of index 1*/
  0x00, 0x00, 0x00, 0x22, 	/*Color of index 2*/
  0x00, 0x00, 0x00, 0x29, 	/*Color of index 3*/
  0x00, 0x00, 0x00, 0x44, 	/*Color of index 4*/
  0x00, 0x00, 0x00, 0x5c, 	/*Color of index 5*/
  0x00, 0x00, 0x00, 0x63, 	/*Color of index 6*/
  0x00, 0x00, 0x00, 0x6d, 	/*Color of index 7*/
  0x00, 0x00, 0x00, 0x79, 	/*Color of index 8*/
  0x00, 0x00, 0x00, 0xa7, 	/*Color of index 9*/
  0x00, 0x00, 0x00, 0xac, 	/*Color of index 10*/
  0x00, 0x00, 0x00, 0xb1, 	/*Color of index 11*/
  0x00, 0x00, 0x00, 0xbc, 	/*Color of index 12*/
  0x00, 0x00, 0x00, 0xc2, 	/*Color of index 13*/
  0x00, 0x00, 0x00, 0xed, 	/*Color of index 14*/
  0x00, 0x00, 0x00, 0xfe, 	/*Color of index 15*/

  0xcc, 0xcc, 0xcc, 0xcc, 0xd8, 0x00, 0x00, 0x00, 0x00, 0x4d, 0xdd, 0xdd, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0x60, 0x00, 0x00, 0x00, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf6, 0x00, 0x00, 0x00, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0x60, 0x00, 0x00, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xf7, 0xaf, 0xff, 0xff, 0xf6, 0x00, 0x00, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x1b, 0xff, 0xff, 0xff, 0x60, 0x00, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xf6, 0x01, 0xbf, 0xff, 0xff, 0xf6, 0x00, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xf6, 0x00, 0x1b, 0xff, 0xff, 0xff, 0x20, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x00, 0x01, 0xbf, 0xff, 0xff, 0x30, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xf6, 0x01, 0x10, 0x1b, 0xff, 0xff, 0x30, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x03, 0x91, 0x01, 0xaf, 0xff, 0x30, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xf6, 0x03, 0xfa, 0x10, 0x1a, 0xff, 0x30, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x03, 0xff, 0xa1, 0x01, 0xaf, 0x30, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x03, 0xff, 0xfb, 0x10, 0x19, 0x30, 0x6f, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x03, 0xff, 0xff, 0xb1, 0x01, 0x10, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x03, 0xff, 0xff, 0xfb, 0x10, 0x00, 0x6f, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x02, 0xff, 0xff, 0xff, 0xb1, 0x00, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x00, 0x6f, 0xff, 0xff, 0xfb, 0x10, 0x6f, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x00, 0x06, 0xff, 0xff, 0xff, 0xb1, 0x5f, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x00, 0x00, 0x6f, 0xff, 0xff, 0xfa, 0x7f, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x00, 0x00, 0x06, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x00, 0x00, 0x00, 0x6f, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xf5, 0x00, 0x00, 0x00, 0x06, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xdd, 0xdd, 0xd4, 0x00, 0x00, 0x00, 0x00, 0x8d, 0xdd, 0xdd, 0xdd, 0xdd, 
};

const lv_img_dsc_t letter_n = {
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 24,
  .header.h = 24,
  .data_size = 352,
  .data = letter_n_map,
};
