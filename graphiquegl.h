/****************************************************************************
**  Copyright (c) 2011, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.tk>
**
**  This file is part of the ZeGrapher project.
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

#ifndef GRAPHIQUEGL_H
#define GRAPHIQUEGL_H

#include "Structures.h" // les includes sont dans ce header
#include "calcul.h"




class GraphiqueGl : public QGLWidget
{
    Q_OBJECT
public:
    explicit GraphiqueGl();
    ~GraphiqueGl();

    void afficherPtX(double X);
    void zoomX(double valeur);
    void zoomY(double valeur);
    char curseur();
    void typeCurseurChange(char type);
    double y(int n, double X);
    bool estExistante(int n);
    void setCalculator(Calcul *calculator);
    StructFenetre bornes();


signals:

    void bornesChangent(StructFenetre laFenetre);
    void couleurChange(const QColor &couleur, short pos);
    void zoomBoxActive(bool active);
    void afficherXY(double X, double Y);
    void sizeChanged(int H, int W);



public slots:

    void changerOptions(Options opt);
    void nouvelleFenetre(const StructFenetre &newFenetre);
    void newEtatQuadrillage(bool etat);
    void changerEtatFonction(int numFonc, bool etat);
    void changerCouleurFonction(int numFonc, const QColor &couleur);
    void doNotDraw();

protected:

    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void miseEnTampon(bool toutCalculer, double nbPixels = 0);


    void tracerAxes();
    void tracerCoubres();


    void determinerCentreEtUnites();
    void placerGraduations();
    void afficherPoint();


    QPainter painter;
    QPen pen;
    StructFenetre fenetre;
    Point centre, lastPosSouris, pointPx, pointUnit;
    double uniteX, uniteY, y1, y2, Xmax, Xmin, posXpx, deplacement;
    Fonction fonctions[6];
    bool dispPoint, boutonPresse, sourisSurUneCourbe, courbeClique,
         dispRectangle, nePasCalculer, quadrillage;
    char typeCurseur;
    short mouseX, mouseY, courbeActuelle, courbeSurvole;
    Options parametres;
    QLineF segment;
    QLinkedList<double> listeDesY[6];
    QLinkedList<double>::const_iterator accesseur;
    QSize windowSize;

    Calcul *calculatrice;
    QRect rectReel;



};

#endif // GraphiqueGl_H
