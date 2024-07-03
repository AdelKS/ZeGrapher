#ifndef MAINVIEWCONTAINER_H
#define MAINVIEWCONTAINER_H

#include "GraphDraw/interactivegraph.h"
#include "Widgets/sheetzoom.h"
#include "Widgets/popupwidget.h"

#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include <algorithm>

class MainViewContainer : public QScrollArea
{
    Q_OBJECT

public:
    MainViewContainer(QWidget *parent = nullptr);
    ~MainViewContainer();

protected slots:
    void onSizeSettingsChange();

protected:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

    double screenDPI;
    InteractiveGraph *mainView;
    SheetZoom *sheetZoom;
    PopupWidget *zoomPopup;
};

#endif // MAINVIEWCONTAINER_H
