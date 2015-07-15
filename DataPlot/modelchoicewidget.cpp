#include "modelchoicewidget.h"
#include "ui_modelchoicewidget.h"

ModelChoiceWidget::ModelChoiceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModelChoiceWidget)
{
    ui->setupUi(this);
    ui->remove->setIcon(QIcon(":/icons/remove.png"));

    models << PolynomialModel;

    connect(ui->Apply, SIGNAL(released()), this, SLOT(apply()));
    connect(ui->remove, SIGNAL(released()), this, SIGNAL(removeMe()));
}

void ModelChoiceWidget::apply()
{
    emit modelSelected(models.at(ui->modelType->currentIndex()));
}

ModelChoiceWidget::~ModelChoiceWidget()
{
    delete ui;
}
