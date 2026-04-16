#ifndef OSSM_SOFTWARE_PAUSE_MENU_H
#define OSSM_SOFTWARE_PAUSE_MENU_H
namespace pause_menu {
    enum class PauseMenu {
        RESUME,
        END_SESSION
    };
    void drawPauseMenu();
    extern PauseMenu pauseMenuOption;
};


#endif // OSSM_SOFTWARE_PAUSE_MENU_H
