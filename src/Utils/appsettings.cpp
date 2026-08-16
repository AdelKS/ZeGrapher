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
#include "structures.h"

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

QVariantList ZeAppSettings::languages() const
{
  QVariantList list;

  for (QLocale::Language lang: supportedLangs())
    list.append(QVariantMap{{"text", langToNativeName(lang)}, {"value", int(lang)}});

  return list;
}

std::optional<ZeAppSettings::POD> ZeAppSettings::exportPod() const
{
  using zg::yml::not_default;
  POD p {
    .language = not_default(langToShortString(QLocale::Language(language)),
                            langToShortString(systemLanguage())),
    .font = zg::yml::QFontPOD::from(font, defaultFont),
    .window_size = zg::yml::QSizePOD::from(windowSize, defaultWindowSize),
    .pane_width = not_default(paneWidth, defaultPaneWidth),
    .csv_pane_width = not_default(csvPaneWidth, defaultCsvPaneWidth),
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
  if (p.language)
  {
    // an unknown code, from an older ZeGrapher or a hand written file, leaves
    // the language the app already picked from the system
    const auto lang = QLocale::codeToLanguage(QString::fromStdString(*p.language));

    if (lang != QLocale::AnyLanguage and language != int(lang))
    {
      language = lang;
      emit languageChanged();
    }
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

  if (p.window_size)
  {
    if (p.window_size->width)
      windowSize.setWidth(*p.window_size->width);
    if (p.window_size->height)
      windowSize.setHeight(*p.window_size->height);
    emit windowSizeChanged();
  }

  if (p.pane_width and paneWidth != *p.pane_width)
  {
    paneWidth = *p.pane_width;
    emit paneWidthChanged();
  }

  if (p.csv_pane_width and csvPaneWidth != *p.csv_pane_width)
  {
    csvPaneWidth = *p.csv_pane_width;
    emit csvPaneWidthChanged();
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
