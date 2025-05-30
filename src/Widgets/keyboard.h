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




#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "structures.h"

namespace Ui {
    class Keyboard;
}

class Keyboard : public QWidget
{
    Q_OBJECT

public:
    explicit Keyboard(QWidget *parent = nullptr);
    ~Keyboard();
     void makeKeyboardConnects();

public slots:
     void keyboardPressed(QObject *widget);
     void funcButtonPressed(QObject *widget);
     void changeFuncButtonsText();
     void removeChar();
     void clearLine();

private:
    Ui::Keyboard *ui;
    QSignalMapper *keyboardButtons;
    QSignalMapper *funcsMapper;

    QList <QPushButton*> buttons;
    QList <QPushButton*> funcButtons;
};

#endif // KEYBOARD_H
