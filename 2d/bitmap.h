#ifndef _BITMAP_H
#define _BITMAP_H

#ifdef __cplusplus

struct palette_array_t;

void build_colormap_good( palette_array_t &palette, ubyte * colormap, int * freq );
void decode_data(ubyte *data, int num_pixels, ubyte * colormap, int * count );

#endif

#endif
