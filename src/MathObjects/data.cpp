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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <string>

#include "MathObjects/data.h"
#include "MathObjects/datasheet.h"
#include "Utils/yaml.h"
#include "DataPlot/datatablemodel.h"

namespace zg {
namespace mathobj {

Data::Data(QObject *parent)
  : Base([](CoordinateSystem s, bool){
             return s == CoordinateSystem::Polar
             ? StringRange{"0", "10"}
             : StringRange{"0", "xmax"}; }, parent)
{
  zcMathObj.set_data(std::vector<std::string>(10));
}

void Data::setName(QString newInputName)
{
  if (inputName == newInputName)
  {
    sync();
    return;
  }

  inputName = newInputName;
  zcMathObj.set_name(inputName.toStdString());

  sync();

  emit nameChanged();
  emit updated();
}

void Data::setState(State s)
{
  if (state == s) return;

  state = s;
  emit stateChanged();
}

void Data::setData(std::vector<std::string> values)
{
  emit dataAboutToBeReplaced();
  zcMathObj.set_data(std::move(values));
  emit dataReplaced();

  valuesEdited();
}

void Data::setValue(size_t row, std::string value)
{
  zcMathObj.set_data_point(row, std::move(value));
  valuesEdited();
}

void Data::insertValues(size_t row, size_t count)
{
  zcMathObj.insert_data_points(row, std::vector<std::string>(count));
  valuesEdited();
}

void Data::clearValues(const std::vector<size_t>& rows)
{
  std::vector<std::string> values = getData();

  bool cleared = false;
  for (size_t row: rows)
    if (row < values.size() and not values[row].empty())
    {
      values[row].clear();
      cleared = true;
    }

  if (not cleared)
    return;

  // ZeCalculator rebinds the column's dependents on each edit, so edit once
  zcMathObj.set_data(std::move(values));
  valuesEdited();
}

void Data::removeValues(std::vector<size_t> rows)
{
  std::ranges::sort(rows);

  const std::vector<std::string>& values = getData();
  std::vector<std::string> kept;
  kept.reserve(values.size());

  auto next = rows.begin();
  for (size_t row = 0; row != values.size(); row++)
  {
    while (next != rows.end() and *next < row)
      next++;

    if (next == rows.end() or *next != row)
      kept.push_back(values[row]);
  }

  if (kept.size() == values.size())
    return;

  // ZeCalculator rebinds the column's dependents on each edit, so edit once
  zcMathObj.set_data(std::move(kept));
  valuesEdited();
}

void Data::valuesEdited()
{
  sync();
  emit updated();
}

const std::vector<std::string>& Data::getData() const
{
  static const std::vector<std::string> empty;
  if (auto* ptr = zcMathObj.get_data())
    return *ptr;
  else return empty;
}

State Data::sync() {
  setState(State::from(zcMathObj.name_status()));

  // on a syntax error, the object keeps its kind and that kind's plot style
  if (const std::optional<bool> discrete = shared::parsedDiscrete(zcMathObj))
    Base::setDiscrete(*discrete);

  return getState();
}

Data::POD Data::exportPod() const
{
  POD p;
  p.name = yml::not_default(getName());
  p.values = yml::not_default(getData());
  p.show_in_table = yml::not_default(showInTable, false);
  exportCurve(p);
  return p;
}

void Data::importPod(Data::POD p)
{
  if (p.name)
    setName(QString::fromStdString(*p.name));

  if (p.values)
    setData(std::move(*p.values));

  importCurve(p);

  if (p.show_in_table)
    setShowInTable(*p.show_in_table);
}

void Data::requestUiInitiatedDelete()
{
  if (auto* sheet = qobject_cast<DataSheet*>(parent()))
    sheet->deleteColumn(this);
}

void Data::setShowInTable(bool s)
{
  if (s == showInTable) return;

  showInTable = s;
  if (showInTable)
    dataTableModel->registerTableColumn(this);
  else
    dataTableModel->deregisterTableColumn(this);

  emit showInTableChanged();
}

Data::~Data()
{
  dataTableModel->deregisterTableColumn(this);
}

}
}
