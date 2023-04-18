// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright The XCSoar Project

#pragma once

#include "ui/display/Display.hpp"
#include "ui/event/Queue.hpp"

class ScreenGlobalInit {
  UI::Display display;
#ifdef MESA_KMS
  UI::Display roundDisplay;
#endif

#ifdef USE_POLL_EVENT
  UI::EventQueue event_queue{display};
#else
  UI::EventQueue event_queue;
#endif

public:
  ScreenGlobalInit();
  ~ScreenGlobalInit();

  auto &GetDisplay() noexcept {
    return display;
  }

#ifdef MESA_KMS
  auto &GetRoundDisplay() noexcept
  {
      return roundDisplay;
  }
#endif

  auto &GetEventQueue() noexcept {
    return event_queue;
  }
};
