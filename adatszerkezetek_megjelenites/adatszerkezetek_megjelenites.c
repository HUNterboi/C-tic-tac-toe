#include "adatszerkezetek_megjelenites.h"

///Ablak keszito
static void sdl_window_init (char const *felirat, int szeles, int magas, SDL_Window **pwindow) {
    SDL_Window *window = SDL_CreateWindow(felirat, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    *pwindow = window;
}

///Renderer keszito
static void sdl_renderer_init (Sdl_video * sdl_video) {
    SDL_Renderer *renderer = SDL_CreateRenderer(sdl_video->window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito : %s", SDL_GetError());
        exit(1);
    }
    sdl_video->renderer = renderer;
    ///Hatter
    SDL_SetRenderDrawColor(sdl_video->renderer, 255, 255, 255, 255);
    SDL_RenderClear(sdl_video->renderer);
}

///window, renderer letrehozasa
void defining (const char* felirat, Sdl_video * sdl_video, int szeles, int magas) {
    sdl_window_init(felirat, szeles, magas, &sdl_video->window);
    sdl_renderer_init(sdl_video);
}

///ttf inicializalas
void ttf_font_init (TTF_Font **pfont) {
    if (TTF_Init() < 0) {
        SDL_Log("Nem indithato a TTF: %s", TTF_GetError());
        exit(1);
    }
    TTF_Font *font = TTF_OpenFont(FONT,FONT_SIZE);
    if (font == NULL) {
        SDL_Log("Nem hozhato letre font : %s", TTF_GetError());
        exit(1);
    }
    *pfont = font;
}

///sdl init
void sdl_init () {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }
}

///szoveg bekérés sdl-ben (infoC-rõl)
bool input_text(char *dest, size_t hossz, SDL_Rect teglalap, SDL_Color hatter, SDL_Color szoveg, TTF_Font *font, SDL_Renderer *renderer) {
    /* Ez tartalmazza az aktualis szerkesztest */
    char composition[SDL_TEXTEDITINGEVENT_TEXT_SIZE];
    composition[0] = '\0';
    /* Ezt a kirajzolas kozben hasznaljuk */
    char textandcomposition[hossz + SDL_TEXTEDITINGEVENT_TEXT_SIZE + 1];
    /* Max hasznalhato szelesseg */
    int maxw = teglalap.w - 2;
    int maxh = teglalap.h - 2;

    dest[0] = '\0';

    bool enter = false;
    bool kilep = false;

    SDL_StartTextInput();
    while (!kilep && !enter) {
        /* doboz kirajzolasa */
        boxRGBA(renderer, teglalap.x, teglalap.y, teglalap.x + teglalap.w - 1, teglalap.y + teglalap.h - 1, hatter.r, hatter.g, hatter.b, 255);
        rectangleRGBA(renderer, teglalap.x, teglalap.y, teglalap.x + teglalap.w - 1, teglalap.y + teglalap.h - 1, szoveg.r, szoveg.g, szoveg.b, 255);
        /* szoveg kirajzolasa */
        int w;
        strcpy(textandcomposition, dest);
        strcat(textandcomposition, composition);
        if (textandcomposition[0] != '\0') {
            SDL_Surface *felirat = TTF_RenderUTF8_Blended(font, textandcomposition, szoveg);
            SDL_Texture *felirat_t = SDL_CreateTextureFromSurface(renderer, felirat);
            SDL_Rect cel = { teglalap.x, teglalap.y, felirat->w < maxw ? felirat->w : maxw, felirat->h < maxh ? felirat->h : maxh };
            SDL_RenderCopy(renderer, felirat_t, NULL, &cel);
            SDL_FreeSurface(felirat);
            SDL_DestroyTexture(felirat_t);
            w = cel.w;
        } else {
            w = 0;
        }
        /* kurzor kirajzolasa */
        if (w < maxw) {
            vlineRGBA(renderer, teglalap.x + w + 2, teglalap.y + 2, teglalap.y + teglalap.h - 3, szoveg.r, szoveg.g, szoveg.b, 192);
        }
        /* megjeleniti a képernyon az eddig rajzoltakat */
        SDL_RenderPresent(renderer);

        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {
            /* Kulonleges karakter */
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    int textlen = strlen(dest);
                    do {
                        if (textlen == 0) {
                            break;
                        }
                        if ((dest[textlen-1] & 0x80) == 0x00)   {
                            /* Egy bajt */
                            dest[textlen-1] = 0x00;
                            break;
                        }
                        if ((dest[textlen-1] & 0xC0) == 0x80) {
                            /* Bajt, egy tobb-bajtos szekvenciabol */
                            dest[textlen-1] = 0x00;
                            textlen--;
                        }
                        if ((dest[textlen-1] & 0xC0) == 0xC0) {
                            /* Egy tobb-bajtos szekvencia elso bajtja */
                            dest[textlen-1] = 0x00;
                            break;
                        }
                    } while(true);
                }
                if (event.key.keysym.sym == SDLK_RETURN) {
                    enter = true;
                }
                break;

            /* A feldolgozott szoveg bemenete */
            case SDL_TEXTINPUT:
                if (strlen(dest) + strlen(event.text.text) < hossz) {
                    strcat(dest, event.text.text);
                }

                /* Az eddigi szerkesztes torolheto */
                composition[0] = '\0';
                break;

            /* Szoveg szerkesztese */
            case SDL_TEXTEDITING:
                strcpy(composition, event.edit.text);
                break;

            case SDL_QUIT:
                /* visszatesszuk a sorba ezt az eventet, mert
                 * sok mindent nem tudunk vele kezdeni */
                SDL_PushEvent(&event);
                kilep = true;
                break;
        }
    }

    /* igaz jelzi a helyes beolvasast; = ha enter miatt allt meg a ciklus */
    SDL_StopTextInput();
    return enter;
}

void jatek_ablak_letrehozas (Sdl_video * sdl_video, Jatek_gombok * jatek_gombok, int * table_size) {
    SDL_DestroyWindow(sdl_video->window);
    SDL_DestroyRenderer(sdl_video->renderer);
    int szeles = CELLSIZE * (*table_size) + 10;
    int magas = CELLSIZE * (*table_size) + 100;
    defining("Bacskai Tamas - Amoba", sdl_video, szeles, magas);
    jatek_gombok->save.x = szeles / 10, jatek_gombok->save.y = 5, jatek_gombok->save.w = szeles / 3, jatek_gombok->save.h = 50;
    jatek_gombok->exit.x = (szeles - szeles / 2), jatek_gombok->exit.y = 5, jatek_gombok->exit.w = szeles / 3, jatek_gombok->exit.h = 50;
    jatek_gombok->background.x = 0, jatek_gombok->background.y = 0, jatek_gombok->background.w = szeles, jatek_gombok->background.h = magas;
}

void jatek_gombok_megrajzolasa (SDL_Renderer * renderer, Jatek_gombok * jatek_gombok) {
    SDL_Texture * save_t = IMG_LoadTexture(renderer, "images/save_button.png");
    SDL_Texture * game_exit_t = IMG_LoadTexture(renderer, "images/exit_button.png");
    SDL_Texture * background_t = IMG_LoadTexture(renderer, "images/game_background.jpg");

    SDL_RenderCopy(renderer, background_t, NULL, &(jatek_gombok->background));
    SDL_RenderCopy(renderer, save_t, NULL, &(jatek_gombok->save));
    SDL_RenderCopy(renderer, game_exit_t, NULL, &(jatek_gombok->exit));

    SDL_DestroyTexture(background_t);
    SDL_DestroyTexture(save_t);
    SDL_DestroyTexture(game_exit_t);
    SDL_RenderPresent(renderer);
}

void mentes_betoltes_nev_beker (Sdl_video * sdl_video, TTF_Font * font, char * egesz_nev []) {
    char nev [27] = "saves/";
    SDL_DestroyWindow(sdl_video->window);
    SDL_DestroyRenderer(sdl_video->renderer);
    defining("Mentes", sdl_video, WINDOW_WIDTH, WINDOW_HEIGHT);
    char nev2 [21] = "";
    SDL_Rect input_rect = {.x = WINDOW_WIDTH / 4, .y = WINDOW_HEIGHT / 10, .w = WINDOW_WIDTH / 2, .h = WINDOW_HEIGHT / 8};
    SDL_Color input_rect_hatter = {.r = 255, .g = 255, .b = 255, .a = 255};
    SDL_Color input_rect_betuszin = {.r = 0, .g = 0, .b = 0, .a = 255};
    while (strlen(nev2) > 20 || strlen(nev2) <= 0) {
        if (!input_text(nev2, 100, input_rect, input_rect_hatter, input_rect_betuszin, font, sdl_video->renderer)) {
            SDL_Log("Nem futott le sikeresen az input_text()!");
            return;
        }
    }
    strcat(nev, nev2);
    strcpy(egesz_nev, nev);
}

void jatek_tabla_betoltes (Sdl_video * sdl_video, Cella *** cellak, Jatek_gombok * jatek_gombok, int * table_size) {
    jatek_ablak_letrehozas(sdl_video, jatek_gombok, table_size);
    jatek_gombok_megrajzolasa(sdl_video->renderer, jatek_gombok);
    for (int i = 0; i < (*table_size); i++) {
        for (int j = 0; j < (*table_size); j++) {
            if ((*cellak)[i][j].tartalma == iksz) {
                SDL_Texture * unit_t = IMG_LoadTexture(sdl_video->renderer, "images/units.png");
                SDL_Rect source = {60, 0, 60, 60};
                SDL_RenderCopy(sdl_video->renderer, unit_t, &source, &((*cellak)[i][j].pozicio));
                SDL_DestroyTexture(unit_t);
            }
            else if ((*cellak)[i][j].tartalma == kor) {
                SDL_Texture * unit_t = IMG_LoadTexture(sdl_video->renderer, "images/units.png");
                SDL_Rect source = {0, 0, 60, 60};
                SDL_RenderCopy(sdl_video->renderer, unit_t, &source, &((*cellak)[i][j].pozicio));
                SDL_DestroyTexture(unit_t);
            }
            else {
                SDL_SetRenderDrawColor(sdl_video->renderer, 255, 255, 255, 255);
                SDL_RenderFillRect((sdl_video->renderer), &((*cellak)[i][j].pozicio));
                SDL_SetRenderDrawColor(sdl_video->renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect((sdl_video->renderer), &((*cellak)[i][j].pozicio));
            }
        }
    }
}
