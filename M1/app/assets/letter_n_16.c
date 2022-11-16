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

#ifndef LV_ATTRIBUTE_IMG_LETTER_N_16
#define LV_ATTRIBUTE_IMG_LETTER_N_16
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_LETTER_N_16 uint8_t letter_n_16_map[] = {
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0x28, 	/*Color of index 1*/
  0x00, 0x00, 0x00, 0x5c, 	/*Color of index 2*/
  0x00, 0x00, 0x00, 0x62, 	/*Color of index 3*/
  0x00, 0x00, 0x00, 0x6d, 	/*Color of index 4*/
  0x00, 0x00, 0x00, 0x71, 	/*Color of index 5*/
  0x00, 0x00, 0x00, 0x78, 	/*Color of index 6*/
  0x00, 0x00, 0x00, 0x8b, 	/*Color of index 7*/
  0x00, 0x00, 0x00, 0xaa, 	/*Color of index 8*/
  0x00, 0x00, 0x00, 0xb6, 	/*Color of index 9*/
  0x00, 0x00, 0x00, 0xba, 	/*Color of index 10*/
  0x00, 0x00, 0x00, 0xd4, 	/*Color of index 11*/
  0x00, 0x00, 0x00, 0xd9, 	/*Color of index 12*/
  0x00, 0x00, 0x00, 0xed, 	/*Color of index 13*/
  0x00, 0x00, 0x00, 0xf2, 	/*Color of index 14*/
  0x00, 0x00, 0x00, 0xfe, 	/*Color of index 15*/

  0xbb, 0xbb, 0xbb, 0x60, 0x00, 0x00, 0x6c, 0xbb, 
  0xff, 0xff, 0xff, 0xe5, 0x00, 0x00, 0x7f, 0xff, 
  0xff, 0xfc, 0xdf, 0xff, 0x50, 0x00, 0x7f, 0xff, 
  0xff, 0xf7, 0x2e, 0xff, 0xe5, 0x00, 0x7f, 0xff, 
  0xff, 0xf7, 0x02, 0xef, 0xfe, 0x40, 0x7f, 0xff, 
  0xff, 0xf7, 0x00, 0x3e, 0xff, 0xa0, 0x7f, 0xff, 
  0xff, 0xf7, 0x01, 0x02, 0xef, 0x90, 0x7f, 0xff, 
  0xff, 0xf7, 0x08, 0x30, 0x2f, 0x90, 0x7f, 0xff, 
  0xff, 0xf7, 0x0a, 0xe3, 0x03, 0x80, 0x7f, 0xff, 
  0xff, 0xf7, 0x0a, 0xfe, 0x20, 0x10, 0x7f, 0xff, 
  0xff, 0xf7, 0x0a, 0xff, 0xe3, 0x00, 0x7f, 0xff, 
  0xff, 0xf7, 0x03, 0xff, 0xfe, 0x20, 0x7f, 0xff, 
  0xff, 0xf7, 0x00, 0x4e, 0xff, 0xe2, 0x7f, 0xff, 
  0xff, 0xf7, 0x00, 0x05, 0xff, 0xfd, 0xcf, 0xff, 
  0xff, 0xf7, 0x00, 0x00, 0x5e, 0xff, 0xff, 0xff, 
  0xbb, 0xc6, 0x00, 0x00, 0x06, 0xcb, 0xbb, 0xbb, 
};

const lv_img_dsc_t letter_n_16 = {
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 16,
  .header.h = 16,
  .data_size = 192,
  .data = letter_n_16_map,
};
