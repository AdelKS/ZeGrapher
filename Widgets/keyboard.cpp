/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.0.
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

#include "Widgets/keyboard.h"
#include "ui_keyboard.h"

Keyboard::Keyboard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Keyboard)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool);    
    setAttribute(Qt::WA_X11DoNotAcceptFocus);
    keyboardButtons = new QSignalMapper(this);
    makeKeyboardConnects();
}

void Keyboard::makeKeyboardConnects()
{
    buttons << ui->abs << ui->acos << ui->asin << ui->atan << ui->ceil << ui->cos << ui->divide << ui->dot << ui->eight
            << ui->exp << ui->f << ui->five << ui->floor << ui->four << ui->g << ui->h << ui->ln << ui->log << ui->m
            << ui->minus << ui->multiply << ui->nine << ui->one << ui->p << ui->par_k << ui->pi << ui->plus << ui->power
            << ui->pthf << ui->ptho << ui->r << ui->seven << ui->sin << ui->six << ui->sqrt << ui->tan << ui->three
            << ui->two << ui->var_x << ui->zero << ui->cosh << ui->sinh << ui->tanh << ui->square << ui->var_n;

    for(short i = 0 ; i < buttons.size(); i++)
    {
        connect(buttons[i], SIGNAL(released()), keyboardButtons, SLOT(map()));
        keyboardButtons->setMapping(buttons[i], buttons[i]);
    }

    connect(keyboardButtons, SIGNAL(mapped(QWidget*)), this, SLOT(keyboardPressed(QWidget*)));
    connect(ui->derivative, SIGNAL(toggled(bool)), this, SLOT(changeFuncButtonsText()));
    connect(ui->function, SIGNAL(toggled(bool)), this, SLOT(changeFuncButtonsText()));
    connect(ui->antiderivative, SIGNAL(toggled(bool)), this, SLOT(changeFuncButtonsText()));

    connect(ui->remove, SIGNAL(released()), this, SLOT(removeChar()));
    connect(ui->clear, SIGNAL(released()), this, SLOT(clearLine()));

}

void Keyboard::changeFuncButtonsText()
{
    if(ui->function->isChecked())
    {
        ui->f->setText("f(");
        ui->g->setText("g(");
        ui->h->setText("h(");
        ui->p->setText("p(");
        ui->r->setText("r(");
        ui->m->setText("m(");
    }
    else if(ui->derivative->isChecked())
    {
        ui->f->setText("f'(");
        ui->g->setText("g'(");
        ui->h->setText("h'(");
        ui->p->setText("p'(");
        ui->r->setText("r'(");
        ui->m->setText("m'(");
    }
    else
    {
        ui->f->setText("F(");
        ui->g->setText("G(");
        ui->h->setText("H(");
        ui->p->setText("P(");
        ui->r->setText("R(");
        ui->m->setText("M(");
    }
}

void Keyboard::removeChar()
{
    QWidget *w = qApp->focusWidget();
    if(w != NULL && w->inherits("QLineEdit"))
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(w);
        lineEdit->setText(lineEdit->text().remove(lineEdit->text().size()-1, 1));
    }
}

void Keyboard::clearLine()
{
    QWidget *w = qApp->focusWidget();
    if(w != NULL && w->inherits("QLineEdit"))
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(w);
        lineEdit->clear();
    }
}

void Keyboard::keyboardPressed(QWidget *widget)
{
    QPushButton *button = qobject_cast<QPushButton*>(widget);
    QWidget *w = qApp->focusWidget();
    if(w != NULL && w->inherits("QLineEdit"))
        qobject_cast<QLineEdit*>(w)->insert(button->text());
}

Keyboard::~Keyboard()
{
    delete ui;
}
