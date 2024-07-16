#ifndef SHEETZOOM_H
#define SHEETZOOM_H

#include <QWidget>

#include "structures.h"

namespace Ui {
class SheetZoom;
}

class SheetZoom : public QWidget
{
    Q_OBJECT

public:
    explicit SheetZoom(QWidget *parent = nullptr);
    void resetZoom();
    ~SheetZoom();

protected slots:
    void activateRealSizePreview();
    void zoomIn();
    void zoomOut();
    void onExternalZoomSettingsChange();
    void onZoomValueChange();

    void updateZoomSettings();

private:
    Ui::SheetZoom *ui;
    ZeZoomSettings zoomSettings;

    friend class MainViewContainer;
};

#endif // SHEETZOOM_H
