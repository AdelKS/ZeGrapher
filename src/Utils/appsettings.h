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
#pragma once

#include <QtQmlIntegration/qqmlintegration.h>
#include <QObject>
#include <QLocale>
#include <QFont>
#include <QColor>
#include <QGuiApplication>

#include "Utils/yaml.h"
#include "themedcolor.h"

struct ZeAppSettings: QObject
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(Language language MEMBER language NOTIFY languageChanged)
  Q_PROPERTY(QFont font MEMBER font WRITE setFont NOTIFY fontChanged)
  Q_PROPERTY(ThemedColor validSyntax MEMBER validSyntax NOTIFY validSyntaxChanged)
  Q_PROPERTY(ThemedColor invalidSyntax MEMBER invalidSyntax NOTIFY invalidSyntaxChanged)
  Q_PROPERTY(ThemedColor warningSyntax MEMBER warningSyntax NOTIFY warningSyntaxChanged)

public:

  enum Language {
    English = QLocale::Language::English,
    French = QLocale::Language::French
  };
  Q_ENUM(Language);

  explicit ZeAppSettings(QObject* parent = nullptr): QObject(parent) {}

  void setDefaultFont(QFont);

  Language language = English;
  QFont font;

  ThemedColor validSyntax = defaultValidSyntax;
  ThemedColor invalidSyntax = defaultInvalidSyntax;
  ThemedColor warningSyntax = defaultWarningSyntax;

  QFont defaultFont = qGuiApp->font();

  const static ThemedColor defaultValidSyntax;
  const static ThemedColor defaultInvalidSyntax;
  const static ThemedColor defaultWarningSyntax;

  Q_INVOKABLE void setFont(QFont);

  struct POD {
    std::optional<Language> language;
    std::optional<zg::yml::QFontPOD> font;
    std::optional<ThemedColor::POD> valid_syntax;
    std::optional<ThemedColor::POD> invalid_syntax;
    std::optional<ThemedColor::POD> warning_syntax;

    operator bool () const { return language or font or valid_syntax or invalid_syntax or warning_syntax; }
  };

  std::optional<POD> exportPod() const;
  void importPod(POD);

signals:
  void validSyntaxChanged();
  void invalidSyntaxChanged();
  void warningSyntaxChanged();
  void fontChanged();
  void languageChanged();

};

inline const ThemedColor ZeAppSettings::defaultValidSyntax = {.dark = "#009999", .light = "#009999"};
inline const ThemedColor ZeAppSettings::defaultInvalidSyntax = {.dark = Qt::darkRed, .light = Qt::darkRed};
inline const ThemedColor ZeAppSettings::defaultWarningSyntax = {.dark = Qt::darkYellow, .light = Qt::darkYellow};

template <>
struct glz::meta<ZeAppSettings::Language>
{
   using enum ZeAppSettings::Language;
   static constexpr auto value = glz::enumerate(
    "english", English,
    "french", French);
};
