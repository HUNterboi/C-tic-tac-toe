#include "menu/menu.h"
#include "jatek/jatek.h"
#include "adatszerkezetek_megjelenites/adatszerkezetek_megjelenites.h"

int main(int argc, char *argv[]) {
    ///Definialas, inicializalas
    sdl_init();
    Adatstruktura adatstruktura;
    defining("Bacskai Tamas - Amoba menu", &adatstruktura.sdl_video, WINDOW_WIDTH, WINDOW_HEIGHT);
    ttf_font_init(&adatstruktura.font);

    adatstruktura.menu_gombok.background.x = 0, adatstruktura.menu_gombok.background.y = 0, adatstruktura.menu_gombok.background.w = WINDOW_WIDTH, adatstruktura.menu_gombok.background.h = WINDOW_HEIGHT;
    adatstruktura.menu_gombok.start.x = WINDOW_WIDTH / 4, adatstruktura.menu_gombok.start.y = 105, adatstruktura.menu_gombok.start.w = WINDOW_WIDTH / 2, adatstruktura.menu_gombok.start.h = WINDOW_HEIGHT / 8;
    adatstruktura.menu_gombok.load.x = WINDOW_WIDTH / 4, adatstruktura.menu_gombok.load.y = 210, adatstruktura.menu_gombok.load.w = WINDOW_WIDTH / 2, adatstruktura.menu_gombok.load.h = WINDOW_HEIGHT / 8;
    adatstruktura.menu_gombok.exit.x = WINDOW_WIDTH / 4, adatstruktura.menu_gombok.exit.y = 315, adatstruktura.menu_gombok.exit.w = WINDOW_WIDTH / 2, adatstruktura.menu_gombok.exit.h = WINDOW_HEIGHT / 8;
    menu_gombok_megrajzolasa(adatstruktura.sdl_video.renderer, adatstruktura.menu_gombok);

    ///jatekohoz szukseges valtozok
    adatstruktura.cellak = NULL;
    adatstruktura.table_size = 0;
    adatstruktura.kovetkezo_jatekos = iksz;

    ///Eventek
    adatstruktura.isRunning.program = true;
    adatstruktura.isRunning.game = false;
    adatstruktura.isRunning.menu = true;

    while (adatstruktura.isRunning.program) {
        while (adatstruktura.isRunning.menu) {
            menu_loop(&adatstruktura);
        }
        while (adatstruktura.isRunning.game) {
            jatek_loop(&adatstruktura);
        }
    }
    if (adatstruktura.cellak != NULL) {
        free(adatstruktura.cellak[0]);
        free(adatstruktura.cellak);
    }
    TTF_CloseFont(adatstruktura.font);
    SDL_DestroyWindow(adatstruktura.sdl_video.window);
    SDL_DestroyRenderer(adatstruktura.sdl_video.renderer);
    SDL_Quit();
    return 0;
}
