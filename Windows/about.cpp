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

#include "Windows/about.h"
#include "ui_about.h"
#include "structures.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    QPixmap logo(":/icons/ZeGrapher.png");
    logo = logo.scaledToWidth(128, Qt::SmoothTransformation);

    ui->logo->setPixmap(logo);

    ui->versionLabel->setText(QString("<span style='font-size: 15px;'><strong>") + SOFTWARE_VERSION_STR + QString("</strong></span>"));
    setWindowIcon(QIcon(":/icons/ZeGrapher.png"));
}

About::~About()
{
    delete ui;
}
