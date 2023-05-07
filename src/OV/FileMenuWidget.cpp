/*
 * FileMenuWidget.cpp
 *
 *  Created on: May 1, 2023
 *      Author: markus
 */

#include "FileMenuWidget.h"

void FileMenuWidget::Prepare([[maybe_unused]] ContainerWindow &parent,
                        [[maybe_unused]] const PixelRect &rc) noexcept
{
  AddButton("Download XCSoar IGC files to SD-Card", [this](){
    const UI::ScopeDropMaster drop_master{display};
    const UI::ScopeSuspendEventQueue suspend_event_queue{event_queue};
    Run("/usr/bin/download-igc.sh");
  });

  AddButton("Download XCSoar to USB", [](){
    static constexpr const char *argv[] = {
      "/usr/bin/download-all.sh", nullptr
    };

    RunProcessDialog(UIGlobals::GetMainWindow(),
                     UIGlobals::GetDialogLook(),
                     "Downloading files", argv);
  });

  AddButton("Upload files from SD-Card to XCSoar", [](){
    static constexpr const char *argv[] = {
      "/usr/bin/upload-xcsoar.sh", nullptr
    };

    RunProcessDialog(UIGlobals::GetMainWindow(),
                     UIGlobals::GetDialogLook(),
                     "Uploading files", argv);
  });
}

