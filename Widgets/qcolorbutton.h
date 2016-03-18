/****************************************************************************
**  Copyright (c) 2016, Adel Kara Slimane <adel.ks@zegrapher.com>
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




#ifndef QCOLORBUTTON_H
#define QCOLORBUTTON_H

#include <QPushButton>
#include <QColorDialog>

class QColorButton : public QPushButton
{
    Q_OBJECT
public:
    explicit QColorButton(QColor color = Qt::black);
    
    void setColor(QColor color);
    QColor getCurrentColor();

signals:
    void colorChanged(QColor color);

protected slots:
    void openColorDialog();
    
protected:
    QColor currentColor;
    
};

#endif // QCOLORBUTTON_H
