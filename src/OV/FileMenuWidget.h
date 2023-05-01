/*
 * FileMenuWidget.h
 *
 *  Created on: May 1, 2023
 *      Author: markus
 */

#ifndef SRC_OV_FILEMENUWIDGET_H_
#define SRC_OV_FILEMENUWIDGET_H_

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

class FileMenuWidget final : public RowFormWidget
{
  UI::Display &display;
  UI::EventQueue &event_queue;

public:
  FileMenuWidget(UI::Display &_display, UI::EventQueue &_event_queue,
                 const DialogLook &look) noexcept
    :RowFormWidget(look),
     display(_display), event_queue(_event_queue) {}

private:
  /* virtual methods from class Widget */
  void Prepare(ContainerWindow &parent, const PixelRect &rc) noexcept override;
};



#endif /* SRC_OV_FILEMENUWIDGET_H_ */
