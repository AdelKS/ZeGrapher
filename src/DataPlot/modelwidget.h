/****************************************************************************
**  Copyright (c) 2019, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
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


#ifndef MODELWIDGET_H
#define MODELWIDGET_H

#include <QFrame>
#include <QVBoxLayout>

#include "information.h"
#include "./modelchoicewidget.h"
#include "./polynomialmodelwidget.h"

class ModelWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ModelWidget(const std::weak_ptr<UserData> &userData,
                         QString xname, QString yname, QWidget *parent = nullptr);

public slots:

    void refreshModel();
    void setAbscissaName(QString name);
    void setOrdinateName(QString name);

protected slots:
    void displaySelectedModel(ModelType type);
    void emitRemoveMeSignal();

signals:
    void removeMe(ModelWidget *model);

private:
    enum State {ChoiceWidget, PolynomialWidget};

    State currentState;
    QString abscissa, ordinate;

    std::weak_ptr<const UserData> userData;
    QVBoxLayout *layout;
    ModelChoiceWidget *modelChoice;
    PolynomialModelWidget *polynomialModel;

};

#endif // MODELWIDGET_H
