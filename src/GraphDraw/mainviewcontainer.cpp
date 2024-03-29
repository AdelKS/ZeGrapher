#include "mainviewcontainer.h"

MainViewContainer::MainViewContainer(QWidget *parent) : QScrollArea(parent)
{
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    mainView = new MainView();

    setWidget(mainView);
    setWidgetResizable(false);

    mainView->onSizeSettingsChange();

    sheetZoom = new SheetZoom();

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

    onSizeSettingsChange();

    connect(&information, SIGNAL(graphSizeSettingsChanged()), this, SLOT(onSizeSettingsChange()));
}

void MainViewContainer::onSizeSettingsChange()
{
    if(information.getGraphSizeSettings().sheetFillsWindow)
        zoomPopup->hideWidget();
    else zoomPopup->showWidget();
}

void MainViewContainer::resizeEvent(QResizeEvent *event)
{
    QScrollArea::resizeEvent(event);

    mainView->updateWidgetSize();
    zoomPopup->updatePositions();
}

void MainViewContainer::showEvent(QShowEvent *event)
{
    QScrollArea::showEvent(event);

    sheetZoom->resetZoom();
}

MainViewContainer::~MainViewContainer()
{

}
