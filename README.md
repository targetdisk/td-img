# Target Disk Image

This repository contains a library and utilities for encoding and decoding
Target Disk Image (`.TDI`) files.

### Building the Utilities

#### Dependencies
- A sane libC
- A recent C compiler
- GNU Make

#### Steps
Build with make:
```
make all
```

### Target Disk Image RIFF File Format

Target Disk Image (`.TDI`) files are RIFF files that can contain `RLE` and
`RLE1` data.

#### RIFF Descriptor Chunk

|Endian|  Data Type  |     Name       |Size|
|------|-------------|----------------|----|
| big  |  `char *`   |    Chunk ID    | 4  |
|little|  `uint32_t` |   Chunk size   | 4  |
| big  |  `char *`   |     Format     | 4  |

#### "fmt" Chunk

|Endian|  Data Type  |     Name       |Size|
|------|-------------|----------------|----|
| big  |  `char *`   |    Chunk ID    | 4  |
|little|  `uint32_t` | Sub-chunk size | 4  |
|little|  `uint32_t` |     Width      | 4  |
|little|  `uint32_t` |     Height     | 4  |
|little|  `uint32_t` |Foreground Color| 4  |

#### "data" Chunk

|Endian|  Data Type  |     Name       |Size|
|------|-------------|----------------|----|
| big  |  `char *`   |    Chunk ID    | 4  |
|little|  `uint32_t` | Sub-chunk size | 4  |
|little| `uint32_t *`|     Data       | *  |

