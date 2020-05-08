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


#ifndef PRINT_H
#define PRINT_H

#include <QWidget>
#include "GraphDraw/mainview.h"

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
    void setImageFigureSizePx(QSize sizePx);

    void enableExportButton();
    void getFileName();
    void exportGraph();

protected slots:
    void onSheetSizeChange();
    void onImageSizeChange();

    void onMainGraphRangeChange(GraphRange range); // range change coming from the application's main graph

    void onSheetMarginChange();
    void onImageMarginChange();

    void onSheetFigureSizeChange();
    void onImageFigureSizeChange();

    void swapSheetHeightAndWidth();
    void swapImageHeightAndWidth();

    void constrainFigureSizeWidgets();
    void resizeExportPreview();
    void zoomIn();
    void zoomOut();
    void newZoomValue(double value);
    void onZoomPercentageUserChange();
    void activateRealSizePreview();
    void exportFormatChanged();

protected:    
    void makeUiConnects();
    void resizeEvent(QResizeEvent *event);
    Ui::Export *ui;
    QTabWidget *tabWidget;
    MainView *exportPreview;
    QTimer timer;
    QString fileName;
    bool orthonormal;
    Information *information;
};

#endif // PRINT_H
