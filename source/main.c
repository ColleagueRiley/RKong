#define RGFW_IMPLEMENTATION
#define RGFW_BUFFER
#include "RGFW.h"

#include <stdio.h>
#include <time.h>

#include "sprites.h"
#include "draw.h"

typedef u32 playerStateEnum;
enum {
    playerNone = RGFW_BIT(0),
    playerAtLadder = RGFW_BIT(1),
    playerOnLadder = RGFW_BIT(2),
    playerJump = RGFW_BIT(3),
    playerFall = RGFW_BIT(4),
    playerDead = RGFW_BIT(5)
};

int main(void) {
    RGFW_window* window = RGFW_createWindow("dk", RGFW_RECT(0, 0, 500, 570), RGFW_windowCenter | RGFW_windowNoResize);
    
    u8* map = make_map(window->r.w, window->r.h);

    srand(time(NULL));
    u32 frames = 0;
    double frameStartTime = RGFW_getTime();

    RGFW_rect player = RGFW_RECT(20, window->r.h - 52, 24, 28); 
    playerStateEnum playerState = 0;
    size_t jumpHeight = 0;

    float lonicSpriteIndex = 0; 
    
    RGFW_point thumbPoint[9] = {RGFW_POINT(80, 85), RGFW_POINT(80, 85), RGFW_POINT(80, 85), RGFW_POINT(80, 85), RGFW_POINT(80, 85), RGFW_POINT(80, 85),
                                RGFW_POINT(80, 85), RGFW_POINT(80, 85), RGFW_POINT(80, 85)};
    i8 thumbDir[9] = {1, 1, 1, 1, 1, 1, 1, 1};
    playerStateEnum thumbState[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    size_t thumbCount = 0;

    while (RGFW_window_shouldClose(window) == RGFW_FALSE) {
        while (RGFW_window_checkEvent(window));

        if (playerState != playerDead) {
            if (RGFW_isPressed(window, RGFW_left) && (playerState & playerOnLadder) == 0) {
                player.x--;
                player.w = -abs(player.w);
                
                if ((playerState & playerJump) == 0) {
                    player.y--;
                }

                if (lonicSpriteIndex >= 5)
                    lonicSpriteIndex = 0;
                lonicSpriteIndex += 0.1; 
            }

            else if (RGFW_isPressed(window, RGFW_right) && (playerState & playerOnLadder) == 0) {
                player.x++;
                player.w = abs(player.w);
                if ((playerState & playerJump) == 0) {
                    player.y--;
                }
                if (lonicSpriteIndex >= 5)
                    lonicSpriteIndex = 0;
                lonicSpriteIndex += 0.1; 
            } 
            if (RGFW_isPressed(window, RGFW_up) && (playerState & playerAtLadder)) {
                if (playerState & playerOnLadder) {
                    player.y--;
                    if (lonicSpriteIndex >= 8)
                        lonicSpriteIndex = 6;
                    lonicSpriteIndex += 0.1;
                } else {
                    playerState |= playerOnLadder;
                    lonicSpriteIndex = 5;
                } 
            }
            if (RGFW_isPressed(window, RGFW_down) && (playerState & playerOnLadder)) {
                if ((player.y + 20) % 65 && player.y + 20 < window->r.h) {
                    player.y++;
                    if (lonicSpriteIndex >= 7)
                        lonicSpriteIndex = 5;
                    lonicSpriteIndex += 0.1;
                } else {
                    lonicSpriteIndex = 0;
                    playerState ^= playerOnLadder;
                } 
            }
            if (RGFW_isPressed(window, RGFW_space) && (playerState & playerOnLadder) == 0 && (playerState & playerJump) == 0  && (playerState & playerFall) == 0) {
                playerState |= playerJump; 
                jumpHeight = 0;
            }
            
            if ((playerState & playerJump)) {
                jumpHeight++;
                if (player.y - 1 > 0) player.y -= 1;
                else {
                    playerState &= ~playerJump;
                    playerState |= playerFall;
                }
            }

            if (jumpHeight >= 30) {
                playerState &= ~playerJump;
            }
     
            if ((playerState & playerFall) && player.y + player.h <= window->r.h)
                player.y += 2;
        }  else if (RGFW_isPressed(window, RGFW_return)) {
            player = RGFW_RECT(20, window->r.h - 52, 24, 28); 
            playerState = 0;
            jumpHeight = 0;
            lonicSpriteIndex = 0; 
            
            RGFW_point oldPoints[9] = {RGFW_POINT(80, 85), RGFW_POINT(80, 85), RGFW_POINT(80, 85), RGFW_POINT(80, 85), RGFW_POINT(80, 85), RGFW_POINT(80, 85),
                                        RGFW_POINT(80, 85), RGFW_POINT(80, 85), RGFW_POINT(80, 85)};
            
            memcpy(thumbPoint, oldPoints, sizeof(RGFW_point) * 9);
            memset(thumbDir, 1, 9);
            memset(thumbState, 0, sizeof(playerStateEnum) * 9);

            thumbCount = 0;
        } else {
            lonicSpriteIndex = 8; 
        }

        memcpy(window->buffer, map, window->r.w * window->r.h * 4);

        drawSprite(window->buffer, window->r.w * 4, window->r.h, thumbSprite, 40, 70, 11, 15);

        for (size_t i = 0; i < thumbCount; i++) {
            u32 thumb = drawSprite(window->buffer, window->r.w * 4, window->r.h, thumbSprite, thumbPoint[i].x, thumbPoint[i].y, 11, 15);
            switch (thumb) {
                case 0x00:
                    thumbPoint[i].y += 8;
                    thumbState[i] |= playerFall;
                    break;

                case 0xAB0000FF: // red floor/girder    
                    if (thumbState[i] & playerFall) {
                        thumbState[i] &= ~playerFall;
                        if (thumbPoint[i].x >= window->r.w - 50 || thumbPoint[i].x <= 50)
                           thumbDir[i] = -thumbDir[i];
                    }
                    break;
            }
            
            if (thumbPoint[i].x <= 50 && thumbPoint[i].y >= window->r.h - 40)
                thumbPoint[i] = RGFW_POINT(80, 85);

            thumbPoint[i].y--;
            thumbPoint[i].x += thumbDir[i] * 2;
        }

        if (thumbCount < sizeof(thumbPoint) / sizeof(thumbPoint[0]) && rand() % 1000 >= 993) {
            thumbCount++;
        }

        u32 ray = drawSprite(window->buffer, window->r.w * 4, window->r.h, raySprite, 60, 70, 32, 29);
    
        u32 topCollide = drawSprite(window->buffer, window->r.w * 4,  window->r.h,
                                    &lonicSpriteSheet[player.h * abs(player.w) * ((u32)lonicSpriteIndex) * 4], player.x, player.y, player.w, player.h - 10);
        
         switch (topCollide) {
            case 0x00: // nothing (probably in the air)
                playerState &= ~playerAtLadder;
                if (playerState & playerOnLadder)
                    player.y -= 20;
                playerState &= ~playerOnLadder;

                if ((playerState & playerJump) == 0) // in the air but not jumping
                    playerState |= playerFall;
                break;
            case 0xffffe300: // teal ladder
                playerState |= playerAtLadder;
                break;
            case 0xff00d0ff: // thumb or ray
                playerState = playerDead;
                lonicSpriteIndex = 8; 
                break; 
            case 0xAB0000FF: // red floor/girder
                if ((playerState & playerJump)) { // jumped and hit ladder, start falling
                    playerState &= ~playerJump;
                    playerState |= playerFall;
                }
                break; 
            default: break;
        }

        u32 collide = drawSprite(window->buffer, window->r.w * 4,  window->r.h,
                                    &lonicSpriteSheet[(abs(player.w) * ((player.h * (u32)lonicSpriteIndex) + (player.h - 10)) ) * 4], player.x, player.y + (player.h - 10), player.w, 10);

        switch (collide) {
            case 0x00: // nothing (probably in the air)
                if ((playerState & playerJump) == 0 && (playerState & playerOnLadder) == 0) // in the air but not jumping
                    playerState |= playerFall;
                break;
            case 0xffffe300: // teal ladder
                playerState |= playerAtLadder;

                if ((playerState & playerJump) == 0 && (playerState & playerOnLadder) == 0) // in the air but not jumping
                    playerState |= playerFall;
                break;
            case 0xAB0000FF: // red floor/girder
                //if ((playerState & playerAtLadder) == 0)  
                  //  playerState &= ~playerOnLadder; // reached girder while on the latter
                
                if ((playerState & playerFall)) // fell onto ladder, stop falling
                    playerState &= ~playerFall;
                if ((playerState & playerJump)) { // jumped and hit ladder, start falling
                    playerState &= ~playerJump;
                    playerState |= playerFall;
                }
                break;
            case 0xff03c1eb:
                playerState = playerDead;

                lonicSpriteIndex = 8; 
                break;
            default: printf("unhandled collision 0x%x\n", collide);
        }

        RGFW_window_swapBuffers(window);
        RGFW_checkFPS(frameStartTime, frames, 60);
        frames++;
    }

    free_map(map);

    RGFW_window_close(window);
}
