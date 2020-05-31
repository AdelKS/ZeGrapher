#include "mainviewcontainer.h"

MainViewContainer::MainViewContainer(Information *information, QWidget *parent) : QScrollArea(parent)
{
    this->information = information;

    mainView = new MainView(information);

    setWidget(mainView);

    sheetZoom = new SheetZoom(information);

    QWidget *zoomContainer = new QWidget();
    zoomContainer->setObjectName("zoomContainer");
    zoomContainer->setStyleSheet("QWidget#zoomContainer {background-color: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #f4f4f4ff, stop: 0.4 #E8E8E8 , stop: 0.6 #E8E8E8, stop: 1 #f4f4f4ff); "
                          " border-style: none; border-bottom-right-radius: 10; border-bottom-left-radius: 10;}");

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(2, 2, 2, 4);
    layout->addWidget(sheetZoom);
    zoomContainer->setLayout(layout);

    zoomPopup = new PopupWidget(PopupPos::TOP, 0.5, this);
    zoomPopup->setWidget(zoomContainer);
    zoomPopup->setState(PopupState::SHOWN);

    connect(information, SIGNAL(graphSizeSettingsChanged()), this, SLOT(resizeMainView()));
    connect(information, SIGNAL(graphZoomSettingsChanged()), this, SLOT(resizeMainView()));

}

void MainViewContainer::resizeMainView()
{
    const auto &sizeSettings = information->getGraphSettings().sizeSettings;
    const auto &zoomSettings = information->getGraphZoomSettings();

    if(sizeSettings.sizingType == ZeSizeSettings::FITWINDOW)
    {
        if(mainView->size() != contentsRect().size())
            mainView->resize(contentsRect().size());
    }
    else if(zoomSettings.zoomingType == ZeZoomSettings::FITSHEET)
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

void MainViewContainer::resizeEvent(QResizeEvent *event)
{
    QScrollArea::resizeEvent(event);

    resizeMainView();
    zoomPopup->updatePositions();
}

MainViewContainer::~MainViewContainer()
{

}
