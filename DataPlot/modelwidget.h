#ifndef MODELWIDGET_H
#define MODELWIDGET_H

#include <QFrame>
#include <QVBoxLayout>

#include "Structures.h"
#include "informations.h"
#include "./modelchoicewidget.h"
#include "./polynomialmodelwidget.h"

enum State{Choice, Polynomial};

class ModelWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ModelWidget(const QList<Point> &dat, Informations *info, QWidget *parent = 0);
    ~ModelWidget();

public slots:
    void setNewData(const QList<Point> &dat);

protected slots:
    void displaySelectedModel(ModelType type);

signals:
    void removeMe();

private:
    State currentState;
    QList<Point> data;
    Informations *informations;

    QVBoxLayout *layout;
    ModelChoiceWidget *modelChoice;
    PolynomialModelWidget *polynomialModel;

};

#endif // MODELWIDGET_H
