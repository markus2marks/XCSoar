/*
 * MainMenuWidget.cpp
 *
 *  Created on: May 1, 2023
 *      Author: markus
 */

#include "MainMenuWidget.h"

/*
 * Function Prepare
 */
void MainMenuWidget::Prepare([[maybe_unused]] ContainerWindow &parent,
      [[maybe_unused]] const PixelRect &rc) noexcept
{
  AddButton("Start XCSoar", [this](){
    CancelTimer();
    StartXCSoar();
  });

  AddButton("File", [this](){
    CancelTimer();

    TWidgetDialog<FileMenuWidget>
      sub_dialog(WidgetDialog::Full{}, dialog.GetMainWindow(),
                 GetLook(), "OpenVario File");
    sub_dialog.SetWidget(display, event_queue, GetLook());
    sub_dialog.AddButton(_("Close"), mrOK);
    return sub_dialog.ShowModal();
  });

  AddButton("System", [this](){
    CancelTimer();

    TWidgetDialog<SystemMenuWidget>
      sub_dialog(WidgetDialog::Full{}, dialog.GetMainWindow(),
                 GetLook(), "OpenVario System");
    sub_dialog.SetWidget(display, event_queue, GetLook());
    sub_dialog.AddButton(_("Close"), mrOK);
    return sub_dialog.ShowModal();
  });

  AddButton("Connections", [this](){
    CancelTimer();

    TWidgetDialog<ConnectionsMenuWidget>
      sub_dialog(WidgetDialog::Full{}, dialog.GetMainWindow(),
                 GetLook(), "OpenVario Connections");
    sub_dialog.SetWidget(display, event_queue, GetLook());
    sub_dialog.AddButton(_("Close"), mrOK);
    return sub_dialog.ShowModal();
  });

  AddButton("Shell", [this](){
    dialog.SetModalResult(LAUNCH_SHELL);
  });

  AddButton("Reboot", [](){
    Run("/sbin/reboot");
  });

  AddButton("Power off", [](){
    Run("/sbin/poweroff");
  });

  AddReadOnly("");
}


