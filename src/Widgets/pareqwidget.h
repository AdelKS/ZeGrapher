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




#ifndef PAREQWIDGET_H
#define PAREQWIDGET_H

#include <QWidget>

#include "Widgets/parconfwidget.h"
#include "Calculus/funccalculator.h"
#include "Calculus/treecreator.h"
#include "Calculus/exprcalculator.h"
#include "Widgets/qcolorbutton.h"
#include "Calculus/colorsaver.h"

class ParEqWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParEqWidget(int num, QList<FuncCalculator*> list, QColor col);
    ~ParEqWidget();

    int getCurrentKPos();
    int getCurrentTPos();

    bool is_t_Animated();
    bool isAnimated();
    bool getDrawState();
    bool keepTracks();   
    bool isPlaying();
    bool isValid();
    bool isParEqParametric();

    void apply();
    void changeID(int newID);
    void nextFrame();
    void setRatio(double r);

    ColorSaver* getColorSaver();

    Range getKRange();
    Range getTRange(double k = 0);

    ParEqValues getParEqValues(Range t_range, double k = 0);
    Point getPoint(double t, double k = 0);

    QList< QList<Point> >* getPointsList();
    QList< QList<Point> >* getCurrentPolygon();    

signals:
    void removeClicked(ParEqWidget *widget);  
    void updateRequest();
    void animationUpdateRequest();
    void returnPressed();

protected slots:
    void emitRemoveSignal();
    void playButtonClicked();
    void animateTtoggled(bool checked);
    void animateKtoggled(bool checked);
    void set_tRange_parametric(bool parametric);
    void checkExpr();
    void sliderMoved(int pos);
    void recalculatePointsList();
    void loopFromStartToggled();
    void loopRoundToggled();    

protected:
    void createWidgets(QColor col);
    void addExprWidgets(QColor col);
    void addTConfWidgets();
    void addKConfWidgets();
    void addAnimationControllWidgets();
    bool areIdentical(Range a, Range b);   
    void updateAnimationSlider();
    void calculatePointsList();   
    void nextFrameTchecked();
    void nextFrameKchecked();

    void checkXline();
    void checkYline();
    void updateTRange(double k);
    void updateKRange();


    ParametricInfo getParInfo(bool &ok);

    QLineEdit *xLine, *yLine;
    QPushButton *play, *loopFromStart, *loopRound;
    QSlider *parSlider;
    QCheckBox *drawCheckBox;
    QLabel *parCharLabel,*parEqNameLabel;
    QLineEdit *parCurrentValLineEdit;   
    QVBoxLayout *mainLayout;
    QWidget *animationControlWidget;
    ParConfWidget *tWidget, *kWidget;
    TreeCreator treeCreator;
    ExprCalculator *calculator;
    ColorSaver colorSaver;
    QColorButton *lastColorButton;
    QPalette validPalette, invalidPalette;

    int index, curvesNum_original, curvesNum_current, current_tPos, current_pos, current_kPos, tPos_end;

    bool valid, playState, is_t_range_parametric, are_expr_parametric,
         isParametric, isTRangeGood, isKRangeGood, isXExprGood, isYExprGood,
         hasSomethingChanged, blockAnimation;

    double current_t, current_k, ratio;
    short increment;    
    QList< QList<Point> > currentPolygon;
    QList< QList<Point> > pointsList;
    QList<FuncCalculator*> funcCalcs;
    QString xExpr, yExpr;
    Range tRange, kRange;
    FastTree *xTree, *yTree;


};

#endif // PAREQWIDGET_H
