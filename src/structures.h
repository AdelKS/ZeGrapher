/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "zecalculator/parsing/types.h"
#include <cfloat>
#include <glaze/yaml.hpp>

#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QLocale>
#include <QMetaType>
#include <QPointF>
#include <QSizeF>
#include <QtQmlIntegration/qqmlintegration.h>

#define NORMAL 0
#define ZOOMER 1
#define DEZOOMER 2
#define ZOOMBOX 3
#define DEPLACER 4
#define NO_CURSOR 5

#define MIN_AMPLITUDE 100 * DBL_EPSILON
#define MAX_AMPLITUDE DBL_MAX / 100

#define NUM_PREC 7 // default precision while displaying decimal numbers
#define MAX_NUM_PREC 8 // the maximum precision in decimal come from imprecision on integration and derivation

#define MAX_NUM_PREC_DEC 1E-8

#define RELATIVE_MIN_SIZE 0.25 // How much the graph can be smaller than its container sheet/background

#define CM_PER_INCH 2.54

/// @brief type of math objects to use in ZeCalculator
inline constexpr zc::parsing::Type zc_t = zc::parsing::Type::RPN;

/// @brief the two letter code of a language, "fr" for French
inline QString langToShortString(QLocale::Language lang)
{
  return QLocale::languageToCode(lang);
}

/// @brief the whole content of a file, empty when the file cannot be read
inline QString readTextFile(const QString& path)
{
  QFile file(path);
  if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    return {};

  return QString::fromUtf8(file.readAll());
}

/// @brief ':/website/<lang>', or ':/website/en' when the build embedded no such folder
/// @note website/meson.build embeds the manual, which the app and the site share
inline QString websiteFolder(QLocale::Language lang)
{
  const QString folder = ":/website/" + langToShortString(lang);
  return QDir(folder).exists() ? folder : ":/website/en";
}

/// @brief every language the app can show, English first
/// @note reads the translations the build embedded, so adding a language is
///       adding its .ts file: nothing here lists them
inline const QList<QLocale::Language>& supportedLangs()
{
  static const QList<QLocale::Language> langs = []
  {
    // English needs no translation file: it is what the source code holds
    QList<QLocale::Language> found = {QLocale::English};

    for (const QString& file: QDir(":/translations").entryList({"ZeGrapher_*.qm"}, QDir::Files))
    {
      const QString code = QFileInfo(file).completeBaseName().section('_', 1);
      if (const auto lang = QLocale::codeToLanguage(code); lang != QLocale::AnyLanguage)
        found.append(lang);
    }

    return found;
  }();

  return langs;
}

/// @brief the OS language if the app can show it, English otherwise
inline QLocale::Language systemLanguage()
{
  const auto lang = QLocale::system().language();
  return supportedLangs().contains(lang) ? lang : QLocale::English;
}

/// @brief the name of a language in that language, "Français" for French
inline QString langToNativeName(QLocale::Language lang)
{
  // QLocale has no name for a bare language: it picks a territory, and the name
  // of the locale it lands on can carry that territory. English comes out as
  // "American English" and Spanish as "español de España". The languages below
  // are the ones where it does.
  static const QHash<QLocale::Language, QString> withoutTerritory {
    {QLocale::English, "English"},
    {QLocale::Spanish, "Español"},
  };

  if (const auto it = withoutTerritory.constFind(lang); it != withoutTerritory.constEnd())
    return *it;

  QString name = QLocale(lang).nativeLanguageName();
  if (not name.isEmpty())
    name[0] = name[0].toUpper();

  return name;
}

enum struct ZeAxisName {X, Y};

struct ZeCoordinateDisplayFormat
{
    bool decimalGlobalConstant, decimalBase;

    bool operator !=(const ZeCoordinateDisplayFormat &other)
    {
        return decimalGlobalConstant != other.decimalGlobalConstant or
                decimalBase != other.decimalBase;
    }
};


struct SizeUnit: QObject
{
  Q_OBJECT
  QML_ELEMENT

public:
  SizeUnit(QObject *parent = nullptr): QObject(parent) {}

  enum Unit {PIXEL, CENTIMETER};
  Q_ENUM(Unit)
};

struct ZeSizeSettings
{
  Q_GADGET

  Q_PROPERTY(SizeUnit::Unit sizeUnit MEMBER sizeUnit)
  Q_PROPERTY(bool sheetFillsWindow MEMBER sheetFillsWindow)
  Q_PROPERTY(double scalingFactor MEMBER scalingFactor)
  Q_PROPERTY(QSize pxSheetSize MEMBER pxSheetSize)
  Q_PROPERTY(QSizeF cmSheetSize MEMBER cmSheetSize)


public:

  SizeUnit::Unit sizeUnit = SizeUnit::PIXEL;
  bool sheetFillsWindow = true;

  double scalingFactor = 1.0;

  QSize pxSheetSize;
  QSizeF cmSheetSize;

  bool operator == (const ZeSizeSettings &other) const = default;

  struct POD
  {
    std::optional<double> scaling;
    std::optional<SizeUnit::Unit> unit;
    std::optional<double> width;
    std::optional<double> height;

    operator bool () const { return scaling or unit or width or height; }
  };

  std::optional<POD> exportPod() const;
  void importPod(POD);
};

template <>
struct glz::meta<SizeUnit::Unit>
{
   using enum SizeUnit::Unit;
   static constexpr auto value = glz::enumerate(
    "centimeter", CENTIMETER,
    "pixel", PIXEL);
};

struct ZoomingType: QObject
{
  Q_OBJECT
  QML_ELEMENT

public:
  ZoomingType(QObject *parent = nullptr): QObject(parent) {}

  enum Type {FITSHEET, CUSTOM};
  Q_ENUM(Type)
};

struct ZeZoomSettings
{
  Q_GADGET
  Q_PROPERTY(ZoomingType::Type zoomingType MEMBER zoomingType)
  Q_PROPERTY(double zoom MEMBER zoom)
  QML_VALUE_TYPE(zeZoomSettings)

public:

  ZoomingType::Type zoomingType = ZoomingType::FITSHEET;
  double zoom = 1.0;

  bool operator == (const ZeZoomSettings &other) const = default;
};

struct Range
{
    Range() : start(0), end(0), step(1) {}
    double start, end, step;
};

struct Point
{
    double x, y;

    bool operator<(const Point &b) const
    {
        return x < b.x;
    }

    operator QPointF() const
    {
        return QPointF(x, y);
    }
};

bool ptCompY(const Point &pt1, const Point &pt2);

template<class Number>
Number int_pow(Number a, int b)
{
    Number res = 1;
    if(b < 0)
    {
        for(int i = 0 ; i > b ; i--)
            res /= a;
    }
    else
    {
        for(int i = 0 ; i < b ; i++)
            res *= a;
    }

    return res;
}

Point operator * (const Point &pt1, const Point &pt2);
