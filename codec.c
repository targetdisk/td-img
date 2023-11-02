#include <errno.h>

#ifndef _UEFI_H_
# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
#endif

#include "order.h"
#include "pixmap.h"

#include "codec.h"

#ifndef RLE1_ONLY
void rle_decompress( pixmap_t *pixmap )
{
  uint32_t *rledata = pixmap->data;
  uint32_t *rawdata = malloc(sizeof(uint32_t) * pixmap->n_pixels);

  if ( rawdata )
  {
    pixmap->data = rawdata;
    uint32_t *pixel;

    for ( size_t pi = 0; pi < pixmap->n_pixels; rledata++ )
    {
      pixel = rledata++;

      for ( uint32_t pi_end = pi + *rledata; pi < pi_end; pi++ )
      {
        *rawdata++ = RGBA2ARGB_32( *pixel );
      }
    }
  }
  else
  {
    fprintf( stderr, "ERROR: malloc failed on pixmap data!!\n" );

    exit( ENOMEM );
  }

  pixmap->datatype = ( pixmap->datatype == RLE ) ? RAW : RAW_BL;
}
#endif /* RLE1_ONLY */

static inline
uint32_t *rle1_put_pixels( uint32_t pixel, size_t reps, uint32_t *rawdata )
{
  for ( size_t n = 0; n < reps; n++ )
    *rawdata++ = pixel;

  return rawdata;
}

void rle1_decompress( pixmap_t *pixmap )
{
  uint32_t *rledata = pixmap->data;
  uint32_t fgcolor = RGBA2ARGB_32( *rledata );
  uint32_t *rawdata = malloc( sizeof( uint32_t ) * pixmap->n_pixels );

  if ( rawdata )
  {
    unsigned char byte;
    size_t reps;
    unsigned char packlocat = 24;

    rledata++; /* Increment past fg color. */
    pixmap->data = rawdata;

    for ( size_t pi = 0; pi < pixmap->n_pixels; )
    {
      byte = (unsigned char)( *rledata >> packlocat );
      reps = (size_t)( byte & 127 );

      pi += reps;

      rawdata = rle1_put_pixels( ( byte >> 7 ? fgcolor : 0 ), reps, rawdata );

      if ( !packlocat )
      {
        packlocat = 24;

        rledata++;
      }
      else
      {
        packlocat -= 8;
      }
    }
  }
  else
  {
    fprintf( stderr, "ERROR: malloc failed on pixmap data!!\n" );

    exit( ENOMEM );
  }

  pixmap->datatype = ( pixmap->datatype == RLE1 ) ? RAW : RAW_BL;
}

void rgba2argb_32( pixmap_t *pixmap )
{
  size_t p = pixmap->n_pixels;
  uint32_t *d = pixmap->data;

  while ( p ) {
    *d = ( *d << 24 ) + ( *d >> 8 );
    p--;
    d++;
  }
}

void decompress( pixmap_t *pixmap )
{
  switch ( pixmap->datatype )
  {
    case RAW:
    case RAW_BL:
      rgba2argb_32( pixmap );

      return;

#ifndef RLE1_ONLY
    case RLE:
    case RLE_BL:
      rle_decompress( pixmap );

      return;
#endif /* RLE1_ONLY */

    case RLE1:
    case RLE1_BL:
      rle1_decompress( pixmap );

      return;
    default:
      fprintf( stderr, "ERROR: image data type not implemented!!\n" );

      exit( EPROTONOSUPPORT );
  }
}

static inline
void pack( char byte, uint32_t *packed, char *n_packed, void (*putfunc)( uint32_t, void *, uint32_t * ),
           void *stream, uint32_t *chk_sz )
{
  *packed = *packed << 8;
  *packed += byte;

  *n_packed += 1;

  if ( *n_packed == 4 )
  {
    (*putfunc)( *packed, stream, chk_sz );

    *packed = 0;
    *n_packed = 0;
  }
}

#ifndef _UEFI_H_
static
void put_stream( uint32_t number, void *outfile, uint32_t *chk_sz )
{
  fwrite( &number, sizeof( uint32_t ), 1, (FILE *)outfile );

  *chk_sz += sizeof( uint32_t );
}

static
void put_pixmap( uint32_t number, void *outfile, uint32_t *chk_sz )
{
  (void)number;
  (void)outfile;
  (void)chk_sz;

  /* TODO */
}

uint32_t rle1_encode( pixmap_t *in, pixmap_t *out )
{
  void (*putfunc)( uint32_t, void *, uint32_t * );
  uint32_t chunk_sz = 0;

  switch ( out->datatype )
  {
    case RLE1:
    case RLE1_BL:
      putfunc = &put_pixmap;

      break;
    case RLE1_STREAM:
      putfunc = &put_stream;
      fprintf( out->data, "data%c%c%c%c", '\0','\0','\0','\0' );

      break;
    default:
      fprintf( stderr, "ERROR: image data type not implemented!!\n" );

      exit( EPROTONOSUPPORT );
  }

  {
    uint32_t packed = 0;
    char pixel, *last_pixel = NULL, count = 0, n_packed = 0;

#   ifdef VISUAL_RLE1_DBG
      size_t pixel_no = 0, y = 0;
#   endif /* VISUAL_RLE1_DBG */

    for ( int p;; )
    {
      fseek( in->data, 3, SEEK_CUR );

      p = fgetc( in->data );

      if ( p == EOF )
      {
        puts("\nEOF");

        break;
      }

      pixel = p & 1;

#     ifdef VISUAL_RLE1_DBG
        pixel_no++;

        if ( ! ( pixel_no % 21 ) && ! ( y % 21 ) )
          printf( "%c", ( pixel ? '#' : ' ' ) );

        if ( ! ( pixel_no % in->height ) )
        {
          y++;

          if ( ! ( y % 21 ) )
            putc( '\n', stdout );
        }
#     endif /* VISUAL_RLE1_DBG */

      if ( last_pixel )
      {
        if ( pixel != *last_pixel )
        {
          pack( ( *last_pixel << 7 ) + count, &packed, &n_packed, putfunc, out->data, &chunk_sz );

          count = 1;
          *last_pixel = pixel;
        }
        else if ( pixel == *last_pixel )
        {
          count++;

          if ( count == 127 )
          {
            pack( ( *last_pixel << 7 ) + count, &packed, &n_packed, putfunc, out->data, &chunk_sz );

            count = 0;

            free( last_pixel );

            last_pixel = NULL;
          }
        }
      }
      else
      {
        last_pixel = malloc( 1 );
        *last_pixel = pixel;

        count = 1;
      }
    }

    pack( ( *last_pixel << 7 ) + count, &packed, &n_packed, putfunc, out->data, &chunk_sz );

    while ( n_packed )
      pack( 0, &packed, &n_packed, putfunc, out->data, &chunk_sz );
  }

  if ( out->datatype == RLE1_STREAM )
  {
    fseek( out->data, 0x24, SEEK_SET );
    fwrite( &chunk_sz, sizeof( uint32_t ), 1, out->data );
  }

  return ( chunk_sz + ( 2 * sizeof( uint32_t ) ) );
}

uint32_t encode( pixmap_t *in, pixmap_t *out )
{
  switch ( ( out->datatype + 1 ) % 3 )
  {
    case 1: /* TODO: RAW */
      return 0;

    case 2: /* TODO: RLE */
      return 0;

    default:
      return rle1_encode( in, out );
  }
}
#endif  /* _UEFI_H_ */
