#ifndef MODELCHOICEWIDGET_H
#define MODELCHOICEWIDGET_H

#include <QWidget>

enum ModelType{PolynomialModel};

namespace Ui {
class ModelChoiceWidget;
}

class ModelChoiceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModelChoiceWidget(QWidget *parent = 0);
    ~ModelChoiceWidget();

protected slots:
    void apply();

signals:
    void modelSelected(ModelType type);
    void removeMe();

protected:
    Ui::ModelChoiceWidget *ui;
    QList<ModelType> models;
};

#endif // MODELCHOICEWIDGET_H
