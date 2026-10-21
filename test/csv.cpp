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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "DataPlot/csvpreviewmodel.h"
#include "app.h"
#include "utils.h"

#include <QAbstractItemModelTester>

namespace {

using zg::CsvPreviewModel;

void testSplit()
{
  using List = QStringList;

  test(CsvPreviewModel::splitCsvLine(u"a,b,c", u",") == List{"a", "b", "c"});
  test(CsvPreviewModel::splitCsvLine(u"", u",") == List{""});
  test(CsvPreviewModel::splitCsvLine(u"a,,c,", u",") == List{"a", "", "c", ""});

  // a quoted field can contain the separator, and a doubled quote is one quote
  test(CsvPreviewModel::splitCsvLine(u"\"a,b\",c", u",") == List{"a,b", "c"});
  test(CsvPreviewModel::splitCsvLine(u"\"say \"\"hi\"\"\",x", u",") == List{"say \"hi\"", "x"});
  test(CsvPreviewModel::splitCsvLine(u"\"open", u",") == List{"open"});

  test(CsvPreviewModel::splitCsvLine(u"1\t2", u"\t") == List{"1", "2"});
  test(CsvPreviewModel::splitCsvLine(u"1;;2;;", u";;") == List{"1", "2", ""});
  test(CsvPreviewModel::splitCsvLine(u"a,b", u"") == List{"a,b"});
}

QByteArray readFile(const QString& path)
{
  QFile file(path);
  test(file.open(QIODevice::ReadOnly | QIODevice::Text));
  return file.readAll();
}

}

int main()
{
  testSplit();

  TestApp app;

  // a sheet written to CSV loads back the same, with short columns ending in
  // empty cells
  auto* sheet = zg::mathWorld->addMathObject(zg::MathObject::DATASHEET)->getDataSheet();

  const std::vector<std::pair<QString, std::vector<std::string>>> columns = {
    {"t", {"0", "1", "2", "3"}},
    {"v", {"max(1, 2)", "", "\"quoted\""}},
    {"w", {"5"}},
  };

  for (const auto& [name, values]: columns)
  {
    zg::mathobj::Data* column = sheet->addColumn();
    column->setName(name);
    column->setData(values);
  }

  const QUrl csv = QUrl::fromLocalFile(app.path("sheet.csv"));
  sheet->exportCSV(csv);
  test(information->getIoErrorCount() == 0);

  CsvPreviewModel preview;
  QAbstractItemModelTester tester(&preview, QAbstractItemModelTester::FailureReportingMode::Fatal);

  preview.setCsvFile(csv);
  test(information->getIoErrorCount() == 0);
  test(preview.columnCount() == 3);
  test(preview.headerData(1, Qt::Horizontal).toString() == "v");
  test(preview.data(preview.index(0, 1)).toString() == "max(1, 2)");

  preview.setRowSkipCount(1);
  preview.setMaxRowCount(2);
  test(preview.rowCount() == 2);
  preview.setRowSkipCount(0);
  preview.setMaxRowCount(10);
  test(preview.rowCount() == 4);

  preview.loadIntoWorld();

  const auto& loaded = zg::mathWorld->getMathObjects().back()->getDataSheet()->getColumns();
  test(loaded.size() == columns.size());

  for (size_t i = 0 ; i != columns.size() ; i++)
  {
    std::vector<std::string> expected = columns[i].second;
    expected.resize(4);

    test(loaded[i]->getName() == columns[i].first);
    test(loaded[i]->getData() == expected);
  }

  // RFC 4180 quotes fields with line breaks
  sheet->getColumns()[2]->setData({"a\nb"});
  sheet->exportCSV(csv);
  test(readFile(csv.toLocalFile()).contains("\"a\nb\""));

  return 0;
}
