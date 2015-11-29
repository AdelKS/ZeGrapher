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



#include "Widgets/parconfwidget.h"

ParConfWidget::ParConfWidget(QChar parName, bool withAnimateButton, bool withKeepTracksButton) : QGroupBox(), treeCreator(NORMAL_EXPR)
{
    setTitle(tr("Adjustments for ") + parName + " :");
    keepTracksButtonAvailable = withKeepTracksButton;

    defaultRange.start = 0;
    defaultRange.step = 1;
    defaultRange.end = 0.5;
    keepTracks = NULL;

    QHBoxLayout *widgetsLayout = new QHBoxLayout;
    widgetsLayout->setSpacing(3);
    addConfWidgets(widgetsLayout);

    startTree = endTree = stepTree = NULL;
    kState = valid = isStepGood = isEndGood = isStartGood = false;

    QColor color;
    color.setNamedColor(VALID_COLOR);
    validPalette.setColor(QPalette::Base, color);

    color.setNamedColor(INVALID_COLOR);
    invalidPalette.setColor(QPalette::Base, color);

    if(withAnimateButton)
    {
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->setSpacing(3);

        mainLayout->addLayout(widgetsLayout);

        QHBoxLayout *animateLayout = new QHBoxLayout;
        animate = new QCheckBox(tr("Animate"));

        connect(animate, SIGNAL(toggled(bool)), this, SIGNAL(animateToggled(bool)));

        animateLayout->addWidget(animate);

        if(withKeepTracksButton)
        {
            keepTracks = new QCheckBox(tr("Keep tracks"));
            animateLayout->addWidget(keepTracks);

            treeCreator.allow_k(true);
        }

        animateLayout->addStretch();
        mainLayout->addLayout(animateLayout);
        setLayout(mainLayout);
    }   
    else setLayout(widgetsLayout);
}

void ParConfWidget::setFuncsList(QList<FuncCalculator *> list)
{
     calculator = new ExprCalculator(keepTracksButtonAvailable, list);
}

bool ParConfWidget::doesKeepTracks()
{
    return keepTracks->isChecked();
}

void ParConfWidget::addConfWidgets(QHBoxLayout *layout)
{
    QLabel *startTxt = new QLabel(tr("Start:"));
    QLabel *endTxt = new QLabel(tr(" End:"));
    QLabel *stepTxt = new QLabel(tr(" Step:"));

    start = new QLineEdit;
    start->setMaximumHeight(25);
    connect(start, SIGNAL(textChanged(QString)), this, SLOT(checkExprForK(QString)));
    connect(start, SIGNAL(textChanged(QString)), this, SLOT(resetPaletteForStartLineEdit()));
    connect(start, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    end = new QLineEdit;
    end->setMaximumHeight(25);
    connect(end, SIGNAL(textChanged(QString)), this, SLOT(checkExprForK(QString)));
    connect(end, SIGNAL(textChanged(QString)), this, SLOT(resetPaletteForEndLineEdit()));
    connect(end, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    step = new QLineEdit;
    step->setMaximumHeight(25);
    connect(step, SIGNAL(textChanged(QString)), this, SLOT(checkExprForK(QString)));
    connect(step, SIGNAL(textChanged(QString)), this, SLOT(resetPaletteForStepLineEdit()));
    connect(step, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    layout->addWidget(startTxt);
    layout->addWidget(start);
    layout->addWidget(endTxt);
    layout->addWidget(end);
    layout->addWidget(stepTxt);
    layout->addWidget(step);
}


void ParConfWidget::checkExprForK(QString expr)
{
    bool is_k_present = expr.contains('k');

    if(is_k_present != kState)
        emit k_typed(is_k_present);

    kState = is_k_present;
}

void ParConfWidget::updateTreeWithExpr(QString &lastExpr, QLineEdit *line, FastTree **treePointerAdress, bool &isExprGood)
{
    if(lastExpr != line->text())
    {
        lastExpr = line->text();
        isExprGood = calculator->checkCalledFuncsValidity(lastExpr);

        if(isExprGood)
        {
            if(*treePointerAdress != NULL)
                treeCreator.deleteFastTree(*treePointerAdress);

            *treePointerAdress = treeCreator.getTreeFromExpr(lastExpr, isExprGood);
        }

        if(isExprGood)
            line->setPalette(validPalette);
        else line->setPalette(invalidPalette);
    }
}

void ParConfWidget::resetPaletteForStartLineEdit()
{
    start->setPalette(neutralPalette);
}

void ParConfWidget::resetPaletteForStepLineEdit()
{
    step->setPalette(neutralPalette);
}

void ParConfWidget::resetPaletteForEndLineEdit()
{
    end->setPalette(neutralPalette);
}

void ParConfWidget::validate()
{
    updateTreeWithExpr(lastStartExpr, start, &startTree, isStartGood);
    updateTreeWithExpr(lastEndExpr, end, &endTree, isEndGood);
    updateTreeWithExpr(lastStepExpr, step, &stepTree, isStepGood);

    valid = isStartGood && isStepGood && isEndGood;
}

void ParConfWidget::setAnimationEnabled(bool enabled)
{
    animate->setEnabled(enabled);
    if(keepTracks != NULL)
        keepTracks->setEnabled(enabled);
}

void ParConfWidget::setAnimationChecked(bool checked)
{
    animate->setChecked(checked);
}

bool ParConfWidget::isValid()
{
    return valid || isHidden();
}

bool ParConfWidget::isAnimateChecked()
{
    return animate->isChecked();
}

Range ParConfWidget::getRange(double k)
{
    if(!valid || isHidden())
        return defaultRange;

    Range range;
    calculator->setK(k);

    range.start = calculator->calculateFromTree(startTree);
    range.step = calculator->calculateFromTree(stepTree);
    range.end = calculator->calculateFromTree(endTree);

    return range;
}

ParConfWidget::~ParConfWidget()
{
    if(stepTree != NULL)
        treeCreator.deleteFastTree(stepTree);
    if(endTree != NULL)
        treeCreator.deleteFastTree(endTree);
    if(startTree != NULL)
        treeCreator.deleteFastTree(startTree);

    delete calculator;
}
