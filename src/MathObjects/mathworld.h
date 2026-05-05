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

#include <QtQmlIntegration/qqmlintegration.h>

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
  QML_ANONYMOUS

  Q_PROPERTY(zg::MathObject* schrodingerConstant READ getSchrodingerConstant NOTIFY schrodingerConstantChanged)

public:
  MathWorld(QObject *parent = nullptr): QAbstractListModel(parent) {}

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  const auto& getMathObjects() const { return mathObjects; }
  auto& getStyles() { return styles; }
  Q_INVOKABLE void removeMathObject(MathObject*);
  Q_INVOKABLE MathObject* addMathObject(MathObject::Type type = MathObject::EQUATION);
  Q_INVOKABLE void moveMathObject(int from, int to);

  Q_INVOKABLE void setSchrodingerConstant(zg::MathObject* c);
  Q_INVOKABLE void unsetSchrodingerConstant(zg::MathObject* c);
  zg::MathObject* getSchrodingerConstant() { return schrodingerConstant; }

  /// @brief add Expr object that is not part of the model because its widget is elsewhere
  Q_INVOKABLE mathobj::Expr* addAltExprObject();
  Q_INVOKABLE void removeAltExprObject(mathobj::Expr*);

  std::unordered_set<MathObject*> direct_revdeps(MathObject&) const;
  std::unordered_set<MathObject*> revdeps(MathObject&) const;

  QHash<int, QByteArray> roleNames() const override;

signals:
  void updated();
  void schrodingerConstantChanged();

protected slots:
  void objectUpdated();
  void updateSchrodingerStatus();

protected:
  std::vector<zg::MathObject*> mathObjects;
  std::unordered_map<const zg::MathObject*, PlotStyle*> styles;

  /// @brief the single constant currently driving simultaneous plotting, or nullptr
  zg::MathObject* schrodingerConstant = nullptr;

  /// @brief math objects that aren't part of the model (that the view represents)
  std::vector<zg::MathObject*> altMathObjects;
  bool syncing = false;
};

} // namespace zg
