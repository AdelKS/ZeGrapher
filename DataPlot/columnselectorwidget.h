/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
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


#ifndef COLUMNSELECTORWIDGET_H
#define COLUMNSELECTORWIDGET_H

#include <QtWidgets>

#define XSELECTOR 0
#define YSELECTOR 1
#define SELECTOR 2

#define POLAR false
#define CARTESIAN true

#define ANIMATION_TIME 240 //in ms
#define ANIMATION_PERIOD 15

struct Animation
{
    int progress, departureAbscissa, arrivalAbscissa, arrivalIndex;
};

struct Selector
{
    QImage image;
    bool betweenColumns, draw;
    int index;
    QPoint pos;
    Animation animation;
};

class ColumnSelectorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ColumnSelectorWidget(int count, int xindex, int yindex, int selectorindex);
    void updateSelectorsPos();
    int getXindex();
    int getYindex();

public slots:
    void setColumnCount(int count);
    void askedForSelector();
    void setCoordinateSystem(bool cartesian);
    
signals:
    void newXIndex(int index);
    void newYIndex(int index);
    void newSelectorPos(bool betweenColumns, int index);
    void askForSelector();

protected slots:
    void updateAnimationProgress();

protected:
    void drawSelectors();


    Selector* otherSelector(Selector* notThisOne);

    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    QPainter painter;
    QTimer timer;
    int columnCount;
    bool coordinateSystem;
    bool hasSelector;
    Selector xselector, yselector, selector;
    Selector *draggedSelector;
    QList<Selector*>selectors;
    QList<Selector*>animatedSelectors;
    
};

#endif // COLUMNSELECTORWIDGET_H
