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

#include <QtQmlIntegration/qqmlintegration.h>

#include "Utils/qmlsingleton.h"

namespace zg {

namespace mathobj {
  struct NamedRef;

namespace shared {
  struct ZcMathObjectBB;
}}

/// @brief a backend that draws one curve. A sheet draws one per column
using Plottable = std::variant<mathobj::Equation*, mathobj::Parametric*, mathobj::Data*>;

static_assert(PlottableBackend<mathobj::Equation> and PlottableBackend<mathobj::Parametric>
              and PlottableBackend<mathobj::Data>);

class MathWorld: public QAbstractListModel
{
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(zg::mathobj::Constant* schrodingerConstant READ getSchrodingerConstant NOTIFY schrodingerConstantChanged)

public:
  /// @brief QML uses the global instance instead of creating one
  static MathWorld* create(QQmlEngine*, QJSEngine*);

  explicit MathWorld(QObject *parent): QAbstractListModel(parent) {}
  ~MathWorld() override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  const auto& getMathObjects() const { return mathObjects; }

  /// @brief the backends that draw a curve, in list order, with each sheet
  ///        replaced by its columns
  std::vector<Plottable> plottables() const;

  Q_INVOKABLE void removeMathObject(zg::MathObject*);
  Q_INVOKABLE zg::MathObject* addMathObject(zg::MathObject::Type type = MathObject::EQUATION);
  Q_INVOKABLE void moveMathObject(int from, int to);

  /// @brief the constant that takes all its values at once, nullptr for none
  Q_INVOKABLE void setSchrodingerConstant(zg::mathobj::Constant* c);
  zg::mathobj::Constant* getSchrodingerConstant() { return schrodingerConstant; }

  /// @brief syncs every object again: their error messages follow the app
  ///        language
  void retranslate();

  /// @brief add Expr object that is not part of the model because its widget is elsewhere
  void trackExprObject(mathobj::Expr*);
  void untrackExprObject(const mathobj::Expr*);

  QHash<int, QByteArray> roleNames() const override;

  using POD = std::vector<MathObject::POD>;

  POD exportPod() const;
  void importPod(POD);

  /// @brief the sequence values cached across evaluations
  zc::eval::Cache cache;

signals:
  void updated();
  void schrodingerConstantChanged();

protected slots:
  void objectUpdated();
  void updateSchrodingerStatus();

protected:
  /// @brief the objects whose expressions use 'o', directly or not
  std::unordered_set<const zc::DynMathObject<zc_t>*> revdeps(const zc::DynMathObject<zc_t>* o);
  std::unordered_set<const zc::DynMathObject<zc_t>*> direct_revdeps(const zc::DynMathObject<zc_t>* o);

  zc::MathWorld<zc_t> backend;

  std::vector<zg::MathObject*> mathObjects;

  /// @brief the single constant currently driving simultaneous plotting, or nullptr
  zg::mathobj::Constant* schrodingerConstant = nullptr;

  /// @brief math objects that aren't part of the model (that the view represents)
  std::vector<zg::mathobj::Expr*> exprObjects;
  bool syncing = false;
  bool destroying = false;

  friend struct mathobj::shared::ZcMathObjectBB;
  friend struct mathobj::NamedRef;
};

/// @brief set by Globals
inline MathWorld* mathWorld = nullptr;

inline MathWorld* MathWorld::create(QQmlEngine*, QJSEngine*)
{
  return qmlSingleton(mathWorld);
}

} // namespace zg
