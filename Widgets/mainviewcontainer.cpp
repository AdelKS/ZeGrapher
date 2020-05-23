#include "mainviewcontainer.h"

MainViewContainer::MainViewContainer(Information *information, QWidget *parent) : QScrollArea(parent)
{
    this->information = information;

    mainView = new MainView(information);

    setWidget(mainView);

    sheetZoom = new SheetZoom(information, this);
    sheetZoom->move(0, 0);

    connect(information, SIGNAL(graphSizeSettingsChanged()), this, SLOT(resizeMainView()));
    connect(information, SIGNAL(graphZoomSettingsChanged()), this, SLOT(resizeMainView()));

}

void MainViewContainer::resizeMainView()
{
    const auto &sizeSettings = information->getGraphSettings().sizeSettings;
    const auto &zoomSettings = information->getGraphZoomSettings();


    if(zoomSettings.zoomingType == ZeZoomSettings::FITSHEET)
    {
        if(sizeSettings.sizeUnit == ZeSizeSettings::SizeUnit::CENTIMETER)
        {
            if(mainView->size() != contentsRect().size())
                mainView->resize(contentsRect().size());
        }
        else
        {
            if(contentsRect().size().width() > sizeSettings.pxSheetSize.width() &&
                    contentsRect().size().height() > sizeSettings.pxSheetSize.height())
                mainView->resize(sizeSettings.pxSheetSize.width(), sizeSettings.pxSheetSize.height());
            else if(mainView->size() != contentsRect().size())
                mainView->resize(contentsRect().size());
        }

    }
    else
    {
        QSize targetSize = mainView->getTargetSupportSizePixels();
        targetSize.setHeight(int(double(targetSize.height()) * zoomSettings.zoom / 100));
        targetSize.setWidth(int(double(targetSize.width()) * zoomSettings.zoom / 100));
        mainView->resize(targetSize);
    }
}
