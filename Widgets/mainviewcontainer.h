#ifndef MAINVIEWCONTAINER_H
#define MAINVIEWCONTAINER_H

#include "GraphDraw/mainview.h"
#include "Widgets/sheetzoom.h"

#include <QObject>
#include <QWidget>
#include <QScrollArea>

class MainViewContainer : public QScrollArea
{
    Q_OBJECT

public:
    MainViewContainer(Information *information, QWidget *parent = nullptr);

protected slots:
    void resizeMainView();

protected:
    Information *information;
    MainView *mainView;
    SheetZoom *sheetZoom;
};

#endif // MAINVIEWCONTAINER_H
