#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <algorithm>
#include <math.h>




SDL_Renderer* sdlRenderer;

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

    renderLine(x1 + 1, y1, x2 + 1, y2);
    renderLine(x1, y1, x2, y2);
    renderLine(x1 - 1, y1, x2 - 1, y2);
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
        
    }


} //Renders the angle Points
