#pragma once

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

#include "Utils/state.h"
#include <zecalculator/zecalculator.h>

namespace zg {
namespace mathobj {

/// @brief holds the names of the two objects used to plot a parametric equation
struct Parametric: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(State* firstState  MEMBER firstState)
  Q_PROPERTY(State* secondState MEMBER secondState)
  Q_PROPERTY(QString firstName  WRITE setFirstName MEMBER firstName)
  Q_PROPERTY(QString secondName WRITE setSecondName MEMBER secondName)
  Q_PROPERTY(double start MEMBER start NOTIFY startChanged)
  Q_PROPERTY(double end MEMBER end NOTIFY endChanged)
  Q_PROPERTY(double step MEMBER step NOTIFY stepChanged)
  Q_PROPERTY(bool cartesian MEMBER cartesian NOTIFY cartesianChanged)
  Q_PROPERTY(bool continuous MEMBER continuous NOTIFY continuousChanged)

public:

  explicit Parametric(QObject *parent = nullptr);

  void setFirstName(QString input_name);
  void setSecondName(QString input_name);

  QString getFirstName() const { return firstName; };
  QString getSecondName()  const { return secondName; };

  void setSlot(size_t slot) { this->slot = slot; }

  double start = 0;
  double end = 0;
  double step = 1;
  bool continuous = true;
  bool cartesian = true;

public slots:
  void refresh();

signals:
  void startChanged();
  void endChanged();
  void stepChanged();
  void continuousChanged();
  void cartesianChanged();

protected:
  QString inputFirstName, inputSecondName;
  tl::expected<zc::parsing::LHS, zc::Error> firstNameLHS = tl::unexpected(zc::Error::empty_expression());
  tl::expected<zc::parsing::LHS, zc::Error> secondNameLHS = tl::unexpected(zc::Error::empty_expression());
  QString firstName, secondName;
  State* firstState = nullptr;
  State* secondState = nullptr;
  std::optional<size_t> slot;
};

}
}
