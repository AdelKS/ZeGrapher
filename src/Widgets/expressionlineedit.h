#ifndef EXPRESSIONLINEEDIT_H
#define EXPRESSIONLINEEDIT_H

#include <QLineEdit>
#include "structures.h"

class ExpressionLineEdit : public QLineEdit
{
public:
    ExpressionLineEdit(QWidget *parent = nullptr);

public slots:
    void clear();
    void setValid();
    void setInvalid();
    void setNeutral();

protected:
    QString validCSS, invalidCSS, neutralCSS;
};

#endif // EXPRESSIONLINEEDIT_H
