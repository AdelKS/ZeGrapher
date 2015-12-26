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


#include "./datawindow.h"
#include "ui_datawindow.h"


DataWindow::DataWindow(Information *info, int ind)
{
    index = ind;
    xindex = STARTING_XPIN_INDEX;
    yindex = STARTING_YPIN_INDEX;

    ui = new Ui::DataWindow;
    ui->setupUi(this);

    widgetState = WIDGET_OPENED;

    widgetOpenAnimation = new QPropertyAnimation(ui->actionsContainerWidget, "maximumWidth", this);
    widgetOpenAnimation->setDuration(WIDGET_ANIMATION_TIME);
    widgetOpenAnimation->setEasingCurve(QEasingCurve::OutCubic);

    widgetCloseAnimation = new QPropertyAnimation(ui->actionsContainerWidget, "maximumWidth", this);
    widgetCloseAnimation->setDuration(WIDGET_ANIMATION_TIME);
    widgetCloseAnimation->setEasingCurve(QEasingCurve::OutCubic);

    windowOpenAnimation = new QPropertyAnimation(this, "geometry", this);
    windowOpenAnimation->setDuration(WIDGET_ANIMATION_TIME);
    windowOpenAnimation->setEasingCurve(QEasingCurve::OutCubic);

    windowCloseAnimation = new QPropertyAnimation(this, "geometry", this);
    windowCloseAnimation->setDuration(WIDGET_ANIMATION_TIME);
    windowCloseAnimation->setEasingCurve(QEasingCurve::OutCubic);

    openAnimation = new QParallelAnimationGroup(this);
    openAnimation->addAnimation(widgetOpenAnimation);
    openAnimation->addAnimation(windowOpenAnimation);

    connect(openAnimation, SIGNAL(finished()), this, SLOT(animationFinished()));

    closeAnimation = new QParallelAnimationGroup(this);
    closeAnimation->addAnimation(widgetCloseAnimation);
    closeAnimation->addAnimation(windowCloseAnimation);

    connect(closeAnimation, SIGNAL(finished()), this, SLOT(animationFinished()));

    connect(ui->retractionButton, SIGNAL(released()), this, SLOT(startAnimation()));

    setWindowTitle(tr("Data fill window: data") + QString::number(ind+1));

    information = info;
    selectorSide = COLUMN_SELECTION;

    dataTable = new DataTable(info, STARTING_ROW_COUNT, STARTING_COLUMN_COUNT, ROW_HEIGHT, COLUMN_WIDTH);
    columnSelector = new ColumnSelectorWidget(STARTING_COLUMN_COUNT, STARTING_XPIN_INDEX, STARTING_YPIN_INDEX, STARTING_SELECTOR_INDEX);
    columnActionsWidget = new ColumnActionsWidget(dataTable, info, STARTING_COLUMN_COUNT);
    rowSelector = new RowSelectorWidget(STARTING_ROW_COUNT);
    rowActionsWidget = new RowActionsWidget(STARTING_ROW_COUNT);


    QHBoxLayout *columnSelectorLayout = new QHBoxLayout();
    columnSelectorLayout->setMargin(0);
    columnSelectorLayout->setSpacing(0);

    columnSelectorSpacer = new QWidget();
    columnSelector->setFixedHeight(COLUMN_SELECTOR_HEIGHT);

    columnSelectorLayout->addWidget(columnSelectorSpacer);
    columnSelectorLayout->addWidget(columnSelector);
    columnSelectorLayout->addStretch();

    ui->tableLayout->addLayout(columnSelectorLayout);

    QVBoxLayout *rowSelectorLayout = new QVBoxLayout();
    rowSelectorLayout->setMargin(0);
    rowSelectorLayout->setSpacing(0);

    rowSelectorSpacer = new QWidget();
    rowSelector->setFixedWidth(ROW_SELECTOR_WIDTH);

    rowSelectorLayout->addWidget(rowSelectorSpacer);
    rowSelectorLayout->addWidget(rowSelector);
    rowSelectorLayout->addStretch();


    QHBoxLayout *secondLayout = new QHBoxLayout();
    secondLayout->setMargin(0);
    secondLayout->setSpacing(0);

    secondLayout->addLayout(rowSelectorLayout);
    secondLayout->addWidget(dataTable);


    ui->tableLayout->addLayout(secondLayout);
    ui->tableLayout->addStretch();

    updateSelectorsSize();

    connect(dataTable, SIGNAL(newPosCorrections()), this, SLOT(updateSelectorsSize()));
    connect(dataTable, SIGNAL(newColumnCount(int)), this, SLOT(updateSelectorsSize()));
    connect(dataTable, SIGNAL(newRowCount(int)), this, SLOT(updateSelectorsSize()));

    columnSelector->updateSelectorsPos();
    rowSelector->updateSelectorsPos();

    columnSelectorSpacer->setFixedWidth(dataTable->getVerticalHeaderSize().width());
    rowSelectorSpacer->setFixedHeight(dataTable->getHorizontalHeaderSize().height());

    ui->actionsLayout->addWidget(columnActionsWidget);
    ui->actionsLayout->addWidget(rowActionsWidget);

    rowActionsWidget->hide();

    csvHandler = new CSVhandler(this);
    connect(csvHandler, SIGNAL(dataFromCSV(QList<QStringList>)), dataTable, SLOT(addData(QList<QStringList>)));

    connect(ui->open, SIGNAL(released()), this, SLOT(openData()));
    connect(ui->save, SIGNAL(released()), this, SLOT(saveData()));


    connect(columnSelector, SIGNAL(newSelectorPos(bool,int)), columnActionsWidget, SLOT(setSelectorPos(bool,int)));
    connect(columnSelector, SIGNAL(askForSelector()), rowSelector, SLOT(askedForSelector()));
    connect(columnSelector, SIGNAL(askForSelector()), this, SLOT(selectorInColumnSelection()));
    connect(columnSelector, SIGNAL(newSelectorPos(bool,int)), this, SLOT(selectorPosChanged(bool,int)));
    connect(columnSelector, SIGNAL(newXIndex(int)), this, SLOT(dataChanged()));
    connect(columnSelector, SIGNAL(newYIndex(int)), this, SLOT(dataChanged()));

    connect(rowSelector, SIGNAL(askForSelector()), this, SLOT(selectorInRowSelection()));
    connect(rowSelector, SIGNAL(askForSelector()), columnSelector, SLOT(askedForSelector()));
    connect(rowSelector, SIGNAL(newIndex(bool,int)), this, SLOT(selectorPosChanged(bool,int)));  
    connect(rowSelector, SIGNAL(newIndex(bool,int)), rowActionsWidget, SLOT(setSelectorPos(bool,int)));

    connect(rowActionsWidget, SIGNAL(insertRowClicked(int)), dataTable, SLOT(insertRow(int)));
    connect(rowActionsWidget, SIGNAL(removeRowClicked(int)), dataTable, SLOT(removeRow(int))); 

    connect(columnActionsWidget, SIGNAL(insertColumnClicked(int)), dataTable, SLOT(insertColumn(int)));
    connect(columnActionsWidget, SIGNAL(removeColumnClicked(int)), dataTable, SLOT(removeColumn(int)));

    connect(dataTable, SIGNAL(newColumnCount(int)), columnSelector, SLOT(setColumnCount(int)));
    connect(dataTable, SIGNAL(newColumnCount(int)), columnActionsWidget, SLOT(setColumnCount(int)));
    connect(dataTable, SIGNAL(newRowCount(int)), rowSelector, SLOT(setRowCount(int)));
    connect(dataTable, SIGNAL(newRowCount(int)), rowActionsWidget, SLOT(setRowCount(int)));
    connect(dataTable, SIGNAL(valEdited(int,int)), this, SLOT(cellValChanged(int,int)));
    connect(dataTable, SIGNAL(newColumnName(int)), this, SLOT(columnNameChanged(int)));
    connect(dataTable, SIGNAL(columnMoved(int,int,int)), this, SLOT(columnMoved(int,int,int)));


    connect(ui->cartesian, SIGNAL(toggled(bool)), columnSelector, SLOT(setCoordinateSystem(bool)));
    connect(ui->cartesian, SIGNAL(toggled(bool)), this, SLOT(remakeDataList()));
    //connect to row actions widget

    connect(ui->addModel, SIGNAL(released()), this, SLOT(addModel()));
    connect(ui->polar, SIGNAL(toggled(bool)), this, SLOT(coordinateSystemChanged(bool)));


    connect(ui->help, SIGNAL(released()), this, SIGNAL(showHelpWindow()));

}

void DataWindow::shrinkActionsWidgetContainer()
{
    widgetState = WIDGET_RETRACTED;
    ui->actionsContainerWidget->setMaximumWidth(0);
    ui->retractionButton->setIcon(QIcon(":/icons/arrow_left.png"));
    animation_width = 360;
    //taken from datawindow.ui's actionsContainerWidget. It's not the best way to do it, but it's the first thing that came to my mind...

}

RetractableWidgetState DataWindow::getRetractableWidgetState()
{
    return widgetState;
}

void DataWindow::columnMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex);
    Q_UNUSED(newVisualIndex);

    if(oldVisualIndex == xindex || oldVisualIndex == yindex || newVisualIndex == xindex || newVisualIndex == yindex)
        remakeDataList();
}

void DataWindow::animationFinished()
{
    if(widgetState == WIDGET_OPENED)
        ui->retractionButton->setIcon(QIcon(":/icons/arrow_right.png"));
    else ui->retractionButton->setIcon(QIcon(":/icons/arrow_left.png"));

}

void DataWindow::startAnimation()
{
    QRect rect;

    if(widgetState == WIDGET_OPENED)
    {
        widgetState = WIDGET_RETRACTED;

        closeAnimation->stop();

        animation_width = ui->actionsContainerWidget->width();

        widgetCloseAnimation->setStartValue(QVariant(animation_width));
        widgetCloseAnimation->setEndValue(QVariant(0));

        rect = geometry();
        rect.setWidth(rect.width() - animation_width);
        rect.translate(animation_width, 0);

        windowCloseAnimation->setStartValue(QVariant(geometry()));
        windowCloseAnimation->setEndValue(QVariant(rect));

        closeAnimation->start();
    }
    else // WIDGET_RETRACTED
    {
        widgetState = WIDGET_OPENED;

        openAnimation->stop();

        widgetOpenAnimation->setStartValue(QVariant(0));
        widgetOpenAnimation->setEndValue(QVariant(animation_width));

        rect = geometry();
        rect.setWidth(rect.width() + animation_width);
        rect.translate(- animation_width, 0);

        windowOpenAnimation->setStartValue(QVariant(geometry()));
        windowOpenAnimation->setEndValue(QVariant(rect));

        openAnimation->start();
    }
}

void DataWindow::addModel()
{
    QString xname = dataTable->getColumnName(xindex);
    QString yname = dataTable->getColumnName(yindex);

    ModelWidget *modelWidget = new ModelWidget(modelData, information, ui->polar->isChecked(), xname, yname);
    modelWidgets << modelWidget;

    ui->modelsLayout->addWidget(modelWidget);

    connect(modelWidget, SIGNAL(removeMe(ModelWidget*)), this, SLOT(removeModelWidget(ModelWidget*)));
    connect(modelWidget, SIGNAL(removeMe(ModelWidget*)), information, SIGNAL(updateOccured()));
}

void DataWindow::columnNameChanged(int index)
{
    if(index == xindex)
        for(int i = 0 ; i < modelWidgets.size() ; i++)
            modelWidgets[i]->setAbscissaName(dataTable->getColumnName(xindex));
    else if(index == yindex)
        for(int i = 0 ; i < modelWidgets.size() ; i++)
            modelWidgets[i]->setOrdinateName(dataTable->getColumnName(yindex));
}

void DataWindow::displayHelp()
{

}

void DataWindow::coordinateSystemChanged(bool polar)
{
    for(int i = 0 ; i < modelWidgets.size() ; i++)
        modelWidgets[i]->setPolar(polar);
}

void DataWindow::removeModelWidget(ModelWidget *w)
{
    w->hide();
    modelWidgets.removeOne(w);
    delete w;

    information->emitDataUpdate();
}

void DataWindow::changeIndex(int ind)
{
    index = ind;
    setWindowTitle(tr("Data fill window: data") + QString::number(ind+1));
}

void DataWindow::dataChanged()
{
    if(!(xindex == columnSelector->getXindex() && yindex == columnSelector->getYindex()))
    {
        xindex = columnSelector->getXindex();
        yindex = columnSelector->getYindex();
        remakeDataList();
    }
}

void DataWindow::cellValChanged(int row, int col)
{
    Q_UNUSED(row);

    if(col == xindex || col == yindex)
        remakeDataList();
}

void DataWindow::remakeDataList()
{
    const QList<QList<double> > &values = dataTable->getValues();
    QList<QPointF> dataList;
    QPointF point;

    Point dataPt;

    modelData.clear();

    int logicalX = dataTable->colLogicalIndex(xindex);
    int logicalY = dataTable->colLogicalIndex(yindex);

    for(int row = 0 ; row < values[0].size(); row++)
    {
        if(!std::isnan(values[logicalX][row]) && !std::isnan(values[logicalY][row]))
        {
            if(ui->polar->isChecked())
            {               
                point.setX(values[logicalY][row] * cos( values[logicalX][row]));
                point.setY(values[logicalY][row] * sin( values[logicalX][row]));
            }
            else
            {
                point.setX(values[logicalX][row]);
                point.setY(values[logicalY][row]);
            }

            dataList << point;

            dataPt.x = values[logicalX][row];
            dataPt.y = values[logicalY][row];

            modelData << dataPt;
        }
    }

    for(int i = 0 ; i < modelWidgets.size() ; i++)
    {
        modelWidgets[i]->setData(modelData);
        modelWidgets[i]->setAbscissaName(dataTable->getColumnName(xindex));
        modelWidgets[i]->setOrdinateName(dataTable->getColumnName(yindex));
    }


    information->setData(index, dataList);
}

void DataWindow::openData()
{
    csvHandler->getDataFromCSV();
}

void DataWindow::saveData()
{
    csvHandler->saveCSV(dataTable->getData());
}

void DataWindow::selectorInColumnSelection()
{
    selectorSide = COLUMN_SELECTION;    

    columnActionsWidget->show();
    rowActionsWidget->hide();
}

void DataWindow::selectorInRowSelection()
{
    selectorSide = ROW_SELECTION;    

    columnActionsWidget->hide();
    rowActionsWidget->show();
}

void DataWindow::selectorPosChanged(bool inBetween, int index)
{
    Q_UNUSED(index);

    if(selectorSide == ROW_SELECTION)
    {
        if(inBetween)        
            ui->actionsGroupBox->setTitle(tr("Between two lines:"));        
        else ui->actionsGroupBox->setTitle(tr("Actions on line:"));
    }
    else
    {
        if(inBetween)        
            ui->actionsGroupBox->setTitle(tr("Between two columns:"));        
        else ui->actionsGroupBox->setTitle(tr("Column actions:"));
    }
}

void DataWindow::updateSelectorsSize()
{
    columnSelectorSpacer->setFixedWidth(ROW_SELECTOR_WIDTH + dataTable->getVerticalHeaderSize().width());
    rowSelectorSpacer->setFixedHeight(dataTable->getHorizontalHeaderSize().height());

    columnSelector->setFixedWidth(dataTable->getColumnCount()*COLUMN_WIDTH);
    rowSelector->setFixedHeight(dataTable->getRowCount()*ROW_HEIGHT);
}

DataWindow::~DataWindow()
{
    delete dataTable;
    delete csvHandler;
}
