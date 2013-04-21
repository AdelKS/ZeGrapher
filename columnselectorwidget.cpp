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

#include "columnselectorwidget.h"

ColumnSelectorWidget::ColumnSelectorWidget(int count)
{      
    setCoordinateSystem(CARTESIAN);

    columnCount = count;

    xselector.index = 0;
    xselector.draw = true;
    xselector.betweenColumns = false;   

    yselector.index = 1;
    yselector.draw = true;
    yselector.betweenColumns = false;  

    selector.index = 2;
    selector.draw = true;
    selector.betweenColumns = false;
    selector.image.load(":icons/selector.png");


    draggedSelector = NULL;

    selectors << &xselector << &yselector << &selector;

    timer.setInterval(ANIMATION_PERIOD);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateAnimationProgress()));  
}

void ColumnSelectorWidget::updateSelectorsPos()
{
    xselector.pos = QPoint(xselector.index*width()/columnCount + (width()/columnCount - xselector.image.width())/2 , height() - xselector.image.height());
    yselector.pos = QPoint(yselector.index*width()/columnCount + (width()/columnCount - yselector.image.width())/2 , height() - yselector.image.height());
    selector.pos = QPoint(selector.index*width()/columnCount + (width()/columnCount - selector.image.width())/2 , height() - selector.image.height());
}

void ColumnSelectorWidget::askedForSelector()
{
    selector.draw = false;
    repaint();
}

void ColumnSelectorWidget::updateAnimationProgress()
{
    for(int i = 0 ; i < animatedSelectors.size(); i++)
    {
        animatedSelectors[i]->animation.progress += ANIMATION_PERIOD;
        animatedSelectors[i]->pos.setX(animatedSelectors[i]->animation.departureAbscissa + animatedSelectors[i]->animation.progress*(animatedSelectors[i]->animation.arrivalAbscissa - animatedSelectors[i]->animation.departureAbscissa)/ANIMATION_TIME);

        if(animatedSelectors[i]->animation.progress == ANIMATION_TIME)
            animatedSelectors.removeAt(i);
    }

    if(animatedSelectors.isEmpty())
        timer.stop();

    repaint();
}

void ColumnSelectorWidget::setCoordinateSystem(short sys)
{
    coordinateSystem = sys;

    if(coordinateSystem == POLAR)
    {
        xselector.image.load(":/icons/Rpin.png");
        yselector.image.load(":/icons/Tetapin.png");
    }
    else if(coordinateSystem == CARTESIAN)
    {
        xselector.image.load(":/icons/Xpin.png");
        yselector.image.load(":/icons/Ypin.png");
    }
}

void ColumnSelectorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    painter.begin(this);   

    drawSelectors();

    painter.end();
}

void ColumnSelectorWidget::drawSelectors()
{

    for(int i = 0 ; i < selectors.size(); i++)
    {
        if(selectors[i]->draw)
        {            
            painter.drawImage(selectors[i]->pos, selectors[i]->image);
        }
    }
}

void ColumnSelectorWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->x() >= width() - draggedSelector->image.width()/2)
    {
        draggedSelector->pos.setX(width() - draggedSelector->image.width()/2);
    }
    else if(event->x() <= draggedSelector->image.width()/2)
    {
         draggedSelector->pos.setX(draggedSelector->image.width()/2);
    }
    else
    {
        draggedSelector->pos.setX(event->x() - draggedSelector->image.width()/2);       
    }
    repaint();
}

void ColumnSelectorWidget::mousePressEvent(QMouseEvent *event)
{
    bool found = false;
    int i = 0;
    QRect rect;

    selectors.push_front(&selector);// to checks first if the main selector is clicked

    while(i < selectors.size()-1 && !found)
    {
        rect = selectors[i]->image.rect();
        rect.translate(selectors[i]->pos);

        found = rect.contains(event->pos());
        if(found)
            draggedSelector = selectors[i];

        i++;
    }

    selectors.removeFirst();

    if(!found)
    {
        emit askForSelector();
        selector.draw = true;
        draggedSelector = &selector;
        mouseMoveEvent(event);
    }

    repaint();
}

Selector* ColumnSelectorWidget::otherSelector(Selector* notThisOne)
{
    if(notThisOne == &xselector)
        return &yselector;
    else return &xselector;
}

void ColumnSelectorWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    int x = draggedSelector->pos.x() + draggedSelector->image.width()/2;
    int index = x * columnCount / width();
    int abscissa = index * width() / columnCount + width()/columnCount/2;

    if(draggedSelector != &selector || (index == 0 && x <= 3*width()/columnCount/4) || (index == columnCount-1 && x >= width()-3*width()/columnCount/4) || abs(x - abscissa) <= width()/columnCount/4) //nearest to column
    {
        if(draggedSelector != &selector && otherSelector(draggedSelector)->index == index)
        {// the user putted the x pin over the y pin, or conversely, we move the putted on pin to dragged pin's old position
            Selector *sel = otherSelector(draggedSelector);

            sel->index = draggedSelector->index;
            sel->animation.progress = 0;
            sel->animation.departureAbscissa = sel->pos.x();
            sel->animation.arrivalAbscissa = sel->index * width() / columnCount + width()/columnCount/2 - sel->image.width()/2;
            animatedSelectors.removeOne(sel);
            animatedSelectors << sel;

            if(sel == &xselector)
                emit newXIndex(sel->index);
        }
        draggedSelector->index = index;
        draggedSelector->betweenColumns = false;
        draggedSelector->animation.progress = 0;
        draggedSelector->animation.departureAbscissa = draggedSelector->pos.x();
        draggedSelector->animation.arrivalAbscissa = abscissa - draggedSelector->image.width()/2;
        animatedSelectors.removeOne(draggedSelector);
        animatedSelectors << draggedSelector;
    }
    else // nearest to between two columns
    {
        if(x - abscissa < 0) // to index
        {
            draggedSelector->index = index;
            draggedSelector->betweenColumns = true;
            draggedSelector->animation.progress = 0;
            draggedSelector->animation.departureAbscissa = draggedSelector->pos.x();
            draggedSelector->animation.arrivalAbscissa = index * width() / columnCount - draggedSelector->image.width()/2;
            animatedSelectors.removeOne(draggedSelector);
            animatedSelectors << draggedSelector;
        }
        else // to index+1
        {
            draggedSelector->index = index+1;
            draggedSelector->betweenColumns = true;
            draggedSelector->animation.progress = 0;
            draggedSelector->animation.departureAbscissa = draggedSelector->pos.x();
            draggedSelector->animation.arrivalAbscissa = (index+1) * width() / columnCount - draggedSelector->image.width()/2;
            animatedSelectors.removeOne(draggedSelector);
            animatedSelectors << draggedSelector;
        }
    }

    if(draggedSelector == &selector)
        emit newSelectorPos(selector.betweenColumns, selector.index);
    else if(draggedSelector == &xselector)
        emit newXIndex(draggedSelector->index);
    else emit newYIndex(draggedSelector->index);

    timer.start();
}

void ColumnSelectorWidget::setColumnCount(int count)
{
    columnCount = count;
}
