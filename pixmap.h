#ifndef PIXMAP_H
#define PIXMAP_H

#ifndef _UEFI_H_
# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
#endif

enum imgdata_type
{
  RAW, RLE, RLE1, /* Core formats */
  RAW_BL, RLE_BL, RLE1_BL, /* Composited formats */
  RAW_STREAM, RLE_STREAM, RLE1_STREAM /* File I/O formats */
};

typedef
struct pixmap_s
{
  uint32_t width;
  uint32_t height;
  size_t n_pixels;
  char datatype;
  void *data; /* Either (uint32_t *) or (FILE *) */
} pixmap_t;

typedef
struct draw_s
{
  pixmap_t *p;
  uint32_t x;
  uint32_t y;
  uint32_t xmax;
  uint32_t ymax;
} draw_t;

#endif /* PIXMAP_H */
