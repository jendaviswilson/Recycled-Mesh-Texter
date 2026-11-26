#include "Banner.h"

Banner::Banner(uint32_t durationMs)
{
    _text = "";
    _shownAtMs = 0;
    _dismissed = true;
    _durationMs = durationMs;
}

void Banner::show (const std::string &text, uint32_t nowMs)
{
    _dismissed = false;
    _shownAtMs = nowMs;
    _text = text;
}

void Banner::dismiss()
{
    _dismissed = true;
}

bool Banner::isActive(uint32_t nowMs) const
{
    return (!_dismissed 
        && (nowMs-_shownAtMs) < _durationMs);
}

const std::string &Banner::text() const
{
    return _text;
}
                                                                                         