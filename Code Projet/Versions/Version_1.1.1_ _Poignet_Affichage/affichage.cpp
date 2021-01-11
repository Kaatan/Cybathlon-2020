
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "affichage.h"
#include <stdint.h>
#include <algorithm>
#include <math.h>


SDL_Renderer* sdlRenderer;

/*
void renderPixel(int x, int y, uint8_t R, uint8_t G, uint8_t B)
{
    uint32_t color = SDL_MapRGB(g_screenSurface->format, R, G, B);

    switch (g_screenSurface->format->BytesPerPixel) {
    case 1: { // Assuming 8-bpp
        uint8_t* bufp;
        bufp = (uint8_t*)g_screenSurface->pixels + y * (uint8_t) g_screenSurface->pitch + x;
        *bufp = color;
    }
          break;

    case 2: { // Probably 15-bpp or 16-bpp
        uint16_t* bufp;
        bufp = (uint16_t*)g_screenSurface->pixels + y * (uint8_t)g_screenSurface->pitch / 2 + x;
        *bufp = color;
    }
          break;

    case 3: { // Slow 24-bpp mode, usually not used
        uint8_t* bufp;
        bufp = (uint8_t*)g_screenSurface->pixels + y * (uint8_t)g_screenSurface->pitch + x * 3;
        if (SDL_BYTEORDER == SDL_LIL_ENDIAN) {
            bufp[0] = color;
            bufp[1] = color >> 8;
            bufp[2] = color >> 16;
        }
        else {
            bufp[2] = color;
            bufp[1] = color >> 8;
            bufp[0] = color >> 16;
        }
    }
          break;

    case 4: { // Probably 32-bpp
        uint32_t* bufp;
        bufp = (uint32_t*)g_screenSurface->pixels + y * g_screenSurface->pitch / 4 + x;
        *bufp = color;
    }
          break;
    }
}
// Inutile dans SDL2
*/

void renderLine(float x1, float y1, float x2, float y2)
{
    // Bresenham's line algorithm
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    if (steep)
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);

    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;

    for (int x = (int)x1; x < maxX; x++)
    {
        if (steep)
        {
            //renderPixel(y, x, 0, 255, 0);
            SDL_SetRenderDrawColor(sdlRenderer, 0, 255, 0, 255);// Couleur verte
            SDL_RenderDrawPoint(sdlRenderer, y, x);
        }
        else
        {
            //renderPixel(x, y, 0, 255, 0);
            SDL_SetRenderDrawColor(sdlRenderer, 0, 255, 0, 255);// Couleur verte
            SDL_RenderDrawPoint(sdlRenderer, x, y);

        }

        error -= dy;
        if (error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
}




void renderThickLine(float x1, float y1, float x2, float y2) {

    renderLine(x1+1, y1, x2+1, y2);
    renderLine(x1, y1, x2, y2);
    renderLine(x1 - 1, y1, x2-1, y2);
    renderLine(x1, y1 + 1, x2, y2);
    renderLine(x1, y1 - 1, x2, y2);
    

}

void renderPivot(int x, int y) { //render juste un carré

    for (int i = 0; i < 10; i++) {

        SDL_SetRenderDrawColor(sdlRenderer, 255, 0, 0, 255);// Couleur rouge

        SDL_RenderDrawPoint(sdlRenderer, x + i - 5, y + 5);
        SDL_RenderDrawPoint(sdlRenderer, x + i - 5, y - 5);
        SDL_RenderDrawPoint(sdlRenderer, x - 5, y + i - 5);
        SDL_RenderDrawPoint(sdlRenderer, x + 5, y - i + 5);
        /*
        renderPixel(x + i - 5, y + 5, 255, 0, 0);
        renderPixel(x + i - 5, y - 5, 255, 0, 0);
        renderPixel(x - 5, y + i - 5, 255, 0, 0);
        renderPixel(x + 5, y + i + 5, 255, 0, 0);
        */
    }


} //Renders the angle Points


/*
void BlackScreen() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; i < WIDTH; j++) {
            renderPixel(i, j, 0, 0, 0);
        }
    }
}

*/

