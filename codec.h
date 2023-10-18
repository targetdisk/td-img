#ifndef CODEC_H
#define CODEC_H

#include "pixmap.h"

#define RGBA2ARGB_32( pixel )  ( ( pixel << 24 ) + ( pixel >> 8 ) )

#ifndef RLE1_ONLY
void rle_decompress( pixmap_t *pixmap );
#endif /* RLE1_ONLY */

void rle1_decompress( pixmap_t *pixmap );
void decompress( pixmap_t *pixmap );

uint32_t rle1_encode( pixmap_t *in, pixmap_t *out );

uint32_t encode( pixmap_t *in, pixmap_t *out );
uint32_t decode( pixmap_t *in, pixmap_t *out ); /* TODO */

#endif /* CODEC_H */
