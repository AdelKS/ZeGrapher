#include "mathworld.h"

namespace zg {

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

  QVariant var;
  var.setValue(mathObjects.at(index.row()).first);

  return var;
}

void MathWorld::removeMathObject(MathObject* obj)
{
  auto it = std::ranges::find(mathObjects, obj, &std::pair<zg::MathObject*, PlotStyle*>::first);
  if (it == mathObjects.end())
    return;

  int index = it - mathObjects.begin();
  beginRemoveRows(QModelIndex(), index, index);
  obj->deleteLater();
  mathObjects.erase(it);
  endRemoveRows();
}

void MathWorld::removeAltExprObject(mathobj::Expr* expr)
{
  auto it = std::ranges::find_if(altMathObjects, [&](MathObject* obj){ return obj->getExpr() == expr; });
  if (it == altMathObjects.end())
    return;

  (*it)->deleteLater();
  altMathObjects.erase(it);
}

void MathWorld::attachStyle(MathObject* obj, PlotStyle* style)
{
  auto it = std::ranges::find(mathObjects, obj, &std::pair<zg::MathObject*, PlotStyle*>::first);
  if (it == mathObjects.end())
    return;

  it->second = style;
}

MathObject* MathWorld::addMathObject(MathObject::Type type)
{
  beginInsertRows(QModelIndex(), mathObjects.size(), mathObjects.size());
  mathObjects.emplace_back(std::make_pair(new zg::MathObject(this, type), static_cast<PlotStyle*>(nullptr)));
  connect(mathObjects.back().first, &MathObject::stateChanged, this, &MathWorld::objectUpdated);
  connect(mathObjects.back().first, &MathObject::updated, this, &MathWorld::objectUpdated);
  endInsertRows();
  return mathObjects.back().first;
}

mathobj::Expr* MathWorld::addAltExprObject()
{
  altMathObjects.emplace_back(new zg::MathObject(this, MathObject::EXPR));
  connect(altMathObjects.back(), &MathObject::stateChanged, this, &MathWorld::objectUpdated);
  connect(altMathObjects.back(), &MathObject::updated, this, &MathWorld::objectUpdated);
  return altMathObjects.back()->getExpr();
}

void MathWorld::objectUpdated()
{
  if (syncing)
    return;

  syncing = true;

  for (auto&& [f, _]: mathObjects)
    f->sync();

  for (auto&& f: altMathObjects)
    f->sync();

  syncing = false;

  updateSchrodingerStatus();

  emit updated();
}

QHash<int, QByteArray> MathWorld::roleNames() const
{
  return { {Qt::DisplayRole, "mathObjPtr"} };
}

void MathWorld::setSchrodingerConstant(MathObject* c)
{
  if (schrodingerConstant == c)
    return;

  schrodingerConstant = c;

  updateSchrodingerStatus();

  emit schrodingerConstantChanged();
}

void MathWorld::unsetSchrodingerConstant(MathObject* c)
{
  if (schrodingerConstant != c or c == nullptr)
    return;

  schrodingerConstant = nullptr;

  updateSchrodingerStatus();

  emit schrodingerConstantChanged();
}

void MathWorld::updateSchrodingerStatus()
{
  std::unordered_set<MathObject*> schrodingerObjects;
  if (schrodingerConstant)
  {
    schrodingerObjects = revdeps(*schrodingerConstant);
    schrodingerObjects.insert(schrodingerConstant);
  }

  for (auto&& [r, _]: mathObjects)
    r->setSchrodinger(schrodingerObjects.contains(r));

  for (MathObject* r: altMathObjects)
    r->setSchrodinger(schrodingerObjects.contains(r));
}

std::unordered_set<MathObject*> MathWorld::direct_revdeps(MathObject& c) const
{
  using PairT = std::pair<zg::MathObject*, PlotStyle*>;

  assert (std::ranges::find(mathObjects, &c, &PairT::first) != mathObjects.end() or
          std::ranges::find(altMathObjects, &c) != altMathObjects.end());

  std::unordered_set<std::string> direct_revdep_names;

  std::visit(
    zc::utils::overloaded{
      [&](const zc::DynMathObject<zc_t>* p){
        for (auto&& [name, t]: zc::mathWorld.direct_revdeps(p->get_name())) direct_revdep_names.insert(name);
      },
      [&](std::pair<const zc::DynMathObject<zc_t>*, const zc::DynMathObject<zc_t>*> p) {
        for (auto&& [name, _]: zc::mathWorld.direct_revdeps(p.first->get_name())) direct_revdep_names.insert(name);
        for (auto&& [name, _]: zc::mathWorld.direct_revdeps(p.second->get_name())) direct_revdep_names.insert(name);
      },
      [](std::monostate){},
    },
    c.getZcObject()
  );

  std::unordered_set<const zc::DynMathObject<zc_t>*> direct_revdep_backends;

  for (zc::DynMathObject<zc_t>& b: zc::mathWorld)
    if (auto it = direct_revdep_names.find(std::string(b.get_name())); it != direct_revdep_names.end())
      direct_revdep_backends.insert(&b);

  std::unordered_set<MathObject*> direct_revdeps_res;

  auto insert_revdep = [&](MathObject* r){
    std::visit(
      zc::utils::overloaded{
        [&](const zc::DynMathObject<zc_t>* p){
          if (direct_revdep_backends.contains(p)) direct_revdeps_res.insert(r);
        },
        [&](std::pair<const zc::DynMathObject<zc_t>*, const zc::DynMathObject<zc_t>*> p) {
          if (direct_revdep_backends.contains(p.first)) direct_revdeps_res.insert(r);
          else if (direct_revdep_backends.contains(p.second)) direct_revdeps_res.insert(r);
        },
        [](std::monostate){},
      },
      r->getZcObject()
    );
  };

  for (auto&& [r, _]: mathObjects)
    insert_revdep(r);

  for (MathObject* r: altMathObjects)
    insert_revdep(r);

  return direct_revdeps_res;
}

std::unordered_set<MathObject*> MathWorld::revdeps(MathObject& c) const
{
  std::unordered_set<MathObject*> to_explore = direct_revdeps(c), revdeps_res, explored;

  while (not to_explore.empty())
  {
    MathObject* d = *to_explore.begin();
    to_explore.erase(to_explore.begin());

    explored.insert(d);
    revdeps_res.insert(d);
    std::unordered_set<MathObject*> new_to_explore = direct_revdeps(*d);
    for (MathObject* e: new_to_explore)
      if (not explored.contains(e))
        to_explore.insert(e);
  }

  return revdeps_res;
}

}
