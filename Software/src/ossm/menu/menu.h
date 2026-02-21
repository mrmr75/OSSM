#ifndef OSSM_MENU_MENU_H
#define OSSM_MENU_MENU_H
#include <Arduino.h>

namespace menu {

/**
 * Draw and run the main menu
 * Handles encoder input for menu navigation
 */
void drawMenu();
template<typename MenuEnum, size_t N>
void drawMenuImpl(const char* headerText,
                        const char* const (&menuStrings)[N],
                        MenuEnum& currentOption);
}  // namespace menu

#endif  // OSSM_MENU_MENU_H
