#ifndef SHEETZOOM_H
#define SHEETZOOM_H

#include <QWidget>

#include "information.h"

namespace Ui {
class SheetZoom;
}

class SheetZoom : public QWidget
{
    Q_OBJECT

public:
    explicit SheetZoom(Information *information, QWidget *parent = nullptr);
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
    Information *information;

    friend class MainViewContainer;
};

#endif // SHEETZOOM_H
