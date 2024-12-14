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

#include "datawindow.h"
#include "information.h"
#include "ui_datawindow.h"


DataWindow::DataWindow(const std::shared_ptr<UserData> &userData,
                       QWidget *parent):
    QWidget(parent),
    xindex(STARTING_XPIN_INDEX),
    yindex(STARTING_YPIN_INDEX),
    ui(new Ui::DataWindow),
    columnSelectorSpacer(new QWidget()),
    rowSelectorSpacer(new QWidget()),
    dataTable(new DataTable(STARTING_ROW_COUNT, STARTING_COLUMN_COUNT, ROW_HEIGHT, COLUMN_WIDTH)),
    columnSelector(new ColumnSelectorWidget(STARTING_COLUMN_COUNT, STARTING_XPIN_INDEX, STARTING_YPIN_INDEX, STARTING_SELECTOR_INDEX)),
    columnActionsWidget(new ColumnActionsWidget(dataTable, STARTING_COLUMN_COUNT)),
    rowSelector(new RowSelectorWidget(STARTING_ROW_COUNT)),
    rowActionsWidget(new RowActionsWidget(STARTING_ROW_COUNT)),
    csvHandler(new CSVhandler(this)),
    widgetState(WIDGET_OPENED),
    widgetSplitter(new QSplitter()),
    windowCloseAnimation(new QPropertyAnimation(this, "geometry", this)),
    windowOpenAnimation(new QPropertyAnimation(this, "geometry", this)),
    openAnimation(new QParallelAnimationGroup(this)),
    closeAnimation(new QParallelAnimationGroup(this)),
    modelData(userData)
{
    ui->setupUi(this);
    // must come after ui->setupUi so the members are instanciated,
    widgetCloseAnimation = new QPropertyAnimation(ui->actionsContainerWidget, "maximumWidth", this);
    widgetOpenAnimation = new QPropertyAnimation(ui->actionsContainerWidget, "maximumWidth", this);


    setWindowFlags(Qt::Window);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(ui->retractionButton);

    widgetSplitter->addWidget(ui->actionsContainerWidget);
    widgetSplitter->addWidget(ui->tableContainerWidget);

    mainLayout->addWidget(widgetSplitter);

    ui->modelsScrollArea->setWidget(ui->containerWidget);

    widgetOpenAnimation->setDuration(WIDGET_ANIMATION_TIME);
    widgetOpenAnimation->setEasingCurve(QEasingCurve::OutCubic);

    widgetCloseAnimation->setDuration(WIDGET_ANIMATION_TIME);
    widgetCloseAnimation->setEasingCurve(QEasingCurve::OutCubic);

    windowOpenAnimation->setDuration(WIDGET_ANIMATION_TIME);
    windowOpenAnimation->setEasingCurve(QEasingCurve::OutCubic);

    windowCloseAnimation->setDuration(WIDGET_ANIMATION_TIME);
    windowCloseAnimation->setEasingCurve(QEasingCurve::OutCubic);

    openAnimation->addAnimation(widgetOpenAnimation);
    openAnimation->addAnimation(windowOpenAnimation);

    connect(openAnimation, SIGNAL(finished()), this, SLOT(animationFinished()));

    closeAnimation->addAnimation(widgetCloseAnimation);
    closeAnimation->addAnimation(windowCloseAnimation);

    connect(closeAnimation, SIGNAL(finished()), this, SLOT(animationFinished()));

    connect(ui->retractionButton, SIGNAL(released()), this, SLOT(startAnimation()));

    setWindowTitle(tr("Data fill window"));


    selectorSide = COLUMN_SELECTION;

    QHBoxLayout *columnSelectorLayout = new QHBoxLayout();
    columnSelectorLayout->setContentsMargins(0, 0, 0, 0);
    columnSelectorLayout->setSpacing(0);

    columnSelector->setFixedHeight(COLUMN_SELECTOR_HEIGHT);

    columnSelectorLayout->addWidget(columnSelectorSpacer);
    columnSelectorLayout->addWidget(columnSelector);
    columnSelectorLayout->addStretch();

    ui->tableLayout->addLayout(columnSelectorLayout);

    QVBoxLayout *rowSelectorLayout = new QVBoxLayout();
    rowSelectorLayout->setContentsMargins(0, 0, 0, 0);
    rowSelectorLayout->setSpacing(0);

    rowSelector->setFixedWidth(ROW_SELECTOR_WIDTH);

    rowSelectorLayout->addWidget(rowSelectorSpacer);
    rowSelectorLayout->addWidget(rowSelector);
    rowSelectorLayout->addStretch();


    QHBoxLayout *secondLayout = new QHBoxLayout();
    secondLayout->setContentsMargins(0, 0, 0, 0);
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

    ui->actionsLayout->addWidget(columnActionsWidget);
    ui->actionsLayout->addWidget(rowActionsWidget);

    rowActionsWidget->hide();

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

    connect(columnActionsWidget, SIGNAL(insertColumnClicked(int)), this, SLOT(remakeDataList()));
    connect(columnActionsWidget, SIGNAL(removeColumnClicked(int)), this, SLOT(remakeDataList()));

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

RetractableWidgetState DataWindow::getRetractableWidgetState()
{
    return widgetState;
}

void DataWindow::columnMoved([[maybe_unused]] int logicalIndex,
                             int oldVisualIndex,
                             int newVisualIndex)
{
    if(oldVisualIndex == xindex || oldVisualIndex == yindex || newVisualIndex == xindex || newVisualIndex == yindex)
        remakeDataList();
}

void DataWindow::animationFinished()
{
    if(widgetState == WIDGET_OPENED)
        ui->retractionButton->setIcon(QIcon(":/icons/arrow_right.png"));
    else
    {
        ui->retractionButton->setIcon(QIcon(":/icons/arrow_left.png"));
        ui->actionsContainerWidget->hide();
    }
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
        ui->actionsContainerWidget->show();
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

    ModelWidget *modelWidget = new ModelWidget(modelData, xname, yname);
    modelWidgets << modelWidget;

    ui->modelsLayout->addWidget(modelWidget);

    connect(modelWidget, SIGNAL(removeMe(ModelWidget*)), this, SLOT(removeModelWidget(ModelWidget*)));
    connect(modelWidget, SIGNAL(removeMe(ModelWidget*)), &information, SIGNAL(updateOccured()));
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
    modelData.lock()->cartesian = !polar;

    information.emitDataUpdate();
}

void DataWindow::removeModelWidget(ModelWidget *w)
{
    w->hide();
    modelWidgets.removeOne(w);
    delete w;

    information.emitDataUpdate();
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
    const int &col_x_index = dataTable->colLogicalIndex(xindex);
    const int &col_y_index = dataTable->colLogicalIndex(yindex);

    const std::vector<double> &x_data = *dataTable->get_column_const_it(col_x_index);
    const std::vector<double> &y_data = *dataTable->get_column_const_it(col_y_index);

    auto &dataPoints = modelData.lock()->dataPoints;
    dataPoints.clear();

    for(uint i = 0 ; i < x_data.size() && i < x_data.size() ; i++)
    {
        if(!std::isfinite(x_data[i]) || !std::isfinite(y_data[i]))
            continue;

        dataPoints.emplace_back(Point({x_data[i], y_data[i]}));
    }

    information.emitDataUpdate();
}

void DataWindow::openData()
{
    csvHandler->getDataFromCSV();
}

void DataWindow::saveData()
{
    csvHandler->saveCSV(dataTable->getStringData());
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
            ui->actionsGroupBox->setTitle(tr("Between-two-lines actions:"));
        else ui->actionsGroupBox->setTitle(tr("Line actions:"));
    }
    else
    {
        if(inBetween)
            ui->actionsGroupBox->setTitle(tr("Between-two-columns actions:"));
        else ui->actionsGroupBox->setTitle(tr("Column actions:"));
    }
}

void DataWindow::updateSelectorsSize()
{
    int width = dataTable->getVerticalHeaderSize().width();
    Q_UNUSED(width);
    columnSelectorSpacer->setFixedWidth(ROW_SELECTOR_WIDTH + dataTable->getVerticalHeaderSize().width());
    rowSelectorSpacer->setFixedHeight(dataTable->getHorizontalHeaderSize().height());

    columnSelector->setFixedWidth(dataTable->getColumnCount()*COLUMN_WIDTH);
    rowSelector->setFixedHeight(dataTable->getRowCount()*ROW_HEIGHT);
}

DataWindow::~DataWindow()
{
    if(isVisible())
    {
        QSettings settings;
        settings.setValue("data_window/geometry", geometry());
        settings.setValue("data_window/shrinked_actions_widget", getRetractableWidgetState() == WIDGET_RETRACTED);
    }

    delete dataTable;
    delete csvHandler;
    delete ui;
    delete columnSelector;
    delete columnActionsWidget;
    delete rowSelector;
    delete rowActionsWidget;
}
