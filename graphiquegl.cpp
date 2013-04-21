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

#include "graphiquegl.h"


GraphiqueGl::GraphiqueGl()
{
    fenetre.Xmax = fenetre.Ymax = 10;
    fenetre.Xmin = fenetre.Ymin = -10;
    fenetre.Xscale = fenetre.Yscale = 1;

    pen.setCapStyle(Qt::RoundCap);

    courbeActuelle = courbeSurvole = -1;
    parametres.couleurDuFond = QColor(Qt::white);
    parametres.couleurDesAxes = QColor(Qt::black);
    parametres.couleurQuadrillage = QColor(Qt::gray);
    parametres.epaisseurDesCourbes = 1;
    parametres.distanceEntrePoints = 4;
    parametres.lissage = true;

    dispPoint = false;
    boutonPresse = false;
    courbeClique = false;
    sourisSurUneCourbe = false;
    dispRectangle = false;
    nePasCalculer = false;
    quadrillage = false;


    for(short i = 0 ; i < 6; i ++)
    {
        fonctions[i].tracer = false;
        fonctions[i].couleur = QColor(Qt::black);
        fonctions[i].epaisseur = 1;
    }

    setMouseTracking(true);


    typeCurseur = NORMAL;

}

StructFenetre GraphiqueGl::bornes()
{
    return fenetre;
}

bool GraphiqueGl::estExistante(int n)
{
    return fonctions[n].tracer;
}

double GraphiqueGl::y(int n, double X)
{
    return calculatrice->calculate(n, X);
}

char GraphiqueGl::curseur()
{
    return typeCurseur;
}

void GraphiqueGl::newEtatQuadrillage(bool etat)
{
    quadrillage = etat;
    repaint();
}

void GraphiqueGl::setCalculator(Calcul *calculator)
{
    calculatrice = calculator;
}

void GraphiqueGl::miseEnTampon(bool toutcalculate, double nbPixels)
{
    deplacement += nbPixels;
    int limite = ceil(deplacement / parametres.distanceEntrePoints);
    double pas = parametres.distanceEntrePoints / uniteX;
    double x;




    if(toutcalculate)
    {
        deplacement = 0;
        Xmax = fenetre.Xmax + pas;
        Xmin = fenetre.Xmin - pas;
        posXpx = - centre.x - parametres.distanceEntrePoints;


        for(short i = 0; i < 6; i++)
        {
            if(!fonctions[i].tracer)
                continue;
            listeDesY[i].clear();

            for(x = fenetre.Xmin - pas ; x <= fenetre.Xmax + pas ; x += pas)
            {
                listeDesY[i] <<  calculatrice->calculate(i, x);
            }
        }
    }
    else if(fabs(deplacement) > parametres.distanceEntrePoints)
    {
        if(deplacement < 0)
        {
            for(short i = 0 ; i < 6; i++)
            {
                if(!fonctions[i].tracer)
                    continue;

                x = Xmax;
                for(int j = 0; j > limite; j--)
                {
                    x += pas;
                    listeDesY[i].append(calculatrice->calculate(i, x));
                    listeDesY[i].removeFirst();

                }
            }
            deplacement -= parametres.distanceEntrePoints * limite;
            posXpx -= parametres.distanceEntrePoints * limite;//limite ici est negative
            Xmax -= limite * pas;
            Xmin -= limite * pas;

        }
        else
        {
            for(short i = 0 ; i < 6; i++)
            {
                if(!fonctions[i].tracer)
                    continue;
                x = Xmin;
                for(int j = 0; j < limite; j++)
                {
                    x -= pas;
                    listeDesY[i].prepend(calculatrice->calculate(i, x));
                    listeDesY[i].removeLast();
                }
            }
            deplacement -= parametres.distanceEntrePoints * limite;
            posXpx -= parametres.distanceEntrePoints * limite;
            Xmax -= limite * pas;
            Xmin -= limite * pas;
        }
    }
}

void GraphiqueGl::wheelEvent(QWheelEvent *event)
{
    double valeur = abs(event->delta()) / 120;
    valeur = 1 - valeur / 10;

    if(event->delta() < 0)
    {
        fenetre.Xmax /= valeur;
        fenetre.Xmin /= valeur;
        fenetre.Ymax /= valeur;
        fenetre.Ymin /= valeur;
    }
    else
    {
        fenetre.Xmax *= valeur;
        fenetre.Xmin *= valeur;
        fenetre.Ymax *= valeur;
        fenetre.Ymin *= valeur;
    }

    if((fenetre.Xmax - fenetre.Xmin)/fenetre.Xscale > 20)
        fenetre.Xscale *= 2;
    else if((fenetre.Xmax - fenetre.Xmin)/fenetre.Xscale < 5)
        fenetre.Xscale /= 2;

    if((fenetre.Ymax - fenetre.Ymin)/fenetre.Yscale > 20)
        fenetre.Yscale *= 2;
    else if((fenetre.Ymax - fenetre.Ymin)/fenetre.Yscale < 5)
        fenetre.Yscale /= 2;



    emit bornesChangent(fenetre);
    repaint();

}

void GraphiqueGl::afficherPtX(double x)
{
    if(courbeActuelle != -1)
    {
        pointUnit.x = x;
        pointUnit.y = calculatrice->calculate(courbeActuelle, x);
        pointPx.x = x * uniteX;
        pointPx.y = -pointUnit.y * uniteY;
        dispPoint = true;

        if(parametres.lissage)
        {
           pointPx.x++;
        }

        repaint();
    }
}

void GraphiqueGl::paintEvent(QPaintEvent *event)
{
    if(windowSize != size())
    {
        emit sizeChanged(width(), height());
        windowSize = size();
    }

    painter.begin(this);
    //trace du background
    painter.setBrush(QBrush(parametres.couleurDuFond));
    painter.drawRect(0, 0, width(), height());

    determinerCentreEtUnites();
    tracerAxes();

    if(dispRectangle)
    {
        painter.setBrush(Qt::NoBrush);
        pen.setWidth(1);
        pen.setColor(parametres.couleurDesAxes);
        painter.setPen(pen);
        painter.drawRect(rectReel);
    }



    painter.translate(QPointF(centre.x, centre.y));

    placerGraduations();
    if(!nePasCalculer)
        miseEnTampon(true);

    tracerCoubres();

    if(dispPoint)
        afficherPoint();


    nePasCalculer = false;
    painter.end();

}

void GraphiqueGl::determinerCentreEtUnites()
{
    uniteY = height() / (fenetre.Ymax - fenetre.Ymin);
    uniteX = width() / (fenetre.Xmax - fenetre.Xmin);

    centre.x = - fenetre.Xmin * uniteX;
    centre.y =  fenetre.Ymax * uniteY;
}

void GraphiqueGl::afficherPoint()
{
    pen.setColor(Qt::black);
    painter.setPen(pen);

    emit afficherXY(pointUnit.x, pointUnit.y);

    pen.setWidth(parametres.epaisseurDesCourbes);
    painter.setPen(pen);

    painter.setRenderHint(QPainter::Antialiasing);

    double space = parametres.epaisseurDesCourbes;

    painter.drawLine(QPointF(pointPx.x-3-space*2,pointPx.y),QPointF(pointPx.x-space, pointPx.y));
    painter.drawLine(QPointF(pointPx.x+3+space*2,pointPx.y),QPointF(pointPx.x+space, pointPx.y));

    painter.drawLine(QPointF(pointPx.x,pointPx.y-3-space*2),QPointF(pointPx.x, pointPx.y-space));
    painter.drawLine(QPointF(pointPx.x,pointPx.y+3+space*2),QPointF(pointPx.x, pointPx.y+space));

    dispPoint = false;

}

void GraphiqueGl::mousePressEvent(QMouseEvent *event)
{
    boutonPresse = true;
    lastPosSouris.x = event->x();
    lastPosSouris.y = event->y();

    if(typeCurseur == NORMAL)
    {
        if(sourisSurUneCourbe)
        {
            courbeActuelle = courbeSurvole;
            courbeClique = true;
        }
        else
        {
            courbeActuelle = -1;
        }
    }
    else if(typeCurseur == ZOOMER)
    {

    }
    else if(typeCurseur == DEZOOMER)
    {

    }
    else if(typeCurseur == ZOOMBOX)
    {
        dispRectangle = true;
        rectReel.setTopLeft(QPoint(event->x(), event->y()));
    }
    else //DEPLACER
    {
        setCursor(Qt::ClosedHandCursor);
    }


}

void GraphiqueGl::mouseReleaseEvent(QMouseEvent *event)
{
    boutonPresse = false;

    if(typeCurseur == NORMAL)
    {
        if(courbeClique)
        {
            courbeClique = false;
        }
    }
    else if(typeCurseur == ZOOMER)
    {
        double halfX = 0.45 * (fenetre.Xmax - fenetre.Xmin), halfY = 0.45 * (fenetre.Ymax - fenetre.Ymin);
        fenetre.Xmin = (event->x() - centre.x) / uniteX - halfX;
        fenetre.Xmax = (event->x() - centre.x) / uniteX + halfX;
        fenetre.Ymin = (-event->y() + centre.y) / uniteY - halfY;
        fenetre.Ymax = (-event->y() + centre.y) / uniteY + halfY;

        emit bornesChangent(fenetre);
        repaint();
    }
    else if(typeCurseur == DEZOOMER)
    {
        double halfX = (fenetre.Xmax - fenetre.Xmin)/1.8, halfY =  (fenetre.Ymax - fenetre.Ymin)/1.8;
        fenetre.Xmin = (event->x() - centre.x) / uniteX - halfX;
        fenetre.Xmax = (event->x() - centre.x) / uniteX + halfX;
        fenetre.Ymin = (-event->y() + centre.y) / uniteY - halfY;
        fenetre.Ymax = (-event->y() + centre.y) / uniteY + halfY;

        emit bornesChangent(fenetre);
        repaint();
    }
    else if(typeCurseur == ZOOMBOX)
    {
        dispRectangle = false;
        if(!rectReel.isNull())
        {
            fenetre.Xmax = (rectReel.right() - centre.x) / uniteX;
            fenetre.Xmin = (rectReel.left() - centre.x) / uniteX;
            fenetre.Ymax = (- rectReel.top() + centre.y) / uniteY;
            fenetre.Ymin = (- rectReel.bottom() + centre.y) / uniteY;
            emit bornesChangent(fenetre);
            repaint();
        }
    }
    else //DEPLACER
    {
        setCursor(Qt::OpenHandCursor);
    }
}

void GraphiqueGl::mouseMoveEvent(QMouseEvent *event)
{
    mouseX = event->x();
    mouseY = event->y();

    if(typeCurseur == NORMAL)
    {
        double x =  (mouseX - centre.x) / uniteX;

        if(courbeActuelle != -1)
        {
            pointUnit.x = x;
            pointUnit.y = calculatrice->calculate(courbeActuelle, x);
            pointPx.x = x * uniteX;
            pointPx.y = -pointUnit.y * uniteY;
            dispPoint = true;
            nePasCalculer = true;
            update();
        }

        bool sourisSurvoleCourbe = false;
        double calcY = 0;

        for(short i = 0; i < 6; i++)
        {
            if(fonctions[i].tracer == true)
            {
                calcY = calculatrice->calculate(i, x);
                if(isnan(calcY) || isinf(calcY) || i == courbeActuelle)
                    continue;
                calcY = -(calcY * uniteY) + centre.y;
                if(abs(calcY - mouseY) < fonctions[i].epaisseur + 2)
                {
                    sourisSurUneCourbe = sourisSurvoleCourbe = true;
                    fonctions[i].epaisseur = parametres.epaisseurDesCourbes + 2;
                    courbeSurvole = i;
                    nePasCalculer = true;
                    repaint();
                    break;
                }
            }
        }
        if(sourisSurUneCourbe && !sourisSurvoleCourbe)
        {
            fonctions[courbeSurvole].epaisseur = parametres.epaisseurDesCourbes;
            sourisSurUneCourbe = false;
            nePasCalculer = true;
            repaint();
        }

    }
    else if(typeCurseur == ZOOMER)
    {

    }
    else if(typeCurseur == DEZOOMER)
    {

    }
    else if(typeCurseur == ZOOMBOX)
    {
        rectReel.setBottomRight(QPoint(event->x(), event->y()));
        nePasCalculer = true;
        repaint();
    }
    else //DEPLACER
    {
        if(boutonPresse)
        {
            double dx = event->x() - lastPosSouris.x;
            double dy = event->y() - lastPosSouris.y;

            centre.x += dx;
            centre.y += dy;

            lastPosSouris.x = event->x();
            lastPosSouris.y = event->y();

            fenetre.Xmax -= dx / uniteX;
            fenetre.Xmin -= dx / uniteX;

            fenetre.Ymax += dy / uniteY;
            fenetre.Ymin += dy / uniteY;

            emit bornesChangent(fenetre);

            determinerCentreEtUnites();

            if(dx != 0)
               miseEnTampon(false, dx);

            nePasCalculer = true;
            repaint();
        }

    }

}

void GraphiqueGl::placerGraduations()
{
    pen.setWidth(1);
    painter.setRenderHint(QPainter::Antialiasing, false);
    double Xpos = ceil(fenetre.Xmin / fenetre.Xscale) * fenetre.Xscale * uniteX; //* uniteX
    double Ypos, posTxt, pas = fenetre.Xscale * uniteX, minY = 1/uniteY, minX = 1/uniteX;



    //trace sur l'axe des X
    if(fenetre.Ymin >= 0)
    {
        Ypos = - fenetre.Ymin * uniteY - 3;
        posTxt = Ypos - 5;
    }
    else if(fenetre.Ymax <= 0)
    {
        Ypos = - fenetre.Ymax * uniteY + 5;
         posTxt = Ypos + 15;
    }
    else
    {
        Ypos = 0;
        posTxt = Ypos + 15;
    }

    double bas = - fenetre.Ymin * uniteY, haut = - fenetre.Ymax * uniteY;

    while(Xpos <= fenetre.Xmax * uniteX)
    {
        if(fabs(Xpos) > minX)
        {
            if(quadrillage)
            {
                pen.setColor(parametres.couleurQuadrillage);
                painter.setPen(pen);
                painter.drawLine(QPointF(Xpos, bas), QPointF(Xpos, haut));
            }
            pen.setColor(parametres.couleurDesAxes);
            painter.setPen(pen);
            painter.drawLine(QPointF(Xpos, Ypos -3), QPointF(Xpos, Ypos +2));
            painter.drawText(QPointF(Xpos,posTxt), QString::number(Xpos/uniteX));
        }

        Xpos += pas;
    }

//trace sur l'axe des Y

    Ypos = ceil(fenetre.Ymin / fenetre.Yscale) * fenetre.Yscale * uniteY; //* uniteX
    pas = fenetre.Yscale * uniteY;

    bool droite = false;

    if(fenetre.Xmin >= 0)
    {
        Xpos = fenetre.Xmin * uniteX + 3;
        posTxt = Xpos + 5;
    }
    else if(fenetre.Xmax <= 0)
    {
        Xpos = fenetre.Xmax * uniteX - 5;
        posTxt = Xpos - 10;
        droite = true;
    }
    else
    {
        Xpos = 0;
        posTxt = Xpos + 5;
    }

    bas =  fenetre.Xmin * uniteX;
    haut =  fenetre.Xmax * uniteX;

    QString nombre;

    while(Ypos <= fenetre.Ymax * uniteY)
    {
        if(fabs(Ypos) > minY)
        {
            if(quadrillage)
            {
                pen.setColor(parametres.couleurQuadrillage);
                painter.setPen(pen);
                painter.drawLine(QPointF(bas, -Ypos), QPointF(haut, -Ypos));
            }


            pen.setColor(parametres.couleurDesAxes);
            painter.setPen(pen);
            painter.drawLine(QPointF(Xpos -3, -Ypos), QPointF(Xpos +2, -Ypos));

            nombre = QString::number(Ypos/uniteY);
            if(droite)
                painter.drawText(QPointF(posTxt -7*nombre.size(), -Ypos), nombre);
            else painter.drawText(QPointF(posTxt, -Ypos), nombre);
        }

        Ypos += pas;
    }
}

void GraphiqueGl::tracerAxes()
{
    // *********** remarque: les y sont positifs en dessous de l'axe x, pas au dessus !! ************//
    pen.setWidth(1);
    pen.setColor(parametres.couleurDesAxes);
    painter.setPen(pen);

    if(fenetre.Xmax > 0 && fenetre.Xmin < 0)
    {
        painter.drawLine((int)centre.x, 0, (int)centre.x, height());
        //l'axe
        painter.drawLine((int)centre.x - 3, 5, (int)centre.x, 0);
        painter.drawLine((int)centre.x + 3, 5, (int)centre.x, 0);

    }
    if(fenetre.Ymax > 0 && fenetre.Ymin < 0)
    {
        painter.drawLine(0, (int)centre.y, width(),  (int)centre.y);
        //la petite fleche au bout
        painter.drawLine(width() - 5, (int)centre.y + 3 , width()-1, (int)centre.y);
        painter.drawLine(width() - 5, (int)centre.y - 3 , width()-1, (int)centre.y);
    }

}

void GraphiqueGl::tracerCoubres()
{

    double posX = posXpx + parametres.distanceEntrePoints;

    if(parametres.lissage)
        posX ++;

    bool pointDepasse = false;

    if(parametres.lissage)
        painter.setRenderHint(QPainter::Antialiasing);

    for(short i = 0 ; i < 6; i++)
    {
        if(!fonctions[i].tracer)
            continue;
        pen.setWidth(fonctions[i].epaisseur);
        pen.setColor(fonctions[i].couleur);
        painter.setPen(pen);

        double val1, val2;


        for(accesseur = listeDesY[i].begin() + 1; accesseur != listeDesY[i].end(); accesseur++)
        {
            if(!isnan(*(accesseur-1)) && !isnan(*accesseur) && !isinf(*(accesseur-1)) && !isinf(*accesseur))
            {
                val1 = - *(accesseur-1) * uniteY;
                val2 = - *accesseur * uniteY;
                if(*accesseur > fenetre.Ymax || *accesseur < fenetre.Ymin )
                {
                    if(!pointDepasse)
                    {
                        segment.setLine(posX - parametres.distanceEntrePoints, val1, posX, val2);
                        painter.drawLine(segment);
                        pointDepasse = true;
                    }
                }
                else
                {
                    segment.setLine(posX - parametres.distanceEntrePoints, val1, posX, val2);
                    painter.drawLine(segment);
                    pointDepasse = false;
                }
            }

            posX += parametres.distanceEntrePoints;
        }

        posX = posXpx + parametres.distanceEntrePoints;

        if(parametres.lissage)
            posX ++;
    }
}

void GraphiqueGl::zoomX(double valeur)
{
    fenetre.Xmin *= valeur;
    fenetre.Xmax *= valeur;

    if((fenetre.Xmax - fenetre.Xmin)/fenetre.Xscale > 20)
        fenetre.Xscale *= 2;
    else if((fenetre.Xmax - fenetre.Xmin)/fenetre.Xscale < 5)
        fenetre.Xscale /= 2;

    emit bornesChangent(fenetre);
    repaint();
}

void GraphiqueGl::zoomY(double valeur)
{
    fenetre.Ymin *= valeur;
    fenetre.Ymax *= valeur;

    if((fenetre.Ymax - fenetre.Ymin)/fenetre.Yscale > 20)
        fenetre.Yscale *= 2;
    else if((fenetre.Ymax - fenetre.Ymin)/fenetre.Yscale < 5)
        fenetre.Yscale /= 2;

    emit bornesChangent(fenetre);
    nePasCalculer = true;
    repaint();
}

void GraphiqueGl::changerEtatFonction(int numFonc, bool etat)
{
    fonctions[numFonc].tracer = etat;
    repaint();
}

void GraphiqueGl::nouvelleFenetre(const StructFenetre &newFenetre)
{
    fenetre = newFenetre;
    repaint();
}

void GraphiqueGl::doNotDraw()
{
    for(short i = 0 ; i < 6 ; i++)
        fonctions[i].tracer = false;
}

void GraphiqueGl::changerCouleurFonction(int numFonc, const QColor &couleur)
{
    fonctions[numFonc].couleur = couleur;
    nePasCalculer = true;
    repaint();
}

void GraphiqueGl::typeCurseurChange(char type)
 {
     typeCurseur = type;
     if(typeCurseur == NORMAL)
         setCursor(Qt::ArrowCursor);
     else if(typeCurseur == ZOOMER)
         setCursor(QCursor(QPixmap("icons/zoom-in-cursor.png"), 12, 11));
     else if(typeCurseur == DEZOOMER)
         setCursor(QCursor(QPixmap("icons/zoom-out-cursor.png"), 12, 11));
     else if(typeCurseur == ZOOMBOX)
         setCursor(Qt::CrossCursor);
     else setCursor(Qt::OpenHandCursor);

 }

void GraphiqueGl::changerOptions(Options opt)
{
    parametres = opt;
    for(short i = 0 ; i < 6; i++)
    {
        fonctions[i].epaisseur = parametres.epaisseurDesCourbes;
        if(fonctions[i].couleur == parametres.couleurDuFond)
        {
            emit couleurChange(parametres.couleurDesAxes, i);
            fonctions[i].couleur = parametres.couleurDesAxes;
        }
    }
    repaint();
}

GraphiqueGl::~GraphiqueGl()
{

}
