/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2012 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#ifndef XCSOAR_SCREEN_FONT_HPP
#define XCSOAR_SCREEN_FONT_HPP

#include "Screen/Point.hpp"
#include "Util/NonCopyable.hpp"
#include "Compiler.h"

#ifdef ANDROID
#include "Screen/Color.hpp"
#else  // !ANDROID
#ifdef ENABLE_SDL
#include <SDL_ttf.h>
#endif
#endif // !ANDROID

#ifdef WIN32
#include <windows.h>
#else
#include <wingdi.h>
#endif

#include <tchar.h>

class TextUtil;

/**
 * A font loaded from storage.  It is used by #Canvas to draw text.
 */
class Font : private NonCopyable {
protected:
#ifdef ANDROID
  TextUtil *text_util_object;

  unsigned line_spacing;
#elif defined(USE_GDI)
  HFONT font;
#else
  TTF_Font *font;
#endif

  unsigned height, ascent_height, capital_height;

  void CalculateHeights();

#ifdef ENABLE_SDL
  bool LoadFile(const char *file, UPixelScalar ptsize, bool bold = false,
                bool italic = false);
#endif

public:
#ifdef ANDROID
  Font():text_util_object(NULL) {}
#else
  Font():font(NULL) {}
#endif

  ~Font() { Destroy(); }

  /**
   * Perform global font initialisation.
   */
  static void Initialise();

public:
  bool
  IsDefined() const {
    #ifdef ANDROID
    return text_util_object != NULL;
    #else
    return font != NULL;
    #endif
  }

  bool Load(const TCHAR *facename, UPixelScalar height, bool bold = false,
            bool italic = false);
  bool Load(const LOGFONT &log_font);
  void Destroy();

  gcc_pure
  PixelSize TextSize(const TCHAR *text) const;

#ifdef ANDROID
  int TextTextureGL(const TCHAR *text, PixelSize &size) const;
#elif defined(USE_GDI)
  HFONT Native() const {
    return font;
  }
#else
  TTF_Font*
  Native() const {
    return font;
  }
#endif

  UPixelScalar GetHeight() const {
    return height;
  }
  UPixelScalar GetAscentHeight() const {
    return ascent_height;
  }
  UPixelScalar GetCapitalHeight() const {
    return capital_height;
  }

#ifdef ANDROID
  UPixelScalar GetLineSpacing() const {
    return line_spacing;
  }
#elif defined(ENABLE_SDL)
  gcc_pure
  UPixelScalar GetLineSpacing() const {
    return ::TTF_FontLineSkip(font);
  }
#endif
};

#endif
