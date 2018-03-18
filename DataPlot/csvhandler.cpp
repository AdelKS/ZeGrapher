/****************************************************************************
**  Copyright (c) 2016, Adel Kara Slimane <adel.ks@zegrapher.com>
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



#include "csvhandler.h"

CSVhandler::CSVhandler(QWidget *parent): QDialog(parent), ui(new Ui::CSVconfig)
{
    ui->setupUi(this);

    fileDialog = new QFileDialog(this);
    fileDialog->setNameFilter(tr("Data (*.csv)"));    

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
    setWindowTitle(tr("Open data from file"));
    ui->apply->setText(tr("Open"));
    ui->fileLocation->clear();
    job = CSV_FILE_OPEN;
     fileDialog->setAcceptMode(QFileDialog::AcceptOpen);

    exec();
}

void CSVhandler::saveCSV(QList<QStringList> data)
{
    values = data;
    removeUnnecessaryCells();

    setWindowTitle(tr("Save data"));
    ui->apply->setText(tr("Save"));
    ui->fileLocation->clear();
    job = CSV_FILE_SAVE;
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);

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
        QMessageBox::warning(this, tr("Error"), tr("Target file was not specified."));
        return;
    }

    if(ui->delimiter->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Error"), tr("Separator was not specified."));
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
