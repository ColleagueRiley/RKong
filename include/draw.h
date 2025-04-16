#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#ifdef RGFW_DEBUG
#define INFO_LOG(x) printf(x);
#else
#define INFO_LOG(x)
#endif

typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t i32;

u8* make_map(size_t w, size_t h);
void free_map(u8* map);

u32 drawSprite(u8* buffer, size_t stride, size_t height, u8* sprite, size_t x, size_t y, i32 w, size_t h);
