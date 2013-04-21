/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.1.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
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

#ifndef ROWSELECTORWIDGET_H
#define ROWSELECTORWIDGET_H

#include <QtGui>

#define XSELECTOR 0
#define YSELECTOR 1
#define SELECTOR 2

#define POLAR 0
#define CARTESIAN 1

#define ROW_ANIMATION_TIME 240 //in ms
#define ANIMATION_PERIOD 15

struct RowAnimation
{
    int progress, departureOrdinate, arrivalOrdinate, arrivalIndex;
};

struct RowSelector
{
    QImage image;
    bool betweenRows, draw;
    int index;
    QPoint pos;
    RowAnimation animation;
};

class RowSelectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RowSelectorWidget(int count);
    void updateSelectorsPos();

public slots:
    void setRowCount(int count);
    void askedForSelector();

signals:
    void newIndex(bool betweenRows, int index);
    void askForSelector();

protected slots:
    void updateAnimationProgress();

protected:
    void drawSelectors();

    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    QPainter painter;
    QTimer timer;
    int rowCount;
    bool hasSelector;
    RowSelector selector;
};

#endif // ROWSELECTORWIDGET_H
