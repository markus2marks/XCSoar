// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright The XCSoar Project

#include "RowFormWidget.hpp"
#include "Form/DataField/File.hpp"
#include "system/Path.hpp"

#include <cassert>

Path
RowFormWidget::GetValueFile(unsigned i) const noexcept
{
  const auto &df = (const FileDataField &)GetDataField(i);
  assert(df.GetType() == DataField::Type::FILE);
  return df.GetValue();
}
