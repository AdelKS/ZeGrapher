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

#include <QtQmlIntegration>
#include <QObject>
#include <QSyntaxHighlighter>

#include "Utils/state.h"

namespace zg {

/// @brief Contains the information needed to compute the math object and how to plot it
struct Highlighted: QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("Abstract class only used to cast to a common parent")

  Q_PROPERTY(QSyntaxHighlighter* highlighter MEMBER highlighter)
  Q_PROPERTY(State state MEMBER state NOTIFY stateChanged)

public:
  Highlighted(QObject* parent): QObject(parent) {}
  virtual ~Highlighted() {};

  Q_INVOKABLE virtual State setExpression(QString) = 0;

  State getState() const { return state; }

  QSyntaxHighlighter* highlighter = nullptr;

signals:
  void stateChanged();
  void expressionChangedByBackend(QString);

protected:
  State state;
  bool doNotRehighlight = false;
};

} // namespace zg
