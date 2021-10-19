#include <spdlog/sinks/base_sink.h>

class GuiLogger : public spdlog::sinks::base_sink<spdlog::details::null_mutex>   {
    void sink_it_(const spdlog::details::log_msg& msg) override;    //NOLINT: third party override.

    void flush_() override;  //NOLINT: third party override.
};