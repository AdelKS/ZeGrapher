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



#ifndef PRINTPREVIEW_H
#define PRINTPREVIEW_H

#include <QPrinterInfo>
#include <QPrinter>

#include "imagepreview.h"

#define PORTRAIT true
#define PAYSAGE false

#define COLOR true
#define GRAYSCALE false

#define SCREEN_RESOLUTION true
#define CUSTOM_RESOLUTION false

#define PRINT_SHEET true
#define PRINT_FILE false

#define NOTHING 0
#define TOPLEFT_CORNER 1
#define TOPRIGHT_CORNER 2
#define BOTTOMLEFT_CORNER 3
#define BOTTOMRIGHT_CORNER 4
#define LEFT_SIDE 5
#define TOP_SIDE 6
#define RIGHT_SIDE 7
#define BOTTOM_SIDE 8
#define ALL 9

class PrintPreview : public ImagePreview
{
    Q_OBJECT

public:
    explicit PrintPreview(Information *info);
    void setViewType(bool type);  
    void setPrinter(QPrinterInfo printInfo);
    void setPDFname(QString pdf);

signals:
    void newGraphSize(double H, double W);  

public slots:    
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);   

    void setGraphHeight(double H);
    void setGraphWidth(double W);   
    void print(int nbPages, bool colorType, bool printType, bool resType, int res = 0);
protected:
    void paintEvent(QPaintEvent *event);
    void determinerCentreEtUnites();
    void drawSheet();
    void drawGraph();
    void assignGraphSize();
    void assignMouseRects();
    void printCurves();
    void testGraphPosition();

    bool viewType;
    QRectF graph, sheetRect, graphRect, topLeft, topRight, top, left, right, bottom, bottomLeft, bottomRight;
    double sheetHeight, graphHeightCm, sheetWidth, graphWidthCm, relativeXposCm, relativeYposCm;
    QPointF lastMousePos ;
    short moveType;
    QPrinterInfo printerInfo;
    QPrinter *printer;
    QString PDFname;

};

#endif // PRINTPREVIEW_H
