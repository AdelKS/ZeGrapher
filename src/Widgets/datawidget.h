/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
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



#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include "qcolorbutton.h"
#include "DataPlot/datawindow.h"
#include "GraphDraw/mathobjectdraw.h"

namespace Ui {
class DataWidget;
}

class DataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataWidget(QWidget *parent = 0);
    ~DataWidget();
    void setWidgetNum(int num);
    void closeDataWindow();
    std::shared_ptr<const UserData> getUserData();
    DataWindow *getDataWindow();

protected slots:
    void emitRemoveSignal();
    void drawSegments(bool draw);
    void drawPoints(bool draw);
    void changeDrawState(bool draw);
    void newPointStyle(int index);
    void newLineStyle(int index);
    void setColor(QColor color);

signals:
    void removeMe(DataWidget *widget);

private:
    QList<PointStyle> pointStyleMap;
    QList<Qt::PenStyle> lineStyleMap;
    Ui::DataWidget *ui;
    QColorButton *colorButton;
    std::shared_ptr<UserData> userData;
    DataWindow *dataWindow;
};

#endif // DATAWIDGET_H
