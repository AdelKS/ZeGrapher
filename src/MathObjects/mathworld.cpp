#include "mathworld.h"

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
  if (role != Qt::DisplayRole)
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

  std::swap(mathObjects[from], mathObjects[to]);

  endMoveRows();

  emit updated();
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
  return { {Qt::DisplayRole, "mathObjPtr"} };
}

void MathWorld::setSchrodingerConstant(zg::mathobj::Constant* c)
{
  if (schrodingerConstant == c)
    return;

  // make any eventual already shrodinger constant lose its status
  if (schrodingerConstant)
    schrodingerConstant->setDeadAndAlive(false);

  schrodingerConstant = c;

  updateSchrodingerStatus();

  emit schrodingerConstantChanged();
}

void MathWorld::unsetSchrodingerConstant(zg::mathobj::Constant* c)
{
  if (schrodingerConstant != c or c == nullptr)
    return;

  schrodingerConstant = nullptr;

  updateSchrodingerStatus();

  emit schrodingerConstantChanged();
  emit updated();
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

  auto inSet = [&](MathObject::EvalHandle h) -> bool {
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

  for (auto* r: mathObjects)
    r->setSchrodinger(inSet(r->getZcObject()));

  for (mathobj::Expr* r: exprObjects)
    r->setSchrodinger(schrodingerBackends.contains(&r->zcMathObj));
}

std::unordered_set<const zc::DynMathObject<zc_t>*>
MathWorld::direct_revdeps(MathObject::EvalHandle handle) const
{
  std::unordered_set<std::string> names;

  std::visit(
    zc::utils::overloaded{
      [&](const zc::DynMathObject<zc_t>* p){
        for (auto&& [name, t]: zc::mathWorld.direct_revdeps(p->get_name())) names.insert(name);
      },
      [&](std::pair<const zc::DynMathObject<zc_t>*, const zc::DynMathObject<zc_t>*> p) {
        for (auto&& [name, _]: zc::mathWorld.direct_revdeps(p.first->get_name())) names.insert(name);
        for (auto&& [name, _]: zc::mathWorld.direct_revdeps(p.second->get_name())) names.insert(name);
      }
    },
    handle
  );

  std::unordered_set<const zc::DynMathObject<zc_t>*> res;
  for (zc::DynMathObject<zc_t>& b: zc::mathWorld)
    if (names.contains(std::string(b.get_name())))
      res.insert(&b);
  return res;
}

std::unordered_set<const zc::DynMathObject<zc_t>*>
MathWorld::revdeps(MathObject::EvalHandle handle) const
{
  auto to_explore = direct_revdeps(handle);
  std::unordered_set<const zc::DynMathObject<zc_t>*> res, explored;

  while (not to_explore.empty())
  {
    const zc::DynMathObject<zc_t>* d = *to_explore.begin();
    to_explore.erase(to_explore.begin());

    explored.insert(d);
    res.insert(d);
    auto new_to_explore = direct_revdeps(MathObject::EvalHandle(d));
    for (auto* e: new_to_explore)
      if (not explored.contains(e))
        to_explore.insert(e);
  }

  return res;
}

}
