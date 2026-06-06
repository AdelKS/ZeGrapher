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

#include <QApplication>
#include <QTimer>

// track application palette change through signals
// this is not offered out of the box by Qt for whatever reason
class PaletteWatcher : public QObject
{
  Q_OBJECT

public:

  explicit PaletteWatcher(QObject *parent = nullptr)
    : QObject(parent), timer(this)
  {
    timer.setInterval(500);
    timer.setSingleShot(true);
    qApp->installEventFilter(this);

    connect(&timer, &QTimer::timeout, this, &PaletteWatcher::paletteChanged);
  }

signals:
  void paletteChanged();

protected:
  bool eventFilter(QObject *obj, QEvent *event) override
  {
    if (obj == qApp && event->type() == QEvent::ApplicationPaletteChange)
      timer.start();
    return QObject::eventFilter(obj, event);
  }

  QTimer timer;

};
