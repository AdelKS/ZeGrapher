#include "numberlineedit.h"

NumberLineEdit::NumberLineEdit(bool allowK, QList<FuncCalculator*> otherFuncs) : calculator(allowK, otherFuncs)
{
    QColor color;
    color.setNamedColor(VALID_COLOR);
    validPalette.setColor(QPalette::Base, color);

    color.setNamedColor(INVALID_COLOR);
    invalidPalette.setColor(QPalette::Base, color);

    connect(this, SIGNAL(returnPressed()), this, SLOT(checkVal()));
    connect(this, SIGNAL(editingFinished()), this, SLOT(checkVal()));
}

void NumberLineEdit::setNumber(double num)
{
    setText(QString::number(num));
    setPalette(validPalette);
}

void NumberLineEdit::checkVal()
{
    if(text().isEmpty())
    {
        setPalette(neutralPalette);
        return;
    }

    bool ok = true;
    double num = calculator.calculateExpression(text(), ok);

    if(ok)
    {
        setNumber(num);
        emit newVal(num);
    }
    else setPalette(invalidPalette);
}
