// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright The XCSoar Project

#pragma once

#include "Widget.hpp"
#include "ui/dim/Rect.hpp"

#include <memory>
#include <utility>

/**
 * A #Widget that contains two other widgets, the second one following
 * below the first one.  The two #Widget pointers are deleted in the
 * destructor.
 *
 * If you construct this object with vertical=false, the second Widget
 * will be right of the first one.
 */
class TwoWidgets : public NullWidget {
  const bool vertical;

  const std::unique_ptr<Widget> first, second;

  PixelRect rc;

public:
  TwoWidgets(std::unique_ptr<Widget> &&_first, std::unique_ptr<Widget> &&_second, bool _vertical=true) noexcept
    :vertical(_vertical),
     first(std::move(_first)),
     second(std::move(_second)) {}

  /**
   * Update the layout after one of the widgets has indicated a size
   * change.  This may only be called between Prepare() and
   * Unprepare().
   */
  void UpdateLayout() noexcept;

  Widget &GetFirst() noexcept {
    return *first;
  }

  const Widget &GetFirst() const noexcept {
    return *first;
  }

  Widget &GetSecond() noexcept {
    return *second;
  }

  const Widget &GetSecond() const noexcept {
    return *second;
  }

protected:
  [[gnu::pure]]
  int CalculateSplit(const PixelRect &rc) const noexcept;

  [[gnu::pure]]
  std::pair<PixelRect,PixelRect> CalculateLayout(const PixelRect &rc) const noexcept;

public:
  /* virtual methods from Widget */
  PixelSize GetMinimumSize() const noexcept override;
  PixelSize GetMaximumSize() const noexcept override;
  void Initialise(ContainerWindow &parent, const PixelRect &rc) noexcept override;
  void Prepare(ContainerWindow &parent, const PixelRect &rc) noexcept override;
  void Unprepare() noexcept override;
  bool Save(bool &changed) noexcept override;
  bool Click() noexcept override;
  void ReClick() noexcept override;
  void Show(const PixelRect &rc) noexcept override;
  bool Leave() noexcept override;
  void Hide() noexcept override;
  void Move(const PixelRect &rc) noexcept override;
  bool SetFocus() noexcept override;
  bool HasFocus() const noexcept override;
  bool KeyPress(unsigned key_code) noexcept override;
};
