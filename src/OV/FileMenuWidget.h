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
  WndForm &dialog;
public:
  FileMenuWidget(UI::Display &_display, UI::EventQueue &_event_queue, const DialogLook &look, WndForm &_dialog) noexcept
    :RowFormWidget(look),
     display(_display), event_queue(_event_queue), dialog(_dialog) {}
    static void loadIGCToSDCard();
private:
  /* virtual methods from class Widget */
  void Prepare(ContainerWindow &parent, const PixelRect &rc) noexcept override;



void Show(const PixelRect &rc) noexcept override {
  HideRow(3);
  RowFormWidget::Show(rc);
}

void Hide() noexcept override
{

    RowFormWidget::Hide();
  }

  bool KeyPress(unsigned key_code) noexcept override
  {
    HideRow(3);
    return RowFormWidget::KeyPress(key_code);
  }
};
#endif /* SRC_OV_FILEMENUWIDGET_H_ */
