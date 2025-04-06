#ifndef THEME_HANDLER_H
#define THEME_HANDLER_H

#include <Arduino.h>
#include "theme_type.h"

extern EscapeRoomTheme currentTheme;
extern bool themeLocked;

void displayThemeSelection();
void handleThemeSelection();
void handleThemeReset();

#endif
