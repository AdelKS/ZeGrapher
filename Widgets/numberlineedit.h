#ifndef NUMBERLINEEDIT_H
#define NUMBERLINEEDIT_H

#include <QLineEdit>

#include "Calculus/exprcalculator.h"
#include "Calculus/funccalculator.h"

class NumberLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    NumberLineEdit(bool allowK = false, QList<FuncCalculator*> otherFuncs = QList<FuncCalculator*>());
    void setNumber(double num);

protected slots:
    void checkVal();

signals:
    void newVal(double val);

protected:
    ExprCalculator calculator;
    QPalette validPalette, invalidPalette, neutralPalette;
};

#endif // NUMBERLINEEDIT_H
