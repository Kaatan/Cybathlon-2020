#pragma once


#ifndef __AFFICHAGE_H__
#define __AFFICHAGE_H__



//void renderPixel(float x, float y, uint8_t R, uint8_t G, uint8_t B);

//void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel);

void renderLine(float x1, float y1, float x2, float y2);

void renderThickLine(float x1, float y1, float x2, float y2);

void renderPivot(int x, int y);

//void BlackScreen();
#endif
