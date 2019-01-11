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


#ifndef PRINT_H
#define PRINT_H

#include <QWidget>
#include "GraphDraw/exportpreview.h"

namespace Ui {
    class Export;
}

class Export : public QWidget
{
    Q_OBJECT

public:
    explicit Export(Information *info, QWidget *parent = nullptr);
    ~Export();

public slots:    
    void setSheetFigureSizeCm(QSizeF sizeCm);
    void enableExportButton();
    void getFileName();
    void exportGraph();

protected slots:
    void onSheetSizeChange();
    void onSheetFigureSizeChange();
    void swapSheetHeightAndWidth();
    void constrainFigureSizeWidgets();
    void resizeExportPreview();
    void zoomIn();
    void zoomOut();
    void newZoomValue(double value);
    void onZoomPercentageUserChange();
    void activateRealSizePreview();
    void updateWidgetsVisibility();

protected:    
    void resizeEvent(QResizeEvent *event);

    Ui::Export *ui;
    QTabWidget *tabWidget;
    ExportPreview *exportPreview;
    QTimer timer;
    QString fileName;
    bool orthonormal;
};

#endif // PRINT_H
