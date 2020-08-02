#include "mainviewcontainer.h"

MainViewContainer::MainViewContainer(Information *information, QWidget *parent) : QScrollArea(parent)
{
    this->information = information;
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

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

    screenDPI = qGuiApp->primaryScreen()->physicalDotsPerInch();

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

    if(sizeSettings.sizingType == ZeSizeSettings::FITWINDOW or zoomSettings.zoomingType == ZeZoomSettings::FITSHEET)
    {
        if(sizeSettings.sizingType == ZeSizeSettings::FITWINDOW)
        {
            ZeSizeSettings newSizeSettings = sizeSettings;

            newSizeSettings.cmSheetSize = QSizeF(double(contentsRect().size().width()) / screenDPI * CM_PER_INCH,
                                                 double(contentsRect().size().height()) / screenDPI * CM_PER_INCH);

            newSizeSettings.pxSheetSize = contentsRect().size();

            disconnect(information, SIGNAL(graphSizeSettingsChanged()), this, SLOT(resizeMainView()));
            information->setGraphSizeSettings(newSizeSettings);
            connect(information, SIGNAL(graphSizeSettingsChanged()), this, SLOT(resizeMainView()));
        }

        if(mainView->size() != contentsRect().size())
            mainView->resize(contentsRect().size());
    }
    else
    {
        double zoom = std::max(zoomSettings.zoom, 1.0);

        if(sizeSettings.sizeUnit == ZeSizeSettings::CENTIMETER)
        {
            QSize newSize(int(sizeSettings.cmSheetSize.width() / CM_PER_INCH * screenDPI * zoom),
                          int(sizeSettings.cmSheetSize.height() / CM_PER_INCH * screenDPI * zoom));

            mainView->resize(newSize);
        }
        else
        {
            QSize newSize(int(sizeSettings.pxSheetSize.width() * zoom),
                          int(sizeSettings.pxSheetSize.height() * zoom));

            mainView->resize(newSize);
        }
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
