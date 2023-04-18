// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright The XCSoar Project

#pragma once

namespace UI { class Display; }

bool
#ifdef MESA_KMS
Startup(UI::Display &display, UI::Display &roundDisplay);
#else
Startup(UI::Display &display);
#endif

void
Shutdown();
