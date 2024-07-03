#ifndef MAINVIEWCONTAINER_H
#define MAINVIEWCONTAINER_H

#include "GraphDraw/interactivegraph.h"
#include "Widgets/sheetzoom.h"
#include "Widgets/popupwidget.h"

#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include <algorithm>

class InteractiveGraphContainer : public QScrollArea
{
    Q_OBJECT

public:
    InteractiveGraphContainer(QWidget *parent = nullptr);
    ~InteractiveGraphContainer();

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
