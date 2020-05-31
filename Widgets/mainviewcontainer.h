#ifndef MAINVIEWCONTAINER_H
#define MAINVIEWCONTAINER_H

#include "GraphDraw/mainview.h"
#include "Widgets/sheetzoom.h"
#include "Widgets/popupwidget.h"

#include <QObject>
#include <QWidget>
#include <QScrollArea>

class MainViewContainer : public QScrollArea
{
    Q_OBJECT

public:
    MainViewContainer(Information *information, QWidget *parent = nullptr);
    ~MainViewContainer();
protected slots:
    void resizeMainView();

protected:
    void resizeEvent(QResizeEvent *event);

    Information *information;
    MainView *mainView;
    SheetZoom *sheetZoom;
    PopupWidget *zoomPopup;
};

#endif // MAINVIEWCONTAINER_H
