#ifndef POPUPWIDGET_H
#define POPUPWIDGET_H

#define GAP 0

#include <QWidget>
#include <QPropertyAnimation>
#include <QDebug>

enum struct PopupPos {TOP, RIGHT, LEFT, BOTTOM};
enum struct PopupState {SHOWN, HIDDEN};
enum struct PopupAnimationState {HIDE_ANIMATION, SHOW_ANIMATION, IDLE};

class PopupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PopupWidget(PopupPos pos, double relPos, QWidget *parent = nullptr);
    void setWidget(QWidget *widget);
    void updatePositions();  
    PopupState getState();
    PopupAnimationState getAnimationState();

public slots:
    void showWidget();
    void hideWidget();


protected slots:
    void animationFinished();

protected:
    QWidget *floatingWidget;
    QPropertyAnimation *animator;

    PopupPos directionPos;
    double relativePos;
    PopupState state;
    PopupAnimationState animationState;
    bool showOnHover;
    int gap;
};

#endif // POPUPWIDGET_H
