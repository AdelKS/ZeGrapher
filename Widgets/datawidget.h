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

#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include "qcolorbutton.h"
#include "DataPlot/datawindow.h"

namespace Ui {
class DataWidget;
}

class DataWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit DataWidget(int num, Informations *info, QWidget *parent = 0);
    void setWidgetNum(int num);
    ~DataWidget();

protected slots:
    void emitRemoveSignal();

signals:
    void removeMe(DataWidget *widget);
    
private:
    Ui::DataWidget *ui;
    QColorButton *colorButton;
    DataWindow *dataWindow;
};

#endif // DATAWIDGET_H
