/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "Structures.h"
#include "information.h"
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
    Informations *information;
    bool polar;
    QString abscissa, ordinate;

    QVBoxLayout *layout;
    ModelChoiceWidget *modelChoice;
    PolynomialModelWidget *polynomialModel;

};

#endif // MODELWIDGET_H
