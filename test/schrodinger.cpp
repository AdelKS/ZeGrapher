/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "app.h"
#include "utils.h"

#include <QCoreApplication>

#include <cmath>

int main()
{
  TestApp app;

  auto* objectA = zg::mathWorld->addMathObject(zg::MathObject::CONSTANT);
  auto* objectB = zg::mathWorld->addMathObject(zg::MathObject::CONSTANT);
  zg::mathobj::Constant* a = objectA->getConstant();
  zg::mathobj::Constant* b = objectB->getConstant();
  a->set_value(2);
  b->set_value(4);

  int changesA = 0, changesB = 0;
  QObject::connect(objectA, &zg::MathObject::schrodingerChanged, [&] { changesA++; });
  QObject::connect(objectB, &zg::MathObject::schrodingerChanged, [&] { changesB++; });

  a->setDeadAndAlive(true);
  test(objectA->isSchrodinger());
  test(std::isnan(a->get_value()));
  test(changesA == 1);

  // B takes over: A gets its value back, and each object notifies once
  b->setDeadAndAlive(true);
  test(zg::mathWorld->getSchrodingerConstant() == b);
  test(not objectA->isSchrodinger() and objectB->isSchrodinger());
  test(a->get_value() == 2);
  test(changesA == 2 and changesB == 1);

  b->setDeadAndAlive(false);
  test(zg::mathWorld->getSchrodingerConstant() == nullptr);
  test(b->get_value() == 4);

  // a deleted Schrödinger constant leaves MathWorld
  a->setDeadAndAlive(true);
  zg::mathWorld->removeMathObject(objectA);
  QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
  test(zg::mathWorld->getSchrodingerConstant() == nullptr);

  return 0;
}
