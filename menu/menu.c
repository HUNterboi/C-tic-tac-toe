#include "menu.h"

static void palyameret_megadas (SDL_Renderer ** renderer, int * table_size, Cella *** cellak, TTF_Font * font) {
    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);
    SDL_RenderClear(*renderer);
    SDL_RenderPresent(*renderer);
    SDL_Rect input_rect = {.x = WINDOW_WIDTH / 4, .y = WINDOW_HEIGHT / 10, .w = WINDOW_WIDTH / 2, .h = WINDOW_HEIGHT / 8};
    char n_char[5];
    SDL_Color input_rect_hatter = {.r = 255, .g = 255, .b = 255, .a = 255};
    SDL_Color input_rect_betuszin = {.r = 0, .g = 0, .b = 0, .a = 255};
    *table_size = 0;
    if ((*cellak) != NULL) {
        free((*cellak)[0]);
        free(*cellak);
        cellak = NULL;
    }
    while (*table_size < 5) {
        *table_size = 0;
        if (!input_text(n_char, 100, input_rect, input_rect_hatter, input_rect_betuszin, font, *renderer)) {
            SDL_Log("Nem futott le sikeresen az input_text()!");
            return -1;
        }
        for (int i = 0; n_char[i] != '\0'; i++) {
            *table_size = (*table_size * 10) + (n_char[i] - '0');
        }
    }
}

static void uj_jatek (Sdl_video * sdl_video, Cella *** cellak, Jatek_gombok * jatek_gombok, int * table_size) {
    *cellak = (Cella **) malloc(sizeof(Cella *) * (*table_size));
    if (*cellak == NULL) {
        printf("Nem sikerult a dinamikus memoria foglalas");
        return;
    }
    (*cellak)[0] = (Cella *) malloc(sizeof(Cella) * (*table_size) * (*table_size));
    if (*cellak[0] == NULL) {
        printf("Nem sikerult a dinamikus memoria foglalas");
        free(*cellak);
        return;
    }
    for (int i = 1; i < *table_size; i++) {
        (*cellak)[i] = (*cellak)[0] + i * (*table_size);
    }
    jatek_ablak_letrehozas(sdl_video, jatek_gombok, table_size);
    jatek_gombok_megrajzolasa(sdl_video->renderer, jatek_gombok);
}

static void betoltes (Sdl_video * sdl_video, Cella *** cellak, Jatek_gombok * jatek_gombok, int * table_size, babu * kovetkezo_jatekos, TTF_Font * font) {
    char nev [27] = "";
    mentes_betoltes_nev_beker(sdl_video, font, &nev);
    FILE * fp = fopen(nev, "rt");
    if (fp == NULL) {
        perror("Fajl megnyitasa sikertelen");
        return;
    }
    fscanf(fp, "%d\n", table_size);
    if (*cellak != NULL) {
        free((*cellak)[0]);
        free(*cellak);
    }
    fscanf(fp, "%d\n", kovetkezo_jatekos);
    uj_jatek(sdl_video, cellak, jatek_gombok, table_size);
    for (int i = 0; i < (*table_size); i++) {
        for (int j = 0; j < (*table_size); j++) {
            fscanf(fp, "%d;%d;%d;%d;%d\n", &((*cellak)[i][j].tartalma), &((*cellak)[i][j].pozicio.x), &((*cellak)[i][j].pozicio.y), &((*cellak)[i][j].pozicio.w), &((*cellak)[i][j].pozicio.h));
        }
    }
    fclose(fp);
    jatek_tabla_betoltes(sdl_video, cellak, jatek_gombok, table_size);
    SDL_RenderPresent(sdl_video->renderer);
}

/// Játék ablak, renderer generálása, ablak szélességének, magasságának kiszámolása, játékon belüli gombok kiszámolása, játék celláinak kiszámolása, megrajzolása
static void uj_jatek_tabla_megrajzolas (SDL_Renderer ** renderer, Cella *** cellak, int * table_size) {
    for (int i = 0; i < (*table_size); i++) {
        for (int j = 0; j < (*table_size); j++) {
            (*cellak)[i][j].pozicio.x = (i * CELLSIZE) + 5, (*cellak)[i][j].pozicio.w = CELLSIZE, (*cellak)[i][j].pozicio.y = (j * CELLSIZE) + 75, (*cellak)[i][j].pozicio.h = CELLSIZE;
            (*cellak)[i][j].tartalma = semmi;
            SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);
            SDL_RenderFillRect((*renderer), &((*cellak)[i][j].pozicio));
            SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect((*renderer), &((*cellak)[i][j].pozicio));
        }
    }
    SDL_RenderPresent(*renderer);
}

static void kilepes (IsRunning * isRunning) {
    isRunning->program = false;
    isRunning->menu = false;
}

void menu_loop (Adatstruktura * adatstruktura) {
    SDL_WaitEvent(&adatstruktura->ev);
        switch (adatstruktura->ev.type) {
            case SDL_QUIT:
                kilepes(&adatstruktura->isRunning);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if ((adatstruktura->ev.button.x >= adatstruktura->menu_gombok.start.x && adatstruktura->ev.button.x <= adatstruktura->menu_gombok.start.x + adatstruktura->menu_gombok.start.w) && (adatstruktura->ev.button.y >= adatstruktura->menu_gombok.start.y && adatstruktura->ev.button.y <= adatstruktura->menu_gombok.start.y + adatstruktura->menu_gombok.start.h)) {
                        palyameret_megadas(&adatstruktura->sdl_video.renderer, &adatstruktura->table_size, &adatstruktura->cellak, adatstruktura->font);
                        uj_jatek(&adatstruktura->sdl_video, &adatstruktura->cellak, &adatstruktura->jatek_gombok, &adatstruktura->table_size);
                        uj_jatek_tabla_megrajzolas(&adatstruktura->sdl_video.renderer, &adatstruktura->cellak, &adatstruktura->table_size);
                        adatstruktura->isRunning.menu = false;
                        adatstruktura->isRunning.game = true;
                    }
                    else if ((adatstruktura->ev.button.x >= adatstruktura->menu_gombok.load.x && adatstruktura->ev.button.x <= adatstruktura->menu_gombok.load.x + adatstruktura->menu_gombok.load.w) && (adatstruktura->ev.button.y >= adatstruktura->menu_gombok.load.y && adatstruktura->ev.button.y <= adatstruktura->menu_gombok.load.y + adatstruktura->menu_gombok.load.h)) {
                        betoltes(&adatstruktura->sdl_video, &adatstruktura->cellak, &adatstruktura->jatek_gombok, &adatstruktura->table_size, &adatstruktura->kovetkezo_jatekos, adatstruktura->font);
                        adatstruktura->isRunning.menu = false;
                        adatstruktura->isRunning.game = true;
                    }
                    else if ((adatstruktura->ev.button.x >= adatstruktura->menu_gombok.exit.x && adatstruktura->ev.button.x <= adatstruktura->menu_gombok.exit.x + adatstruktura->menu_gombok.exit.w) && (adatstruktura->ev.button.y >= adatstruktura->menu_gombok.exit.y && adatstruktura->ev.button.y <= adatstruktura->menu_gombok.exit.y + adatstruktura->menu_gombok.exit.h)) {
                        kilepes(&adatstruktura->isRunning);
                    }
            }
}
