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
#include "GraphDraw/graph.h"

#include <QQuickWindow>

namespace {

QByteArray readFile(const QString& path)
{
  QFile file(path);
  test(file.open(QIODevice::ReadOnly));
  return file.readAll();
}

}

int main()
{
  TestApp app;

  test(app.load(R"(graph:
  size: {width: 15, height: 10}
math_objects:
  - type: equation
    equation: f(x) = cos(x)
)"));

  Graph graph;

  graph.exportTo(QUrl::fromLocalFile(app.path("graph.pdf")));
  test(information->getIoErrorCount() == 0);
  const QByteArray pdf = readFile(app.path("graph.pdf"));
  test(pdf.startsWith("%PDF") and pdf.trimmed().endsWith("%%EOF"));

  graph.exportTo(QUrl::fromLocalFile(app.path("graph.svg")));
  test(information->getIoErrorCount() == 0);
  test(readFile(app.path("graph.svg")).trimmed().endsWith("</svg>"));

  // an image takes the device pixel ratio of the window
  QQuickWindow window;
  graph.setParentItem(window.contentItem());

  graph.exportTo(QUrl::fromLocalFile(app.path("graph.png")));
  test(information->getIoErrorCount() == 0);
  test(readFile(app.path("graph.png")).startsWith("\x89PNG"));

  // an extension without an image format reports an error, and writes nothing
  graph.exportTo(QUrl::fromLocalFile(app.path("graph.unknown")));
  test(information->getIoErrorCount() == 1);
  test(not QFile::exists(app.path("graph.unknown")));
  information->popIoError();

  // exporting to a missing folder reports an error
  graph.exportTo(QUrl::fromLocalFile(app.path("missing-folder/graph.pdf")));
  test(information->getIoErrorCount() == 1);
  test(not QFile::exists(app.path("missing-folder/graph.pdf")));

  graph.exportTo(QUrl::fromLocalFile(app.path("missing-folder/graph.png")));
  test(information->getIoErrorCount() == 2);
  test(not QFile::exists(app.path("missing-folder/graph.png")));

  return 0;
}
