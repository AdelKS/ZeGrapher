/****************************************************************************
**  Copyright (c) 2019, Adel Kara Slimane <adel.ks@zegrapher.com>
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



#include "DataPlot/rowselectorwidget.h"

RowSelectorWidget::RowSelectorWidget(int count)
{
    rowCount = count;

    selector.index = 0;
    selector.draw = false;
    selector.betweenRows = false;
    selector.image.load(":icons/rotatedselector.png");

    timer.setInterval(ANIMATION_PERIOD);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateAnimationProgress()));
}

void RowSelectorWidget::updateSelectorsPos()
{
    selector.pos.setX(width() - selector.image.width());
    selector.pos.setY(selector.index*height()/rowCount + selector.image.height()/2);
}

void RowSelectorWidget::askedForSelector()
{
    selector.draw = false;
    repaint();
}

void RowSelectorWidget::updateAnimationProgress()
{
    selector.animation.progress += ANIMATION_PERIOD;
    selector.pos.setY(selector.animation.departureOrdinate + selector.animation.progress*(selector.animation.arrivalOrdinate - selector.animation.departureOrdinate)/ROW_ANIMATION_TIME);

    if(selector.animation.progress == ROW_ANIMATION_TIME)
        timer.stop();

    repaint();
}


void RowSelectorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    painter.begin(this);

    if(selector.draw)
        painter.drawImage(selector.pos, selector.image);

    painter.end();
}

void RowSelectorWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->y() >= height() - selector.image.height()/2)
    {
        selector.pos.setY(height() - selector.image.height());
    }
    else if(event->y() <= selector.image.height()/2)
    {
        selector.pos.setY(0);
    }
    else
    {
        selector.pos.setY(event->y() - selector.image.height()/2);
    }

     repaint();
}

void RowSelectorWidget::mousePressEvent(QMouseEvent *event)
{
    if(!selector.draw)
    {
        selector.draw = true;
        emit askForSelector();
    }

    if(event->y() >= height() - selector.image.height()/2)
    {
        selector.pos.setY(height() - selector.image.height()/2);
    }
    else if(event->y() <= selector.image.height()/2)
    {
        selector.pos.setY(selector.image.height()/2);
    }
    else
    {
        selector.pos.setY(event->y() - selector.image.height()/2);
    }

    repaint();
}

void RowSelectorWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    int y = selector.pos.y() + selector.image.height()/2;
    int index = y * rowCount / height();
    int ordinate = index * height() / rowCount + height()/rowCount/2;

    if((index == 0 && y <= 3*height()/rowCount/4) || (index == rowCount-1 && y >= height()-3*height()/rowCount/4) || abs(y - ordinate) <= height()/rowCount/4) //nearest to the center of the column
    {
        selector.index = index;
        selector.betweenRows = false;
        selector.animation.progress = 0;
        selector.animation.departureOrdinate = selector.pos.y();
        selector.animation.arrivalOrdinate = ordinate - selector.image.height()/2;
    }
    else if(y - ordinate < 0) // nearer to index
    {
        selector.index = index;
        selector.betweenRows = true;
        selector.animation.progress = 0;
        selector.animation.departureOrdinate = selector.pos.y();
        selector.animation.arrivalOrdinate = index * height() / rowCount - selector.image.height()/2;
    }
    else //nearer to index+1
    {
        selector.index = index+1;
        selector.betweenRows = true;
        selector.animation.progress = 0;
        selector.animation.departureOrdinate = selector.pos.y();
        selector.animation.arrivalOrdinate = (index+1) * height() / rowCount - selector.image.height()/2;
    }

    emit newIndex(selector.betweenRows, selector.index);


    timer.start();
}

void RowSelectorWidget::setRowCount(int count)
{    
    rowCount = count;
}
