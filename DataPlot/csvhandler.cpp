/****************************************************************************
**  Copyright (c) 2014, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.1.
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

#include "csvhandler.h"

CSVhandler::CSVhandler(QWidget *parent): QDialog(parent), ui(new Ui::CSVconfig)
{
    ui->setupUi(this);

    fileDialog = new QFileDialog(this);
    fileDialog->setNameFilter(tr("Données (*.csv)"));

    job = CSV_NO_FILE;

    setWindowIcon(QIcon(":/icons/csv.png"));

    connect(ui->apply, SIGNAL(released()), this, SLOT(apply()));
    connect(ui->askForFileLocation, SIGNAL(released()), this, SLOT(askForFileLocation()));
    connect(ui->cancel, SIGNAL(released()), this, SLOT(close()));
}

void CSVhandler::askForFileLocation()
{
    if(fileDialog->exec())
    {
        ui->fileLocation->setText(fileDialog->selectedFiles().first());
        if(job == CSV_FILE_SAVE && !ui->fileLocation->text().endsWith(".csv"))
            ui->fileLocation->setText(ui->fileLocation->text()+".csv");

    }
}

void CSVhandler::getDataFromCSV()
{
    setWindowTitle(tr("Ouvrir données"));
    ui->apply->setText(tr("Ouvrir"));
    ui->fileLocation->clear();
    job = CSV_FILE_OPEN;
    fileDialog->setFileMode(QFileDialog::ExistingFile);

    exec();
}

void CSVhandler::saveCSV(QList<QStringList> data)
{
    values = data;
    removeUnnecessaryCells();

    setWindowTitle(tr("Enregistrer données"));
    ui->apply->setText(tr("Enregistrer"));
    ui->fileLocation->clear();
    job = CSV_FILE_SAVE;
    fileDialog->setFileMode(QFileDialog::AnyFile);

    exec();
}

void CSVhandler::removeUnnecessaryCells()
{
    bool unnecessary = true;
    int col = 0, row = 1;

    for(row = 0 ; row < values.size(); row++)
        for(col = 0 ; col < values[row].size(); col++)
            if(values[row][col] == " ")
                values[row][col].clear();

    //start by removing unnecessary columns, at first, "values" is a square matrix, that's why it's possbile to ask for values[0].size()
    col = 0;
    row = 1;

    while(col < values[0].size())
    {
        unnecessary = true;
        for(row = 1 ; row < values.size() && unnecessary ; row++)
            unnecessary = values[row][col].isEmpty();

        if(unnecessary)
            for(row = 0 ; row < values.size() ; row++)
                values[row].removeAt(col);
        else col++;
    }

    //removing unnecessary rows

    row = 0;

    while(row < values.size())
    {
        unnecessary = true;
        for(col = 0 ; col < values[0].size() && unnecessary ; col++)
            unnecessary = values[row][col].isEmpty();

        if(unnecessary)
            values.removeAt(row);
        else row++;
    }

    //removing empty cells at the end of each line

    for(row = 0 ; row < values.size(); row++)
        while(values[row].last().isEmpty()) {values[row].removeLast();}
}

void CSVhandler::apply()
{
    if(ui->fileLocation->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Fichier cible non spécifié."));
        return;
    }

    if(ui->delimiter->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Séparateur non spécifié."));
        return;
    }


    if(job == CSV_FILE_SAVE)
    {
        QFile file(ui->fileLocation->text());
        if(file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
        {
            QTextStream out(&file);
            for(int i = 0 ; i < values.size(); i++)
                out << values[i].join(ui->delimiter->text()) << '\n';

            file.close();
        }
    }
    else if(job == CSV_FILE_OPEN)
    {
        QFile file(ui->fileLocation->text());
        values.clear();

        if(file.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&file);
            while(!in.atEnd())
                values << in.readLine().split(ui->delimiter->text());

            file.close();
            emit dataFromCSV(values);
        }
    }

    close();
}
