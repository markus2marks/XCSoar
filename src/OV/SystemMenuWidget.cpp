/*
 * SystemMenuWidget.cpp
 *
 *  Created on: May 1, 2023
 *      Author: markus
 */
#include "SystemMenuWidget.h"

void SystemMenuWidget::Prepare([[maybe_unused]] ContainerWindow &parent,
                          [[maybe_unused]] const PixelRect &rc) noexcept
{
  AddButton("Update System", [](){
    static constexpr const char *argv[] = {
      "/usr/bin/update-system.sh", nullptr
    };

    RunProcessDialog(UIGlobals::GetMainWindow(),
                     UIGlobals::GetDialogLook(),
                     "Update System", argv);
  });

  AddButton("Update Maps", [](){
    static constexpr const char *argv[] = {
      "/usr/bin/update-maps.sh", nullptr
    };

    RunProcessDialog(UIGlobals::GetMainWindow(),
                     UIGlobals::GetDialogLook(),
                     "Update Maps", argv);
  });

//  AddButton("Calibrate Sensors", CalibrateSensors);
//  AddButton("Calibrate Touch", [this](){
//    const UI::ScopeDropMaster drop_master{display};
//    const UI::ScopeSuspendEventQueue suspend_event_queue{event_queue};
//    Run("/usr/bin/ov-calibrate-ts.sh");
//  });

  AddButton("System Settings", [this](){
    const UI::ScopeDropMaster drop_master{display};
    const UI::ScopeSuspendEventQueue suspend_event_queue{event_queue};
    Run("/usr/lib/openvario/libexec/system_settings.sh");
  });

  AddButton("System Info", [this](){
    const UI::ScopeDropMaster drop_master{display};
    const UI::ScopeSuspendEventQueue suspend_event_queue{event_queue};
    Run("/usr/bin/raspinfo.sh");
  });
}


