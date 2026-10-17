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
  connect(&style, &PlotStyle::updated, this, &Data::updated);
}

void Data::setName(QString new_input_name)
{
  if (input_name == new_input_name)
  {
    sync();
    return;
  }

  input_name = new_input_name;
  zcMathObj.set_name(input_name.toStdString());

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

void Data::removeValue(size_t row)
{
  // remove_data_point() grows a column that ends before 'row'
  if (row >= getData().size())
    return;

  zcMathObj.remove_data_point(row);
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
  {
    Base::setDiscrete(*discrete);
    style.setDiscrete(*discrete);
  }

  return getState();
};

Data::POD Data::exportPod() const
{
  return POD {
    .name = yml::not_default(getName()),
    .start = yml::not_default(getStartStr(), getDefaultStringRange().start),
    .end = yml::not_default(getEndStr(), getDefaultStringRange().end),
    .coordinates = yml::not_default(coordinateSystem, Cartesian),
    .values = yml::not_default(getData()),
    .style = style.exportPod(),
    .show_in_table = yml::not_default(showInTable, false)
  };
}

void Data::importPod(Data::POD p)
{
  if (p.name)
    setName(QString::fromStdString(*p.name));

  if (p.values)
    setData(std::move(*p.values));

  if (p.coordinates)
    setCoordinateSystem(*p.coordinates);

  if (p.start)
    setStart(QString::fromStdString(*p.start));

  if (p.end)
    setEnd(QString::fromStdString(*p.end));

  if (p.style)
    style.importPod(std::move(*p.style));

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

std::optional<zg::SamplingSettings> Data::getSamplingSettings()
{
  return Base::getSamplingSettings(getRevision());
}

}
}
