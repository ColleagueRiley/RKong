#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "draw.h"

#define DRAW_C
#include "sprites.h"

#include <math.h>

#include <assert.h>

u8* make_map(size_t w, size_t h) {
    u8* buffer = (u8*)malloc(w * h * 4);

    size_t latterX = 0;
    
    float slopes[6] = {0.5, 0.5, 0.5, 0.5, 0.35, -0.005};
    u8 ladder[6] = {70, 80, 70, 80, 40, 0};

    for (size_t i = 0; i < 6; i++) {
        size_t y = (100 + (i * 14) + (i * 75));
        assert((y + 14) <= h);
        
        size_t x = (i % 2) * 100; 
        if (i == 5) x = 0;

        for (; x < w - 20; x += 20) {                
            size_t ladderX = 0;
            if ((i != 0 && i != 5) || x >= (w / 2))
                y = y + (i * slopes[i]);
            
            drawSprite(buffer, w * 4, h, (u8*)girderSprite, x, y, 20, 14);
                
            if (i == 5) continue;

            if (i % 2) { 
                if (x != 120) continue;
            } else {
                if (i != 5 && x >= w - 100) break;
                if (x != w - 120) continue;          
            }

            for (size_t y2 = y - 10; y2 < y + ladder[i]; y2 += 9) { 
                drawSprite(buffer, w * 4, h, ladderSprite, x, y2, 24, 9);
            }
        }
    }
    
    return buffer;
}

void free_map(u8* map) {
     free(map);
}

#include <stdio.h>
u32 drawSprite(u8* buffer, size_t stride, size_t height, u8* sprite, size_t x, size_t y, i32 width, size_t h) {
    u8 flip = 0;
    size_t w = (size_t)abs(width);
    if (width < 0)
        flip = 1;

    if (y < 0 || y + h > height) return 0;

    u32 out = 0;
    for (size_t y1 = y; y1 < y + h; y1++) {
        for (size_t x1 = x; x1 < x + w; x1++) {
            size_t index = (y1 * stride) + x1 * 4;
            
            size_t bitmap_index = ((y1 - y) * (w * 4)) + (x1 - x) * 4;
            if (flip)           
                bitmap_index = ((y1 - y) * (w * 4)) + (w - (x1 - x)) * 4;

            if (sprite[bitmap_index + 3] == 0x00 && sprite != (u8*)girderSprite) continue;
            
            if (out == 0 && !(*((u32*)&buffer[index]) == 0x000000FF || *((u32*)&buffer[index]) == 0x00000000 || *((u32*)&buffer[index]) == 0xFF000000))
                out = *((u32*)&buffer[index]);
            
            memcpy(&buffer[index], &sprite[bitmap_index], 4);
        }
    }
    
    return out;
}


