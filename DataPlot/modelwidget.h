#ifndef MODELWIDGET_H
#define MODELWIDGET_H

#include <QFrame>
#include <QVBoxLayout>

#include "Structures.h"
#include "informations.h"
#include "./modelchoicewidget.h"
#include "./polynomialmodelwidget.h"

enum State {ChoiceWidget, PolynomialWidget};

class ModelWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ModelWidget(const QList<Point> &dat, Informations *info, bool isPolar, QString xname, QString yname, QWidget *parent = 0);
    ~ModelWidget();

public slots:

    void setPolar(bool state);
    void setData(const QList<Point> &data);
    void setAbscissaName(QString name);
    void setOrdinateName(QString name);

protected slots:
    void displaySelectedModel(ModelType type);
    void emitRemoveMeSignal();

signals:
    void removeMe(ModelWidget *model);

private:
    State currentState;
    QList<Point> data;
    Informations *informations;
    bool polar;
    QString abscissa, ordinate;

    QVBoxLayout *layout;
    ModelChoiceWidget *modelChoice;
    PolynomialModelWidget *polynomialModel;

};

#endif // MODELWIDGET_H
