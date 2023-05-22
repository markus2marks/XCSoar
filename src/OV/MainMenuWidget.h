/*
 * MainMenuWidget.h
 *
 *  Created on: May 1, 2023
 *      Author: markus
 */

#ifndef SRC_OV_MAINMENUWIDGET_H_
#define SRC_OV_MAINMENUWIDGET_H_
#include "Dialogs/DialogSettings.hpp"
#include "Dialogs/Message.hpp"
#include "Dialogs/WidgetDialog.hpp"
#include "Dialogs/ProcessDialog.hpp"
#include "UIGlobals.hpp"
#include "Look/DialogLook.hpp"
#include "Screen/Layout.hpp"
#include "../test/src/Fonts.hpp"
#include "ui/window/Init.hpp"
#include "ui/window/SingleWindow.hpp"
#include "ui/event/Queue.hpp"
#include "ui/event/Timer.hpp"
#include "Language/Language.hpp"
#include "system/Process.hpp"
#include "util/ScopeExit.hxx"
#include "Hardware/DisplayDPI.hpp"
#include "Widget/RowFormWidget.hpp"
#include "FileMenuWidget.h"
#include "SystemMenuWidget.h"
#include "ConnectionsMenuWidget.h"

enum Buttons {
  LAUNCH_SHELL = 100,
};

class MainMenuWidget final : public RowFormWidget
{
  enum Controls {
    XCSOAR,
    FILE,
    SYSTEM,
    CONNECTIONS,
    SHELL,
    REBOOT,
    SHUTDOWN,
    TIMER,
  };

  UI::Display &display;
  UI::EventQueue &event_queue;

  WndForm &dialog;

  UI::Timer timer{[this](){
    if (--remaining_seconds == 0) {
      HideRow(Controls::TIMER);
      StartXCSoar();
    } else {
      ScheduleTimer();
    }
  }};

  unsigned remaining_seconds = 3;

public:
  MainMenuWidget(UI::Display &_display, UI::EventQueue &_event_queue, WndForm &_dialog) noexcept
  : RowFormWidget(_dialog.GetLook()), display(_display), event_queue(_event_queue), dialog(_dialog) {}

private:
  void StartXCSoar() noexcept {
    const UI::ScopeDropMaster drop_master{display};
    const UI::ScopeSuspendEventQueue suspend_event_queue{event_queue};
    Run("/usr/bin/xcsoar", "-fly", "-dpi=150");
  }

  void ScheduleTimer() noexcept {
    assert(remaining_seconds > 0);

    timer.Schedule(std::chrono::seconds{1});

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Starting XCSoar in %u seconds (press any key to cancel)",
             remaining_seconds);
    SetText(Controls::TIMER, buffer);
  }

  void CancelTimer() noexcept {
    timer.Cancel();
    remaining_seconds = 0;
    HideRow(Controls::TIMER);
  }

  /* virtual methods from class Widget */
  void Prepare(ContainerWindow &parent, const PixelRect &rc) noexcept override;

  void Show(const PixelRect &rc) noexcept override {
    RowFormWidget::Show(rc);

    if (remaining_seconds > 0)
      ScheduleTimer();
  }

  void Hide() noexcept override {
    CancelTimer();
    RowFormWidget::Hide();
  }

  bool KeyPress(unsigned key_code) noexcept override {
    CancelTimer();
    return RowFormWidget::KeyPress(key_code);
  }
};



#endif /* SRC_OV_MAINMENUWIDGET_H_ */
