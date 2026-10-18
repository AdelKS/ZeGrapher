/****************************************************************************
**  Copyright (c) 2025, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "mathworld.h"
#include "Utils/algos.h"
#include "Utils/yaml.h"

namespace zg {

MathWorld::~MathWorld()
{
  // delete tracked MathObjects here so their value-member Expr destructors
  // can untrack themselves while exprObjects is still alive — otherwise
  // QObject's deleteChildren would run them after our members are gone.
  destroying = true;
  while (not mathObjects.empty())
  {
    auto* m = mathObjects.back();
    mathObjects.pop_back();
    delete m;
  }
}

int MathWorld::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return mathObjects.size();
}

QVariant MathWorld::data(const QModelIndex &index, int role) const
{
  if (not index.isValid() or role != Qt::UserRole)
    return QVariant();

  return QVariant::fromValue(mathObjects.at(index.row()));
}

void MathWorld::removeMathObject(MathObject* obj)
{
  auto it = std::ranges::find(mathObjects, obj);
  if (it == mathObjects.end())
    return;

  int index = it - mathObjects.begin();
  beginRemoveRows(QModelIndex(), index, index);
  obj->deleteLater();

  mathObjects.erase(it);
  endRemoveRows();
  emit updated();
}

void MathWorld::untrackExprObject(const mathobj::Expr* expr)
{
  auto it = std::ranges::find(exprObjects, expr);
  if (it == exprObjects.end())
    return;

  exprObjects.erase(it);
  if (destroying)
    return;

  objectUpdated();
  emit updated();
}

void MathWorld::moveMathObject(int from, int to)
{
  const int n = int(mathObjects.size());
  if (from == to or from < 0 or from >= n or to < 0 or to >= n)
    return;

  // QAbstractItemModel::beginMoveRows expects the destination row index in
  // pre-move coordinates; when moving down, that's one past the target slot.
  const int qmlTo = from < to ? to + 1 : to;
  if (not beginMoveRows(QModelIndex(), from, from, QModelIndex(), qmlTo))
    return;

  utils::move_element(mathObjects, from, to);

  endMoveRows();

  emit updated();
}

std::vector<Plottable> MathWorld::plottables() const
{
  std::vector<Plottable> objects;

  for (const MathObject* o: mathObjects)
    std::visit(zc::utils::overloaded{
      [](mathobj::Constant*) {},
      [&](mathobj::DataSheet* sheet) {
        for (mathobj::Data* column: sheet->getColumns())
          objects.push_back(column);
      },
      [&](auto* b) { objects.push_back(b); },
    }, o->backend);

  return objects;
}

MathObject* MathWorld::addMathObject(MathObject::Type type)
{
  beginInsertRows(QModelIndex(), mathObjects.size(), mathObjects.size());
  auto* obj = new zg::MathObject(this, type);

  mathObjects.emplace_back(obj);

  connect(mathObjects.back(), &MathObject::updated, this, &MathWorld::objectUpdated);
  connect(mathObjects.back(), &MathObject::destroyed, this, &MathWorld::objectUpdated);
  endInsertRows();
  return mathObjects.back();
}

void MathWorld::trackExprObject(mathobj::Expr* o)
{
  exprObjects.push_back(o);
  connect(o, &mathobj::Expr::updated, this, &MathWorld::objectUpdated);
}

void MathWorld::objectUpdated()
{
  if (destroying or syncing)
    return;

  syncing = true;

  for (auto* f: mathObjects)
    f->sync();

  for (auto* f: exprObjects)
    f->sync();

  syncing = false;

  updateSchrodingerStatus();

  emit updated();
}

QHash<int, QByteArray> MathWorld::roleNames() const
{
  // views read DisplayRole as text, so the pointer gets its own role
  return { {Qt::UserRole, "mathObjPtr"} };
}

void MathWorld::setSchrodingerConstant(zg::mathobj::Constant* c)
{
  if (schrodingerConstant == c)
    return;

  if (zg::mathobj::Constant* old = std::exchange(schrodingerConstant, c))
    old->schrodingerSwitched();

  if (c)
    c->schrodingerSwitched();

  updateSchrodingerStatus();

  emit schrodingerConstantChanged();
}

void MathWorld::updateSchrodingerStatus()
{
  if (destroying)
    return;

  std::unordered_set<const zc::DynMathObject<zc_t>*> schrodingerBackends;
  if (schrodingerConstant)
  {
    schrodingerBackends = revdeps(&schrodingerConstant->zcMathObj);
    schrodingerBackends.insert(&schrodingerConstant->zcMathObj);
  }

  auto inSet = [&](EvalHandle h) -> bool {
    return std::visit(
      zc::utils::overloaded{
        [&](const zc::DynMathObject<zc_t>* p) { return schrodingerBackends.contains(p); },
        [&](std::pair<const zc::DynMathObject<zc_t>*, const zc::DynMathObject<zc_t>*> p) {
          return schrodingerBackends.contains(p.first) or schrodingerBackends.contains(p.second);
        }
      },
      h
    );
  };

  for (Plottable p: plottables())
    std::visit([&](auto* o) { o->setSchrodinger(inSet(o->getZcObject())); }, p);

  for (mathobj::Expr* r: exprObjects)
    r->setSchrodinger(schrodingerBackends.contains(&r->zcMathObj));
}

std::unordered_set<const zc::DynMathObject<zc_t>*>
MathWorld::direct_revdeps(EvalHandle handle)
{
  std::unordered_set<std::string> names;

  std::visit(
    zc::utils::overloaded{
      [&](const zc::DynMathObject<zc_t>* p){
        for (auto&& [name, t]: backend.direct_revdeps(p->get_name())) names.insert(name);
      },
      [&](std::pair<const zc::DynMathObject<zc_t>*, const zc::DynMathObject<zc_t>*> p) {
        for (auto&& [name, _]: backend.direct_revdeps(p.first->get_name())) names.insert(name);
        for (auto&& [name, _]: backend.direct_revdeps(p.second->get_name())) names.insert(name);
      }
    },
    handle
  );

  std::unordered_set<const zc::DynMathObject<zc_t>*> res;
  for (const zc::DynMathObject<zc_t>& b: backend)
    if (names.contains(std::string(b.get_name())))
      res.insert(&b);
  return res;
}

std::unordered_set<const zc::DynMathObject<zc_t>*>
MathWorld::revdeps(EvalHandle handle)
{
  auto to_explore = direct_revdeps(handle);
  std::unordered_set<const zc::DynMathObject<zc_t>*> res, explored;

  while (not to_explore.empty())
  {
    const zc::DynMathObject<zc_t>* d = *to_explore.begin();
    to_explore.erase(to_explore.begin());

    explored.insert(d);
    res.insert(d);
    auto new_to_explore = direct_revdeps(EvalHandle(d));
    for (auto* e: new_to_explore)
      if (not explored.contains(e))
        to_explore.insert(e);
  }

  return res;
}

MathWorld::POD MathWorld::exportPod() const
{
  MathWorld::POD math_objects;
  math_objects.reserve(mathObjects.size());
  for (const MathObject* o: mathObjects)
    math_objects.push_back(o->exportPod());
  return math_objects;
}

void MathWorld::importPod(POD math_objects)
{
  // MathObject::POD lists its alternatives in MathObject::Type order
  using T = MathObject::POD;
  static_assert(std::is_same_v<std::variant_alternative_t<MathObject::EQUATION, T>, mathobj::Equation::POD>
                and std::is_same_v<std::variant_alternative_t<MathObject::CONSTANT, T>, mathobj::Constant::POD>
                and std::is_same_v<std::variant_alternative_t<MathObject::PARAMETRIC, T>, mathobj::Parametric::POD>
                and std::is_same_v<std::variant_alternative_t<MathObject::DATASHEET, T>, mathobj::DataSheet::POD>);

  for (MathObject::POD& o: math_objects)
    addMathObject(MathObject::Type(o.index()))->importPod(std::move(o));
}

}
