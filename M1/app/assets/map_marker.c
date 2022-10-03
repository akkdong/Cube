#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_MAP_MARKER
#define LV_ATTRIBUTE_IMG_MAP_MARKER
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_MAP_MARKER uint8_t map_marker_map[] = {
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 0*/
  0xff, 0xff, 0xff, 0x0d, 	/*Color of index 1*/
  0xff, 0xff, 0xff, 0x22, 	/*Color of index 2*/
  0xff, 0xff, 0xff, 0x2a, 	/*Color of index 3*/
  0xff, 0xff, 0xff, 0x3e, 	/*Color of index 4*/
  0xff, 0xff, 0xff, 0x54, 	/*Color of index 5*/
  0xff, 0xff, 0xff, 0x63, 	/*Color of index 6*/
  0xff, 0xff, 0xff, 0x7e, 	/*Color of index 7*/
  0xff, 0xff, 0xff, 0x8d, 	/*Color of index 8*/
  0xff, 0xff, 0xff, 0x9b, 	/*Color of index 9*/
  0xff, 0xff, 0xff, 0xa8, 	/*Color of index 10*/
  0xff, 0xff, 0xff, 0xbe, 	/*Color of index 11*/
  0xff, 0xff, 0xff, 0xca, 	/*Color of index 12*/
  0xff, 0xff, 0xff, 0xe4, 	/*Color of index 13*/
  0xff, 0xff, 0xff, 0xf4, 	/*Color of index 14*/
  0xff, 0xff, 0xff, 0xfd, 	/*Color of index 15*/

  0x00, 0x00, 0x16, 0xce, 0xec, 0x60, 0x00, 0x00, 
  0x00, 0x01, 0xbf, 0x95, 0x6a, 0xfb, 0x10, 0x00, 
  0x00, 0x09, 0xe4, 0x14, 0x40, 0x5f, 0x80, 0x00, 
  0x00, 0x1f, 0x71, 0xcf, 0xfc, 0x18, 0xe1, 0x00, 
  0x00, 0x4f, 0x37, 0xd3, 0x3e, 0x72, 0xf4, 0x00, 
  0x00, 0x5f, 0x19, 0xb0, 0x0b, 0x90, 0xf5, 0x00, 
  0x4d, 0xef, 0x45, 0xf7, 0x7f, 0x63, 0xf6, 0x00, 
  0xda, 0x7d, 0xa0, 0x7d, 0xe7, 0x0a, 0xff, 0xe5, 
  0xf5, 0x04, 0xf7, 0x00, 0x00, 0x7f, 0x42, 0xad, 
  0xf5, 0x00, 0x6f, 0x80, 0x08, 0xf6, 0x00, 0x5f, 
  0xf5, 0x00, 0x06, 0xf8, 0x8e, 0x60, 0x00, 0x5f, 
  0xf5, 0x00, 0x00, 0x5f, 0xf6, 0x00, 0x00, 0x5f, 
  0xf5, 0x00, 0x00, 0x05, 0x50, 0x00, 0x00, 0x5f, 
  0xf9, 0x30, 0x00, 0x00, 0x26, 0x97, 0x41, 0x5f, 
  0xad, 0xfd, 0x96, 0xad, 0xfe, 0xbd, 0xfe, 0xdf, 
  0x00, 0x47, 0xce, 0xb7, 0x40, 0x00, 0x26, 0x9d, 
};

const lv_img_dsc_t map_marker = {
  .header.cf = LV_IMG_CF_INDEXED_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 16,
  .header.h = 16,
  .data_size = 192,
  .data = map_marker_map,
};
