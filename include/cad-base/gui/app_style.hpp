#ifndef APP_STYLE_HPP
#define APP_STYLE_HPP

#include "imgui.h"

struct AppStyle {
    const ImVec4 ACCENT_COLOUR_DIM   = ImVec4(0.160f, 0.204f, 0.204f, 1.000f); //Standard highlight colour, used for active tabs etc
    const ImVec4 ACCENT_COLOUR_MEDIUM = ImVec4(0.25f, 0.38f, 0.36f, 1.00f); //Less important highlight colour, used for inactive tabs etc
    const ImVec4 ACCENT_COLOUR_BRIGHT = ImVec4(0.24f, 0.38f, 0.34f, 1.000f); //Brighter, used for hovering etc

    const ImVec4 BACKGROUND_COLOUR_DIM   = ImVec4(0.06f, 0.06f, 0.06f, 1.00f); //Standard background colour

    const ImVec4 BACKGROUND_COLOUR_MEDIUM = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    const ImVec4 BACKGROUND_COLOUR_BRIGHT = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    const ImVec4 BACKGROUND_COLOUR_TEXT = ImVec4(0.88f, 0.88f, 0.88f, 1.00f); //Brighter, used for text

    const ImVec4 ACCENT_COLOUR_UNDEFINED = ImVec4(1.00f, 0.43f, 0.35f, 1.00f); //A highly contrasting colour used to show elements not themed yet.
};

#endif