// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright The XCSoar Project

#pragma once

#include "InfoBoxes/Content/Base.hpp"

class InfoBoxContentMacCready : public InfoBoxContent
{
public:
  const InfoBoxPanel *GetDialogContent() noexcept override;
  void Update(InfoBoxData &data) noexcept override;
};
