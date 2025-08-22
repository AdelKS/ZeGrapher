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

#include "MathObjects/mathobject.h"

#include <QAbstractListModel>

#include <QtQml/qqmlregistration.h>

namespace zc {
  /// @note we need this declaration of global variable be before the ZG one
  ///       so destruction order works well: zg::MathObject needs to register itself
  ///       from this mathworld backend during its destruction
  inline MathWorld<zc_t> mathWorld;
}

namespace zg {

class MathWorld: public QAbstractListModel
{
  Q_OBJECT
  QML_ELEMENT

public:
  MathWorld(QObject *parent = nullptr): QAbstractListModel(parent) {}

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  const auto& getMathObjects() const { return mathObjects; }
  Q_INVOKABLE void removeMathObject(MathObject*);
  Q_INVOKABLE MathObject* addMathObject(MathObject::Type type = MathObject::EQUATION);

  /// @brief add math object but that is not part of the list
  /// @note because it's used in separate widgets
  /// @note but we still need to forward sync requests so we track them here
  Q_INVOKABLE MathObject* addAltMathObject(MathObject::Type type);

  QHash<int, QByteArray> roleNames() const override;

signals:
  void updated();

protected slots:
  void objectUpdated(zg::MathObject*);

protected:
  std::vector<zg::MathObject*> mathObjects;

  /// @brief math objects that aren't part of the model (that the view represents)
  std::vector<zg::MathObject*> altMathObjects;
  bool syncing = false;
};

inline MathWorld mathWorld;

/// @brief register the 'information' global variable with QML
/// @note  see https://doc.qt.io/qt-6/qml-singleton.html#exposing-an-existing-object-as-a-singleton

struct MathWorldForeign
{
  Q_GADGET
  QML_FOREIGN(MathWorld)
  QML_SINGLETON
  QML_NAMED_ELEMENT(MathWorld)

public:

  static MathWorld *create(QQmlEngine *, QJSEngine *engine)
  {
    // The engine has to have the same thread affinity as the singleton.
    Q_ASSERT(engine->thread() == mathWorld.thread());

    // There can only be one engine accessing the singleton.
    if (s_engine)
      Q_ASSERT(engine == s_engine);
    else
      s_engine = engine;

    // Explicitly specify C++ ownership so that the engine doesn't delete
    // the instance.
    QJSEngine::setObjectOwnership(&mathWorld,
                                  QJSEngine::CppOwnership);
    return &mathWorld;
  }

private:
  inline static QJSEngine *s_engine = nullptr;
};

} // namespace zg
