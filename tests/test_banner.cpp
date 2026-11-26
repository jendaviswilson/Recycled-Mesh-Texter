// Checks Banner against the behavior described in ui/Banner.h.

#include <cstdio>
#include <string>
#include "ui/Banner.h"

int failureCount = 0;
int checkCount = 0;

void check(bool condition, const char *description) {
  checkCount++;
  if (condition) {
    std::printf("pass: %s\n", description);
  } else {
    failureCount++;
    std::printf("FAIL: %s\n", description);
  }
}

int main() {
  Banner banner(3000);
  check(!banner.isActive(0), "never-shown banner is inactive at time 0");
  check(!banner.isActive(100), "never-shown banner is inactive later");

  banner.show("NEW: BOB", 1000);
  check(banner.text() == "NEW: BOB", "show stores the text");
  check(banner.isActive(1000), "active at the moment it is shown");
  check(banner.isActive(3999), "active just before the duration ends");
  check(!banner.isActive(4000), "inactive once the full duration has passed");
  check(!banner.isActive(9000), "stays inactive later");

  banner.show("NEW: ED", 5000);
  check(banner.text() == "NEW: ED", "second show replaces the text");
  check(banner.isActive(7000), "second show restarts the timer");
  banner.dismiss();
  check(!banner.isActive(5001), "dismiss hides it immediately");
  check(!banner.isActive(7000), "dismissed banner stays hidden");

  banner.show("NEW: AL", 10000);
  check(banner.isActive(10001), "show after dismiss makes it active again");

  // A 32-bit millisecond clock rolls over after about 49.7 days.
  banner.show("NEW: ROLL", 4294966000u);
  check(banner.isActive(500), "active across clock rollover (1796 ms elapsed)");
  check(!banner.isActive(2000), "expires correctly after rollover (3296 ms elapsed)");

  Banner shortBanner(0);
  shortBanner.show("NEW: ZERO", 100);
  check(!shortBanner.isActive(100), "zero duration is never active");

  std::printf("\n%d checks, %d failed\n", checkCount, failureCount);
  return failureCount == 0 ? 0 : 1;
}