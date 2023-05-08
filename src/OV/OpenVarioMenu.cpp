// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright The XCSoar Project


#include "MainMenuWidget.h"
//#include "OpenVarioMenu.h"
#include <cassert>



static DialogSettings dialog_settings;
static UI::SingleWindow *global_main_window;
static DialogLook *global_dialog_look;

const DialogSettings &
UIGlobals::GetDialogSettings()
{
  return dialog_settings;
}

const DialogLook &
UIGlobals::GetDialogLook()
{
  assert(global_dialog_look != nullptr);

  return *global_dialog_look;
}

UI::SingleWindow &
UIGlobals::GetMainWindow()
{
  assert(global_main_window != nullptr);

  return *global_main_window;
}

//static void
//CalibrateSensors() noexcept
//{
//  /* make sure sensord is stopped while calibrating sensors */
//  static constexpr const char *start_sensord[] = {
//    "/bin/systemctl", "start", "sensord.service", nullptr
//  };
//  static constexpr const char *stop_sensord[] = {
//    "/bin/systemctl", "stop", "sensord.service", nullptr
//  };
//
//  RunProcessDialog(UIGlobals::GetMainWindow(),
//                   UIGlobals::GetDialogLook(),
//                   "Calibrate Sensors", stop_sensord,
//                   [](int status){
//                     return status == EXIT_SUCCESS ? mrOK : 0;
//                   });
//
//  AtScopeExit(){
//    RunProcessDialog(UIGlobals::GetMainWindow(),
//                     UIGlobals::GetDialogLook(),
//                     "Calibrate Sensors", start_sensord,
//                     [](int status){
//                       return status == EXIT_SUCCESS ? mrOK : 0;
//                     });
//  };
//
//  /* calibrate the sensors */
//  static constexpr const char *calibrate_sensors[] = {
//    "/opt/bin/sensorcal", "-c", nullptr
//  };
//
//  static constexpr int STATUS_BOARD_NOT_INITIALISED = 2;
//  static constexpr int RESULT_BOARD_NOT_INITIALISED = 100;
//  int result = RunProcessDialog(UIGlobals::GetMainWindow(),
//                                UIGlobals::GetDialogLook(),
//                                "Calibrate Sensors", calibrate_sensors,
//                                [](int status){
//                                  return status == STATUS_BOARD_NOT_INITIALISED
//                                    ? RESULT_BOARD_NOT_INITIALISED
//                                    : 0;
//                                });
//  if (result != RESULT_BOARD_NOT_INITIALISED)
//    return;
//
//  /* initialise the sensors? */
//  if (ShowMessageBox("Sensorboard is virgin. Do you want to initialise it?",
//                     "Calibrate Sensors", MB_YESNO) != IDYES)
//    return;
//
//  static constexpr const char *init_sensors[] = {
//    "/opt/bin/sensorcal", "-i", nullptr
//  };
//
//  result = RunProcessDialog(UIGlobals::GetMainWindow(),
//                            UIGlobals::GetDialogLook(),
//                            "Calibrate Sensors", init_sensors,
//                            [](int status){
//                              return status == EXIT_SUCCESS
//                                ? mrOK
//                                : 0;
//                            });
//  if (result != mrOK)
//    return;
//
//  /* calibrate again */
//  RunProcessDialog(UIGlobals::GetMainWindow(),
//                   UIGlobals::GetDialogLook(),
//                   "Calibrate Sensors", calibrate_sensors,
//                   [](int status){
//                     return status == STATUS_BOARD_NOT_INITIALISED
//                       ? RESULT_BOARD_NOT_INITIALISED
//                       : 0;
//                   });
//}

static int Main(UI::EventQueue &event_queue, UI::SingleWindow &main_window,
     const DialogLook &dialog_look)
{
  TWidgetDialog<MainMenuWidget> dialog(WidgetDialog::Full{}, main_window, dialog_look, "OpenVario");
  dialog.SetWidget(main_window.GetDisplay(), event_queue, dialog);

  return dialog.ShowModal();
}

static int Main()
{
  dialog_settings.SetDefaults();

  ScreenGlobalInit screen_init;

  Display::SetForcedDPI(150, 150);
  Layout::Initialise(screen_init.GetDisplay(), {800, 480});

  InitialiseFonts();

  DialogLook dialog_look;
  dialog_look.Initialise();

  UI::TopWindowStyle main_style;
  main_style.Resizable();
#ifdef MESA_KMS
  main_style.InitialOrientation(DisplayOrientation::PORTRAIT);
#else
  main_style.InitialOrientation(DisplayOrientation::LANDSCAPE);
#endif
  UI::SingleWindow main_window{screen_init.GetDisplay()};
  main_window.Create(_T("XCSoar/KoboMenu"), {600, 800}, main_style);

  main_window.Show();

  global_dialog_look = &dialog_look;
  global_main_window = &main_window;

  int action = Main(screen_init.GetEventQueue(), main_window, dialog_look);

  main_window.Destroy();

  DeinitialiseFonts();

  return action;
}

int main()
{
  EGL::DrmDisplay::DisplayOpenDriDevice();
  int action = Main();

  switch (action) {
  case LAUNCH_SHELL:
    execl("/bin/bash", "bash", "--login", nullptr);
    execl("/bin/ash", "-ash", nullptr);
    execl("/bin/ash", "-sh", nullptr);
    perror("Failed to launch shell");
    return EXIT_FAILURE;
  }

  return action;
}
