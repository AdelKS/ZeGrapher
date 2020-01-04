#include "popupwidget.h"

PopupWidget::PopupWidget(PopupPos pos, double relPos, QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
    directionPos = pos;
    relativePos = relPos;
    state = PopupState::SHOWN;
    animationState = PopupAnimationState::IDLE;
    showOnHover = false;
    gap = GAP;

    animator = new QPropertyAnimation(this);
    animator->setEasingCurve(QEasingCurve::OutCubic);
    animator->setPropertyName("pos");

    connect(animator, &QPropertyAnimation::finished, this, &PopupWidget::animationFinished);
}

void PopupWidget::setWidget(QWidget *widget)
{
    floatingWidget = widget;
    floatingWidget->setParent(this);

    animator->setTargetObject(floatingWidget);   

    updatePositions();
}

void PopupWidget::updatePositions()
{
    QSize containerSize;

    if(directionPos == PopupPos::TOP or directionPos == PopupPos::BOTTOM)
    {
        containerSize.setWidth(floatingWidget->width());
        containerSize.setHeight((floatingWidget->height() + gap) * 2);
    }
    else
    {
        containerSize.setWidth((floatingWidget->width() + gap) * 2);
        containerSize.setHeight(floatingWidget->height());
    }

    setFixedSize(containerSize);

    QRect parentRec = parentWidget()->rect();
    parentRec.moveTo(0, 0);

    switch (directionPos)
    {
        case PopupPos::TOP:
        move(QPoint(parentRec.width() * relativePos - floatingWidget->width()/2, - floatingWidget->height() - gap));

        if(state == PopupState::SHOWN)
            floatingWidget->move(0, containerSize.height() - floatingWidget->height());
        else floatingWidget->move(0, 0);

        break;
        case PopupPos::BOTTOM:
        move(QPoint(parentRec.width() * relativePos - floatingWidget->width()/2, parentRec.height() - floatingWidget->height() - gap));

        if(state == PopupState::SHOWN)
            floatingWidget->move(0, containerSize.height() - floatingWidget->height());
        else floatingWidget->move(0, 0);

        break;
        case PopupPos::LEFT:
        move(QPoint(- floatingWidget->width() - gap, parentRec.height() * relativePos - floatingWidget->height()/2));

        if(state == PopupState::SHOWN)
            floatingWidget->move(containerSize.width() - floatingWidget->width(), 0);
        else floatingWidget->move(0, 0);

        break;
        case PopupPos::RIGHT:
        move(QPoint(parentRec.width() - floatingWidget->width() - gap, parentRec.height() * relativePos - floatingWidget->height()/2));

        if(state == PopupState::SHOWN)
            floatingWidget->move(0, 0);
        else floatingWidget->move(containerSize.width() - floatingWidget->width(), 0);

        break;
    }

    if(state == PopupState::SHOWN)
        floatingWidget->show();
    else floatingWidget->hide();
}

void PopupWidget::showWidget()
{
    if(state == PopupState::SHOWN or animationState == PopupAnimationState::SHOW_ANIMATION)
        return;

    floatingWidget->show();

    animator->stop();

    switch (directionPos)
    {
        case PopupPos::TOP:
        animator->setStartValue(floatingWidget->pos());
        animator->setEndValue(QPoint(floatingWidget->pos().x(), height() - floatingWidget->height()));
        break;
        case PopupPos::BOTTOM:
        animator->setStartValue(floatingWidget->pos());
        animator->setEndValue(QPoint(floatingWidget->pos().x(), 0));
        break;
        case PopupPos::LEFT:
        animator->setStartValue(floatingWidget->pos());
        animator->setEndValue(QPoint(width() - floatingWidget->width(), floatingWidget->pos().y()));
        break;
        case PopupPos::RIGHT:
        animator->setStartValue(floatingWidget->pos());
        animator->setEndValue(QPoint(0,  floatingWidget->pos().y()));
        break;
    }

    animationState = PopupAnimationState::SHOW_ANIMATION;

    animator->start();
}

void PopupWidget::setState(PopupState state)
{
    this->state = state;
    animator->stop();
    animationState = PopupAnimationState::IDLE;

    updatePositions();
}

PopupState PopupWidget::getState()
{
    return state;
}

PopupAnimationState PopupWidget::getAnimationState()
{
    return animationState;
}

void PopupWidget::animationFinished()
{
    switch (animationState)
    {
        case PopupAnimationState::HIDE_ANIMATION:
            state = PopupState::HIDDEN;
            floatingWidget->hide();
            break;
        case PopupAnimationState::SHOW_ANIMATION:
            state = PopupState::SHOWN;
            break;
        default:
            qDebug() << "Problem in popup animation";
    }

    animationState = PopupAnimationState::IDLE;
}

void PopupWidget::hideWidget()
{
    if(state == PopupState::HIDDEN or animationState == PopupAnimationState::HIDE_ANIMATION)
        return;

    animator->stop();

    switch (directionPos)
    {
        case PopupPos::TOP:
        animator->setStartValue(floatingWidget->pos());
        animator->setEndValue(QPoint(floatingWidget->pos().x(), 0));
        break;
        case PopupPos::BOTTOM:
        animator->setStartValue(floatingWidget->pos());
        animator->setEndValue(QPoint(floatingWidget->pos().x(), height() - floatingWidget->height()));
        break;
        case PopupPos::LEFT:
        animator->setStartValue(floatingWidget->pos());
        animator->setEndValue(QPoint(0, floatingWidget->pos().y()));
        break;
        case PopupPos::RIGHT:
        animator->setStartValue(floatingWidget->pos());
        animator->setEndValue(QPoint(width() - floatingWidget->width(), floatingWidget->pos().y()));
        break;
    }

    animationState = PopupAnimationState::HIDE_ANIMATION;

    animator->start();
}
