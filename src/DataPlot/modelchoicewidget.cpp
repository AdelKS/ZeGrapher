/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/


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
