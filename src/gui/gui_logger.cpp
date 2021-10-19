#include "cad-base/gui/gui_logger.hpp"

#include <iostream>
#include <spdlog/sinks/base_sink.h>

void GuiLogger::sink_it_(const spdlog::details::log_msg& msg) {
    // log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
    // msg.raw contains pre formatted log

    // If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
    spdlog::memory_buf_t formatted;
    spdlog::sinks::base_sink<spdlog::details::null_mutex>::formatter_->format(msg, formatted);
    std::cout << "test: " << fmt::to_string(formatted);
}

void GuiLogger::flush_() {
    std::cout << std::flush;
}
