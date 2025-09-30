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

MathObject* MathWorld::addAltMathObject(MathObject::Type type)
{
  altMathObjects.emplace_back(new zg::MathObject(this, type));
  connect(altMathObjects.back(), &MathObject::stateChanged, this, &MathWorld::objectUpdated);
  connect(altMathObjects.back(), &MathObject::updated, this, &MathWorld::objectUpdated);
  return altMathObjects.back();
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

  emit updated();
}

QHash<int, QByteArray> MathWorld::roleNames() const
{
  return { {Qt::DisplayRole, "mathObjPtr"} };
}

}
