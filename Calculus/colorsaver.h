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



#ifndef COLORSAVER_H
#define COLORSAVER_H

#include <QtGui>

class ColorSaver : public QObject
{
    Q_OBJECT

public:
    ColorSaver(QColor firstANDlast = Qt::black);

    void setCurvesNum(double num);
    QColor getColor(int index);

public slots:
    void setFristColor(QColor col);
    void setLastColor(QColor col);

signals:
    void colorsChanged();

protected:
    void saveColors();

    double curvesNum;
    QColor firstColor, lastColor;
    QList<QColor> colorsList;

};

#endif // COLORSAVER_H
