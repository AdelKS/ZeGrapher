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



#include "Widgets/keyboard.h"
#include "ui_keyboard.h"

Keyboard::Keyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Keyboard)
{
    ui->setupUi(this);

    ui->backspace->setIcon(QIcon(":/icons/backspace.png"));
    ui->clear->setIcon(QIcon(":/icons/clear.png"));

    setWindowFlags(Qt::Tool | Qt::WindowDoesNotAcceptFocus /**| Qt::WindowStaysOnTopHint**/);
    setWindowIcon(QIcon(":/icons/keyboard.png"));
    keyboardButtons = new QSignalMapper(this);
    funcsMapper = new QSignalMapper(this);
    makeKeyboardConnects();
}

void Keyboard::makeKeyboardConnects()
{
    buttons << ui->divide << ui->dot << ui->eight << ui->five << ui->four << ui->minus
            << ui->multiply << ui->nine << ui->one << ui->par_k << ui->pi << ui->plus << ui->power
            << ui->pthf << ui->ptho << ui->seven  << ui->six << ui->three
            << ui->two << ui->var_x << ui->zero << ui->square << ui->var_n ;

    funcButtons << ui->abs << ui->acos << ui->asin << ui->atan << ui->ceil << ui->cos << ui->exp << ui->f << ui->floor
                   << ui->g << ui->h << ui->ln << ui->log << ui->cosh << ui->sinh << ui->tanh << ui->sin << ui->sqrt << ui->tan
                   << ui->acosh << ui->asinh << ui->atanh << ui->erf << ui->erfc << ui->gamma << ui->m << ui->r << ui->p;

    for(short i = 0 ; i < buttons.size(); i++)
    {
        connect(buttons[i], SIGNAL(released()), keyboardButtons, SLOT(map()));
        keyboardButtons->setMapping(buttons[i], buttons[i]);
    }

    for(short i = 0 ; i < funcButtons.size(); i++)
    {
        connect(funcButtons[i], SIGNAL(released()), funcsMapper, SLOT(map()));
        funcsMapper->setMapping(funcButtons[i], funcButtons[i]);
    }

    connect(keyboardButtons, SIGNAL(mapped(QWidget*)), this, SLOT(keyboardPressed(QWidget*)));
    connect(funcsMapper, SIGNAL(mapped(QWidget*)), this, SLOT(funcButtonPressed(QWidget*)));

    connect(ui->derivative, SIGNAL(toggled(bool)), this, SLOT(changeFuncButtonsText()));
    connect(ui->function, SIGNAL(toggled(bool)), this, SLOT(changeFuncButtonsText()));
    connect(ui->antiderivative, SIGNAL(toggled(bool)), this, SLOT(changeFuncButtonsText()));

    connect(ui->backspace, SIGNAL(released()), this, SLOT(removeChar()));
    connect(ui->clear, SIGNAL(released()), this, SLOT(clearLine()));

}

void Keyboard::changeFuncButtonsText()
{
    if(ui->function->isChecked())
    {
        ui->f->setText("f");
        ui->g->setText("g");
        ui->h->setText("h");
        ui->p->setText("p");
        ui->r->setText("r");
        ui->m->setText("m");
    }
    else if(ui->derivative->isChecked())
    {
        ui->f->setText("f'");
        ui->g->setText("g'");
        ui->h->setText("h'");
        ui->p->setText("p'");
        ui->r->setText("r'");
        ui->m->setText("m'");
    }
    else
    {
        ui->f->setText("F");
        ui->g->setText("G");
        ui->h->setText("H");
        ui->p->setText("P");
        ui->r->setText("R");
        ui->m->setText("M");
    }
}

void Keyboard::removeChar()
{
    QWidget *w = qApp->focusWidget();
    if(w != nullptr && w->inherits("QLineEdit"))
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(w);
        lineEdit->setText(lineEdit->text().remove(lineEdit->text().size()-1, 1));
    }
}

void Keyboard::clearLine()
{
    QWidget *w = qApp->focusWidget();
    if(w != nullptr && w->inherits("QLineEdit"))
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(w);
        lineEdit->clear();
    }
}

void Keyboard::keyboardPressed(QWidget *widget)
{
    QPushButton *button = qobject_cast<QPushButton*>(widget);
    QWidget *w = qApp->focusWidget();
    if(w != nullptr && w->inherits("QLineEdit"))
        qobject_cast<QLineEdit*>(w)->insert(button->text());
}

void Keyboard::funcButtonPressed(QWidget *widget)
{
    QPushButton *button = qobject_cast<QPushButton*>(widget);
    QWidget *w = qApp->focusWidget();
    if(w != nullptr && w->inherits("QLineEdit"))
        qobject_cast<QLineEdit*>(w)->insert(button->text() + "(");
}

Keyboard::~Keyboard()
{
    delete ui;
}
