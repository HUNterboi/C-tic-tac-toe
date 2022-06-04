#include "jatek.h"

void menu_gombok_megrajzolasa (SDL_Renderer * renderer, Menu_gombok menu_gombok) {
    SDL_Texture * start_t = IMG_LoadTexture(renderer, "images/start_button.png");
    SDL_Texture * load_t = IMG_LoadTexture(renderer, "images/load_button.png");
    SDL_Texture * exit_t = IMG_LoadTexture(renderer, "images/exit_button.png");
    SDL_Texture * background_t = IMG_LoadTexture(renderer, "images/menu_background.jpg");

    SDL_RenderCopy(renderer, background_t, NULL, &menu_gombok.background);
    SDL_RenderCopy(renderer, start_t, NULL, &menu_gombok.start);
    SDL_RenderCopy(renderer, load_t, NULL, &menu_gombok.load);
    SDL_RenderCopy(renderer, exit_t, NULL, &menu_gombok.exit);

    SDL_DestroyTexture(start_t);
    SDL_DestroyTexture(load_t);
    SDL_DestroyTexture(exit_t);
    SDL_DestroyTexture(background_t);
    SDL_RenderPresent(renderer);
}

static void mentes (Sdl_video * sdl_video, Cella *** cellak, Jatek_gombok * jatek_gombok, int * table_size, babu * kovetkezo_jatekos, TTF_Font * font) {
    char nev [27] = "";
    mentes_betoltes_nev_beker(sdl_video, font, &nev);
    FILE * fp = fopen(nev, "wt");
    if (fp == NULL) {
        perror("Fájl megnyitása sikertelen");
        return;
    }
    fprintf(fp, "%d\n", (*table_size));
    fprintf(fp, "%d\n", (*kovetkezo_jatekos));
    for (int i = 0; i < (*table_size); i++) {
        for (int j = 0; j < (*table_size); j++) {
            fprintf(fp, "%d;%d;%d;%d;%d\n", (*cellak)[i][j].tartalma, (*cellak)[i][j].pozicio.x, (*cellak)[i][j].pozicio.y, (*cellak)[i][j].pozicio.w, (*cellak)[i][j].pozicio.h);
        }
    }
    fclose(fp);
    jatek_tabla_betoltes(sdl_video, cellak, jatek_gombok, table_size);
    SDL_RenderPresent(sdl_video->renderer);
}

static void win_screen (Sdl_video * sdl_video, Jatek_gombok * jatek_gombok, babu * kovetkezo_jatekos) {
    SDL_DestroyWindow(sdl_video->window);
    SDL_DestroyRenderer(sdl_video->renderer);
    jatek_gombok->exit.x = 0, jatek_gombok->exit.y = 0, jatek_gombok->exit.w = WINDOW_WIDTH, jatek_gombok->exit.h = WINDOW_HEIGHT;
    if (*kovetkezo_jatekos == kor) {
        defining("kor - axis win", sdl_video, WINDOW_WIDTH, WINDOW_HEIGHT);
        SDL_Texture * win_background_t = IMG_LoadTexture(sdl_video->renderer, "images/kor_win.jpg");
        SDL_RenderCopy(sdl_video->renderer, win_background_t, NULL, &(jatek_gombok->exit));
        SDL_DestroyTexture(win_background_t);
    }
    else {
        defining("iksz - allies win", sdl_video, WINDOW_WIDTH, WINDOW_HEIGHT);
        SDL_Texture * win_background_t = IMG_LoadTexture(sdl_video->renderer, "images/iksz_win.jpg");
        SDL_RenderCopy(sdl_video->renderer, win_background_t, NULL, &(jatek_gombok->exit));
        SDL_DestroyTexture(win_background_t);
    }
    SDL_RenderPresent(sdl_video->renderer);
}

static void win_detect (Sdl_video * sdl_video, Cella *** cellak, Jatek_gombok * jatek_gombok, int *table_size, babu * kovetkezo_jatekos,  int s, int o) {
    int i = s - 1;
    int j = o - 1;
    int in_a_row = 1;
    while (i >= 0 && j >= 0 && i < *table_size && j < *table_size && (*cellak)[i][j].tartalma == (*cellak)[s][o].tartalma) {
        in_a_row++;
        i--;
        j--;
    }
    i = s + 1;
    j = o + 1;
    while (i >= 0 && j >= 0 && i < *table_size && j < *table_size && (*cellak)[i][j].tartalma == (*cellak)[s][o].tartalma) {
        in_a_row++;
        i++;
        j++;
    }
    if (in_a_row >= 5)
    {
        win_screen(sdl_video, jatek_gombok, kovetkezo_jatekos);
    }
    i = s - 1;
    j = o + 1;
    in_a_row = 1;
    while (i >= 0 && j >= 0 && i < *table_size && j < *table_size && (*cellak)[i][j].tartalma == (*cellak)[s][o].tartalma) {
        in_a_row++;
        i--;
        j++;
    }
    i = s + 1;
    j = o - 1;
    while (i >= 0 && j >= 0 && i < *table_size && j < *table_size && (*cellak)[i][j].tartalma == (*cellak)[s][o].tartalma) {
        in_a_row++;
        i++;
        j--;
    }
    if (in_a_row >= 5)
    {
        win_screen(sdl_video, jatek_gombok, kovetkezo_jatekos);
    }
    i = s - 1;
    in_a_row = 1;
    while (i >= 0 && i < *table_size && (*cellak)[i][o].tartalma == (*cellak)[s][o].tartalma) {
        in_a_row++;
        i--;
    }
    i = s + 1;
    while (i >= 0 && i < *table_size && (*cellak)[i][o].tartalma == (*cellak)[s][o].tartalma) {
        in_a_row++;
        i++;
    }
    if (in_a_row >= 5)
    {
        win_screen(sdl_video, jatek_gombok, kovetkezo_jatekos);
    }
    j = o - 1;
    in_a_row = 1;
    while (j >= 0 && j < *table_size && (*cellak)[s][j].tartalma == (*cellak)[s][o].tartalma) {
        in_a_row++;
        j--;
    }
    j = o + 1;
    while (j >= 0 && j < *table_size && (*cellak)[s][j].tartalma == (*cellak)[s][o].tartalma) {
        in_a_row++;
        j++;
    }
    if (in_a_row >= 5)
    {
        win_screen(sdl_video, jatek_gombok, kovetkezo_jatekos);
    }
}

static void hit_detect (Sdl_video * sdl_video, SDL_Event ev, Cella *** cellak, Jatek_gombok * jatek_gombok, int * table_size, babu * kovetkezo_jatekos) {
    int s = 0;
    int o = 0;
    bool found = false;
    while (s < (*table_size) && found == false) {
        o = 0;
        while (o < (*table_size) && found == false) {
            if ((*cellak)[s][o].tartalma == semmi && (ev.button.x >= (*cellak)[s][o].pozicio.x && ev.button.x <= (*cellak)[s][o].pozicio.x + (*cellak)[s][o].pozicio.w) && (ev.button.y >= (*cellak)[s][o].pozicio.y && ev.button.y <= (*cellak)[s][o].pozicio.y + (*cellak)[s][o].pozicio.h)) {
                if (*kovetkezo_jatekos == iksz) {
                    SDL_Texture * unit_t = IMG_LoadTexture(sdl_video->renderer, "images/units.png");
                    SDL_Rect source = {60, 0, 60, 60};
                    SDL_RenderCopy(sdl_video->renderer, unit_t, &source, &((*cellak)[s][o].pozicio));
                    SDL_DestroyTexture(unit_t);
                }
                else {
                    SDL_Texture * unit_t = IMG_LoadTexture(sdl_video->renderer, "images/units.png");
                    SDL_Rect source = {0, 0, 60, 60};
                    SDL_RenderCopy(sdl_video->renderer, unit_t, &source, &((*cellak)[s][o].pozicio));
                    SDL_DestroyTexture(unit_t);
                }
                (*cellak)[s][o].tartalma = *kovetkezo_jatekos;
                SDL_RenderPresent(sdl_video->renderer);
                found = true;
            }
            if (found == false)
                o++;
        }
        if (found == false)
            s++;
    }
    if (found) {
        win_detect(sdl_video, cellak, jatek_gombok, table_size, kovetkezo_jatekos, s, o);
        if (*kovetkezo_jatekos == iksz) {
            *kovetkezo_jatekos = kor;
        }
        else {
            *kovetkezo_jatekos = iksz;
        }
    }
}

static void kilepes (Sdl_video * sdl_video, IsRunning * isRunning, Menu_gombok menu_gombok) {
    isRunning->game = false;
    isRunning->menu = true;
    SDL_DestroyWindow(sdl_video->window);
    SDL_DestroyRenderer(sdl_video->renderer);
    defining("Bacskai Tamas - Amoba menu", sdl_video, WINDOW_WIDTH, WINDOW_HEIGHT);
    menu_gombok_megrajzolasa(sdl_video->renderer, menu_gombok);
    SDL_RenderPresent(sdl_video->renderer);
}

void jatek_loop (Adatstruktura * adatstruktura) {
    SDL_WaitEvent(&adatstruktura->ev);
    switch (adatstruktura->ev.type) {
        case SDL_QUIT:
            kilepes(&adatstruktura->sdl_video, &adatstruktura->isRunning, adatstruktura->menu_gombok);
            break;
        case SDL_MOUSEBUTTONDOWN:
            if ((adatstruktura->ev.button.x >= adatstruktura->jatek_gombok.exit.x && adatstruktura->ev.button.x <= adatstruktura->jatek_gombok.exit.x + adatstruktura->jatek_gombok.exit.w) && (adatstruktura->ev.button.y >= adatstruktura->jatek_gombok.exit.y && adatstruktura->ev.button.y <= adatstruktura->jatek_gombok.exit.y + adatstruktura->jatek_gombok.exit.h)) {
                kilepes(&adatstruktura->sdl_video, &adatstruktura->isRunning, adatstruktura->menu_gombok);
            }
            else if ((adatstruktura->ev.button.x >= adatstruktura->jatek_gombok.save.x && adatstruktura->ev.button.x <= adatstruktura->jatek_gombok.save.x + adatstruktura->jatek_gombok.save.w) && (adatstruktura->ev.button.y >= adatstruktura->jatek_gombok.save.y && adatstruktura->ev.button.y <= adatstruktura->jatek_gombok.save.y + adatstruktura->jatek_gombok.save.h)) {
                mentes(&adatstruktura->sdl_video, &adatstruktura->cellak, &adatstruktura->jatek_gombok, &adatstruktura->table_size, &adatstruktura->kovetkezo_jatekos, adatstruktura->font);
            }
            else {
                hit_detect(&adatstruktura->sdl_video, adatstruktura->ev, &adatstruktura->cellak, &adatstruktura->jatek_gombok, &adatstruktura->table_size, &adatstruktura->kovetkezo_jatekos);
            }
            break;
    }
}
