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




#include "Export/imagesave.h"
#include "ui_imagesave.h"

ImageSave::ImageSave(Information *info) : ui(new Ui::ImageSave)
{
    ui->setupUi(this);

    window = info->getGraphRange();

    scene = new ImagePreview(info);
    scene->setMinimumSize(150,150);
    ui->scrollArea->setWidget(scene);


    connect(ui->height, SIGNAL(valueChanged(int)), this, SLOT(setH(int)));
    connect(ui->width, SIGNAL(valueChanged(int)), this, SLOT(setW(int)));
    connect(ui->save, SIGNAL(released()), this, SLOT(save()));
    connect(ui->legendBox, SIGNAL(toggled(bool)), scene, SLOT(setLegendState(bool)));
    connect(ui->legendSize, SIGNAL(valueChanged(int)), scene, SLOT(setlegendFontSize(int)));
    connect(ui->xLegend, SIGNAL(textChanged(QString)), scene, SLOT(setXaxisLegend(QString)));
    connect(ui->yLegend, SIGNAL(textChanged(QString)), scene, SLOT(setYaxisLegend(QString)));
    connect(ui->bold, SIGNAL(toggled(bool)), scene, SLOT(setBold(bool)));
    connect(ui->italic, SIGNAL(toggled(bool)), scene, SLOT(setItalic(bool)));
    connect(ui->underline, SIGNAL(toggled(bool)), scene, SLOT(setUnderline(bool)));
    connect(ui->numPrec, SIGNAL(valueChanged(int)), scene, SLOT(setNumPrec(int)));

}

void ImageSave::save()
{
    scene->repaint();
    QImage *image = scene->drawImage();    
    QString fichier;


    fichier = fileDialog.getSaveFileName(this, tr("Save picture"), QString(), "Images (*.ppm *.tiff *.bmp *.png *.gif *.jpg *.jpeg)");
    if(fichier.isEmpty())
        return;

    else if(!(fichier.endsWith(".ppm") || fichier.endsWith(".tiff") ||
                fichier.endsWith(".bmp") || fichier.endsWith(".png") ||
                fichier.endsWith(".gif") || fichier.endsWith(".jpg") ||
                fichier.endsWith(".jpeg")))
        {
            fichier.append(".png");
        }


    image->save(fichier);
    hide();
}

void ImageSave::setSize(int W, int H)
{
    if(ui->defaultSize->isChecked())
    {
        ui->width->setValue(W);
        ui->height->setValue(H);

        scene->setFixedSize(W, H);
    }
}

void ImageSave::setW(int W)
{
    scene->setFixedWidth(W);
}

void ImageSave::setH(int H)
{
    scene->setFixedHeight(H);
}

void ImageSave::setWindow(ZeGraphView win)
{
    window = win;    
}

void ImageSave::setPrecision(short prec)
{
    precision = prec;
}

ImageSave::~ImageSave()
{
    delete ui;   
}
