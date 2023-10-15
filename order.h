#ifndef ORDER_H
#define ORDER_H

#define DEHEX_CHAR(c)  ( c < ':' ? c - '0' : c - ( 'a' - 10 ) )

#define FLIP_UI32(n)  (           \
    ((n & 0x000000FF) << 24) |    \
    ((n & 0x0000FF00) <<  8) |    \
    ((n & 0x00FF0000) >>  8) |    \
    ((n & 0xFF000000) >> 24))

#define FLIP_UI16(n)  (  \
    ((n & 0xFF00) >> 8) | ((n & 0xFF) << 8))

#if __BYTE_ORDER__ == BIG_ENDIAN
#  define LE_UI32(n)  (FLIP_UI32(n))
#  define BE_UI32(n)  (n)
#  define LE_UI16(n)  (FLIP_UI16(n))
#  define BE_UI16(n)  (n)
#elif __BYTE_ORDER__ == LITTLE_ENDIAN
#  define LE_UI32(n)  (n)
#  define BE_UI32(n)  (FLIP_UI32(n))
#  define LE_UI16(n)  (n)
#  define BE_UI16(n)  (FLIP_UI16(n))
#else
#  error "Unsupported C compiler, sorry!"
#endif

#endif /* ORDER_H */
