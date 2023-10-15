#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "codec.h"
#include "order.h"
#include "pixmap.h"

#define USAGE \
  "Usage: %s WIDTH HEIGHT COLOR OUTFILE\n\n" \
  "DESCRIPTION\n" \
  "    Take 32bpp RGBA data from stdin and convert it to 1-bit RLE.\n\n" \
  "REQUIRED PARAMETERS\n" \
  "    WIDTH  The width of the pixmap in pixels.\n" \
  "    HEIGHT  The height of the pixmap in pixels.\n" \
  "    COLOR  The color to render opaque output pixels.\n" \
  "    OUTFILE  The 1-bit RLE output file to write to.\n"

int main ( int argc, char *argv[] )
{
  pixmap_t in, out;

  char datatype;
  uint32_t file_size = 0;
  FILE *outfile = NULL;

  if ( argc < 5 )
  {
    fprintf(stderr, USAGE, argv[0]);
    return 1;
  }

  if ( *argv[1] == '\0' )
  {
    fprintf( stderr, "ERROR: $0 borked!\n" );

    return 1;
  }

  if ( isatty( fileno( stdin ) ) )
  {
    fprintf( stderr, "ERROR: stdin can't be a terminal!\n" );

    return 1;
  }

  outfile = fopen(argv[4], "w");
  if ( !outfile )
  {
    fprintf(stderr, "ERROR %d: %s while opening \"%s\" for writing!\n",
        errno, strerror( errno ), argv[4] );

    return 1;
  }

  fprintf( outfile, "RIFF%c%c%c%c", '\0', '\0', '\0', '\0' );

  switch ( *( argv[1] - 2 ) )
  {
    case '1':
      {
        in.datatype = RAW_STREAM;
        out.datatype = RLE1_STREAM;

        fprintf( outfile, "RLE1" );

        break;
      }

    case 'e':
      {
        in.datatype = RAW_STREAM;
        out.datatype = RLE_STREAM;

        fprintf( outfile, "RLE%c", '\0' );

        break;
      }

    case 'w':
      {
        /* TODO: add rgba2raw and raw2rgba */
        in.datatype = ( *( argv[1] - 6 ) == 'e' ) ? RLE_STREAM : RLE1_STREAM;
        out.datatype = RAW_STREAM;

        fprintf( outfile, "RGBA" );

        break;
      }

    default:
      fprintf(stderr, "ERROR: image data type not implemented!!\n");

      return EPROTONOSUPPORT;
  }

  file_size += 4; /* Count "RGBA" "RLE." "RLE1"  */

  {
    uint32_t chunk_sz = LE_UI32( sizeof(uint32_t) * 3 ),
             width = LE_UI32( (uint32_t)atol( argv[1] ) ),
             height = LE_UI32( (uint32_t)atol( argv[2] ) ),
             fgcolor = 0;

    if ( *(argv[3]++) == '0' && *(argv[3]++) == 'x' )
    {
      while ( *argv[3] )
      {
        fgcolor = ( fgcolor << 4 ) + DEHEX_CHAR( *argv[3] );
        argv[3]++;
      }
    }
    else
    {
      fgcolor = LE_UI32( (uint32_t)atol( argv[3] ) );
    }

    fprintf( outfile, "fmt%c", '\0' );
    fwrite( &chunk_sz, sizeof(uint32_t), 1, outfile );

    /* counted by chunk_sz */
    fwrite( &width, sizeof(uint32_t), 1, outfile );
    fwrite( &height, sizeof(uint32_t), 1, outfile );
    fwrite( &fgcolor, sizeof(uint32_t), 1, outfile );

    in.width = width;
    in.height = height;
    in.n_pixels = width * height;
    in.data = stdin;

    out.width = width;
    out.height = height;
    out.n_pixels = in.n_pixels;
    out.data = outfile;

    file_size += sizeof(uint32_t) * 2 + chunk_sz;
    file_size += encode( &in, &out );
  }

  fclose( outfile );

  return 0;
}
