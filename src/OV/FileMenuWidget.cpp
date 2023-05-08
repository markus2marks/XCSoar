/*
 * FileMenuWidget.cpp
 *
 *  Created on: May 1, 2023
 *      Author: markus
 */

#include "FileMenuWidget.h"
#include <sys/mount.h>
#include "Widget/TextWidget.hpp"
//#include "OpenVarioMenu.h"

void loadIGCToSDCard();

void loadIGCToSDCard()
{
    TextWidget text;
    PixelRect pr(100,100,100,100);
    text.Prepare(*(UIGlobals::GetMainWindow().GetParent()), pr);
    int ret_val = mount("/dev/mmcblk2p1", "media/sd-card", "", MS_SHARED, "");
    if (ret_val == 0)
    {
            text.SetText("mount succeed\n");
    }
    else
    {
        text.SetText("mount failed\n");
    }
}

void FileMenuWidget::Prepare([[maybe_unused]] ContainerWindow &parent,
                        [[maybe_unused]] const PixelRect &rc) noexcept
{
  AddButton("Download XCSoar IGC files to SD-Card", loadIGCToSDCard);

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

