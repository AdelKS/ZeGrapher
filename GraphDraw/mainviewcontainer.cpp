#include "mainviewcontainer.h"

MainViewContainer::MainViewContainer(Information *information, QWidget *parent) : QScrollArea(parent)
{
    this->information = information;
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    mainView = new MainView(information);

    setWidget(mainView);

    setWidgetResizable(true);

    mainView->onSizeSettingsChange();

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
}

void MainViewContainer::resizeEvent(QResizeEvent *event)
{
    QScrollArea::resizeEvent(event);

    zoomPopup->updatePositions();
}

MainViewContainer::~MainViewContainer()
{

}
