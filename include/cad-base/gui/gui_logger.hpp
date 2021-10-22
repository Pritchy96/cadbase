#ifndef GUI_LOGGER_HPP
#define GUI_LOGGER_HPP


#include "imgui.h"
#include <spdlog/sinks/base_sink.h>

class GuiLogger : public spdlog::sinks::base_sink<spdlog::details::null_mutex>   {
    public: 
        void sink_it_(const spdlog::details::log_msg& msg) override;    //NOLINT: third party override.
        void flush_() override;  //NOLINT: third party override.
        
        void Update();
        void AddLog(const char* fmt, ...);

        ImGuiTextBuffer     buffer;
        ImGuiTextFilter     filter;
        ImVector<int>       line_offsets; // Index to lines offset. We maintain this with AddLog() calls.
        bool                auto_scroll = true;  // Keep scrolling if already at the bottom.

};

#endif