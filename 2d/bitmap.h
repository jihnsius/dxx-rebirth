#ifndef _BITMAP_H
#define _BITMAP_H

#ifdef __cplusplus

typedef ubyte palette_array_t[256*3];

void build_colormap_good( palette_array_t &palette, ubyte * colormap, int * freq );
void decode_data(ubyte *data, int num_pixels, ubyte * colormap, int * count );

#endif

#endif
