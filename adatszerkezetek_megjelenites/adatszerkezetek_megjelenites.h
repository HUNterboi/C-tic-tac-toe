#ifndef ADATSZERKEZETEK_MEGJELENITES_H
#define ADATSZERKEZETEK_MEGJELENITES_H

#define WINDOW_WIDTH 680
#define WINDOW_HEIGHT 420
#define FONT_SIZE 32
#define FONT "C:/Windows/Fonts/times.ttf"
#define CELLSIZE 25

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

#include "../debugmalloc/debugmalloc-impl.h"
#include "../debugmalloc/debugmalloc.h"

typedef struct Sdl_video {
    SDL_Window * window;
    SDL_Renderer * renderer;
} Sdl_video;

typedef enum babu {
    semmi, iksz, kor
} babu;

typedef struct Cella {
    SDL_Rect pozicio;
    babu tartalma;
} Cella;

typedef struct IsRunning {
    bool program;
    bool game;
    bool menu;
} IsRunning;

typedef struct Menu_gombok {
    SDL_Rect start, load, exit, background;
} Menu_gombok;

typedef struct Jatek_gombok {
    SDL_Rect exit, save, background;
} Jatek_gombok;

typedef struct Adatstruktura {
    Sdl_video sdl_video;
    SDL_Event ev;
    IsRunning  isRunning;
    Menu_gombok menu_gombok;
    Jatek_gombok  jatek_gombok;
    TTF_Font * font;
    int  table_size;
    Cella ** cellak;
    babu  kovetkezo_jatekos;
} Adatstruktura;

void sdl_init ();

void ttf_font_init (TTF_Font **pfont);

void defining (const char* felirat, Sdl_video * sdl_video, int szeles, int magas);

bool input_text(char *dest, size_t hossz, SDL_Rect teglalap, SDL_Color hatter, SDL_Color szoveg, TTF_Font *font, SDL_Renderer *renderer);

void jatek_ablak_letrehozas (Sdl_video * sdl_video, Jatek_gombok * jatek_gombok, int * table_size);

void jatek_gombok_megrajzolasa (SDL_Renderer * renderer, Jatek_gombok * jatek_gombok);

void mentes_betoltes_nev_beker (Sdl_video * sdl_video, TTF_Font * font, char * egesz_nev []);

void jatek_tabla_betoltes (Sdl_video * sdl_video, Cella *** cellak, Jatek_gombok * jatek_gombok, int * table_size);

#endif
