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



#ifndef FENETREOPTIONS_H
#define FENETREOPTIONS_H

#include "Structures.h"
#include "informations.h"
#include "Widgets/qcolorbutton.h"


namespace Ui {
    class fenetreoptions;
}

class FenetreOptions : public QWidget
{
    Q_OBJECT

public:
    explicit FenetreOptions(Informations *info);
    ~FenetreOptions();   

private slots:
    void appliquer();

private:

    Informations *informations;
    Ui::fenetreoptions *ui;   
    Options parametres;
    QColorButton *couleurAxes, *couleurFond, *couleurQuadrillage;

};

#endif // FENETREOPTIONS_H
