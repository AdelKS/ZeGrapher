/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "appsettings.h"

#include <QGuiApplication>
#include <QStyleHints>

void ZeAppSettings::setFont(QFont font)
{
  if (font != this->font)
  {
    this->font = font;
    emit fontChanged();
  }
}

void ZeAppSettings::setDefaultFont(QFont f)
{
  if (font == defaultFont)
    setFont(f);

  defaultFont = f;
}

std::optional<ZeAppSettings::POD> ZeAppSettings::exportPod() const
{
  using zg::yml::not_default;
  POD p {
    .language = not_default(language, English),
    .font = zg::yml::QFontPOD::from(font, defaultFont),
    .valid_syntax = validSyntax.exportPod(defaultValidSyntax),
    .invalid_syntax = invalidSyntax.exportPod(defaultInvalidSyntax),
    .warning_syntax = warningSyntax.exportPod(defaultWarningSyntax),
  };

  if (p)
    return p;
  else return {};
}

void ZeAppSettings::importPod(POD p)
{
  if (p.language and language != *p.language)
  {
    language = *p.language;
    emit languageChanged();
  }

  if (p.font)
  {
    if (p.font->name)
      font.setFamily(QString::fromStdString(*p.font->name));
    if (p.font->weight)
      font.setWeight(*p.font->weight);
    if (p.font->size)
      font.setPointSize(*p.font->size);
    emit fontChanged();
  }

  if (p.valid_syntax)
  {
    validSyntax.importPod(std::move(*p.valid_syntax));
    emit validSyntaxChanged();
  }

  if (p.invalid_syntax)
  {
    invalidSyntax.importPod(std::move(*p.invalid_syntax));
    emit invalidSyntaxChanged();
  }

  if (p.warning_syntax)
  {
    warningSyntax.importPod(std::move(*p.warning_syntax));
    emit warningSyntaxChanged();
  }
}
