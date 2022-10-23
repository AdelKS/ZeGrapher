#ifndef EXPRESSIONLINEEDIT_H
#define EXPRESSIONLINEEDIT_H

#include <QLineEdit>

class ExpressionLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    enum struct State {NEUTRAL, VALID, INVALID};
    ExpressionLineEdit(QWidget *parent = nullptr);

public slots:
    void clear();
    void setValid();
    void setInvalid();
    void setNeutral();

protected slots:
    void updateBackground();

protected:
    State state = State::NEUTRAL;
    QString validCSS, invalidCSS, neutralCSS;
};

#endif // EXPRESSIONLINEEDIT_H
