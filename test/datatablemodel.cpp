/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "app.h"
#include "utils.h"

#include <QAbstractItemModelTester>

int main()
{
  TestApp app;

  zg::DataTableModel& model = *zg::dataTableModel;

  // a failed tester check ends the test with qFatal()
  constexpr auto fatal = QAbstractItemModelTester::FailureReportingMode::Fatal;
  QAbstractItemModelTester tableTester(&model, fatal);
  QAbstractItemModelTester worldTester(zg::mathWorld, fatal);

  auto* sheet = zg::mathWorld->addMathObject(zg::MathObject::DATASHEET)->getDataSheet();
  QAbstractItemModelTester sheetTester(sheet, fatal);

  zg::mathobj::Data* a = sheet->addColumn();
  a->setName("a");
  a->setData({"1", "2", "3", "4"});

  zg::mathobj::Data* b = sheet->addColumn();
  b->setName("b");
  b->setData({"1", "2"});

  a->setShowInTable(true);
  b->setShowInTable(true);
  test(model.columnCount() == 2);
  test(model.rowCount() == 4);
  test(model.headerData(1, Qt::Horizontal).toString() == "b");

  // the table has as many rows as its longest column
  test(model.insertRows(1, 2));
  test(model.rowCount() == 6);

  model.removeRow(0);
  test(model.rowCount() == 5);

  // removing a cell from the only longest column also removes a row
  model.removeCell(model.index(4, 0));
  test(model.rowCount() == 4);

  // a cell below the end of its column has nothing to remove
  model.removeCell(model.index(3, 1));
  test(model.rowCount() == 4);

  // editing the last row adds an empty one below it
  test(model.setData(model.index(3, 1), "7"));
  test(model.data(model.index(3, 1)).toString() == "7");
  test(model.rowCount() == 5);

  model.clearCells({model.index(0, 0), model.index(1, 1)});
  test(model.data(model.index(0, 0)).toString().isEmpty());

  // table edits reach the graph through MathWorld
  int updates = 0;
  QObject::connect(zg::mathWorld, &zg::MathWorld::updated, [&] { updates++; });
  test(model.setData(model.index(0, 0), "5"));
  test(updates == 1);
  test(a->getData().front() == "5");

  // removing a row past the end of a column doesn't change its length
  b->setData({"1", "2"});
  model.removeRow(model.rowCount() - 1);
  test(b->getData().size() == 2);

  b->setData(std::vector<std::string>(10, "0"));
  test(model.rowCount() == 10);

  a->setShowInTable(false);
  test(model.columnCount() == 1);

  // a deleted column leaves the table
  sheet->deleteColumn(b);
  QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
  test(model.columnCount() == 0);
  test(model.rowCount() == 0);

  zg::mathWorld->addMathObject(zg::MathObject::EQUATION);
  zg::mathWorld->moveMathObject(1, 0);
  zg::mathWorld->moveMathObject(0, 1);
  test(zg::mathWorld->rowCount() == 2);

  return 0;
}
