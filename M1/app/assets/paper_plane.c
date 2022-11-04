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

#ifndef LV_ATTRIBUTE_IMG_PAPER_PLANE
#define LV_ATTRIBUTE_IMG_PAPER_PLANE
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_PAPER_PLANE uint8_t paper_plane_map[] = {
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 0*/
  0xff, 0xff, 0xff, 0x0a, 	/*Color of index 1*/
  0xff, 0xff, 0xff, 0x17, 	/*Color of index 2*/
  0xff, 0xff, 0xff, 0x31, 	/*Color of index 3*/
  0xff, 0xff, 0xff, 0x43, 	/*Color of index 4*/
  0xff, 0xff, 0xff, 0x53, 	/*Color of index 5*/
  0xff, 0xff, 0xff, 0x60, 	/*Color of index 6*/
  0xff, 0xff, 0xff, 0x70, 	/*Color of index 7*/
  0xff, 0xff, 0xff, 0x7b, 	/*Color of index 8*/
  0xff, 0xff, 0xff, 0x8d, 	/*Color of index 9*/
  0xff, 0xff, 0xff, 0x9f, 	/*Color of index 10*/
  0xff, 0xff, 0xff, 0xb8, 	/*Color of index 11*/
  0xff, 0xff, 0xff, 0xcb, 	/*Color of index 12*/
  0xff, 0xff, 0xff, 0xe4, 	/*Color of index 13*/
  0xff, 0xff, 0xff, 0xed, 	/*Color of index 14*/
  0xff, 0xff, 0xff, 0xfd, 	/*Color of index 15*/

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x8a, 
  0x00, 0x00, 0x00, 0x00, 0x02, 0x5a, 0xef, 0xf9, 
  0x00, 0x00, 0x00, 0x38, 0xcf, 0xfb, 0xcf, 0xf4, 
  0x00, 0x15, 0x9d, 0xfd, 0xa5, 0x17, 0xfd, 0xe1, 
  0x4b, 0xff, 0xc8, 0x30, 0x00, 0x7f, 0x7b, 0xa0, 
  0xec, 0x62, 0x00, 0x00, 0x07, 0xf7, 0x1f, 0x60, 
  0xbe, 0x30, 0x00, 0x00, 0x7f, 0x70, 0x5f, 0x20, 
  0x1b, 0xe3, 0x00, 0x07, 0xf8, 0x00, 0xac, 0x00, 
  0x01, 0xbe, 0x00, 0x7f, 0x70, 0x00, 0xd8, 0x00, 
  0x00, 0x5f, 0x07, 0xf8, 0x00, 0x03, 0xf3, 0x00, 
  0x00, 0x5f, 0x7f, 0x80, 0x00, 0x08, 0xd0, 0x00, 
  0x00, 0x5f, 0xf7, 0x00, 0x00, 0x0c, 0xa0, 0x00, 
  0x00, 0x6f, 0xff, 0xfe, 0x30, 0x2f, 0x50, 0x00, 
  0x00, 0x25, 0x55, 0x5b, 0xe3, 0x6f, 0x10, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xbe, 0xcb, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1b, 0xe4, 0x00, 0x00, 
};

const lv_img_dsc_t paper_plane = {
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 16,
  .header.h = 16,
  .data_size = 192,
  .data = paper_plane_map,
};