/*
 * FileMenuWidget.cpp
 *
 *  Created on: May 1, 2023
 *      Author: markus
 */

#include "FileMenuWidget.h"
#include <sys/mount.h>
#include "Widget/TextWidget.hpp"
#include "Dialogs/WidgetDialog.hpp"
#include "MainMenuWidget.h"


void FileMenuWidget::loadIGCToSDCard()
{

//    int ret_val = 1;//mount("/dev/mmcblk2p1", "media/sd-card", "", MS_SHARED, "");
//    if (ret_val == 0)
//    {
//            text.SetText("mount succeed\n");
//    }
//    else
//    {
//        text.SetText("mount failed\n");
//    }
}


void FileMenuWidget::Prepare([[maybe_unused]] ContainerWindow &parent,
                        [[maybe_unused]] const PixelRect &rc) noexcept
{

  AddButton("Download XCSoar IGC files to SD-Card", [this]()
    {
      ShowRow(3);
       int ret_val = mount("/dev/mmcblk2p1", "media/sd-card", "", MS_SHARED, "");
          if (ret_val == 0)
          {
            this->SetMultiLineText(3,"mount succeed\n");
          }
          else
          {
            this->SetMultiLineText(3,"mount failed\n");
          }

    }
  );

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
  AddMultiLine("");
}

