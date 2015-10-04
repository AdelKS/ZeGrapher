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



#ifndef PRINT_H
#define PRINT_H

#include <QWidget>
#include "GraphDraw/printpreview.h"

#define PORTRAIT true
#define PAYSAGE false

#define SCREEN_RESOLUTION true
#define CUSTOM_RESOLUTION false

#define PRINT_SHEET true
#define PRINT_FILE false

namespace Ui {
    class Print;
}

class Print : public QWidget
{
    Q_OBJECT

public:
    explicit Print(Informations *info);
    ~Print();  

public slots:
    void viewChanged(int);
    void setGraphSize(double H, double W);
    void print();
    void enablePrintButton();
    void getPDFname();
    void setPrintButtonText(bool);
    void setOrthonormal(bool state);
    void checkScaleWidgets(bool);

protected:    

    Ui::Print *ui;
    PrintPreview *scene;   
    QList<QPrinterInfo> printersList;
    QTimer timer;
    QString PDFname;
    bool orthonormal;
};

#endif // PRINT_H
