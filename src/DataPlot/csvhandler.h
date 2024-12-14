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



#ifndef CSVHANDLER_H
#define CSVHANDLER_H

#include <QtWidgets>

#include "ui_csvconfig.h"


enum Job {CSV_FILE_SAVE, CSV_FILE_OPEN, CSV_NO_FILE};

class CSVhandler : public QDialog
{
    Q_OBJECT


public:
    CSVhandler(QWidget *parent);
    ~CSVhandler();

    void getDataFromCSV();
    void saveCSV(QList<QStringList> data);

signals:
    void dataFromCSV(QList<QStringList>);

protected slots:
    void askForFileLocation();
    void apply();

protected:
    void removeUnnecessaryCells();

    Ui::CSVconfig *ui;
    Job job;
    QList<QStringList> values;
    QFileDialog *fileDialog;

};

#endif // CSVHANDLER_H
