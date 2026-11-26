#pragma once
#include <cstdint>
#include <string>

// A temporary alert line, such as "NEW: BOB", shown in place of the normal display for a while.
// Time is passed in as milliseconds (millis() on hardware), matching the core's uint32_t clock.
class Banner {
public:
  explicit Banner(uint32_t durationMs);

  // Shows text starting at nowMs. Replaces any banner already showing and restarts the timer.
  void show(const std::string &text, uint32_t nowMs);
  // Hides the banner immediately.
  void dismiss();

  // True from show() until durationMs has passed or dismiss() is called.
  bool isActive(uint32_t nowMs) const;
  // Text of the most recent show().
  const std::string &text() const;

private:
  std::string _text;
  uint32_t _durationMs;
  uint32_t _shownAtMs;
  bool _dismissed;
};
