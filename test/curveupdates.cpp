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

int main()
{
  TestApp app;

  // custom bounds, which a coordinate change keeps
  auto* equation = zg::mathWorld->addMathObject(zg::MathObject::EQUATION)->getEquation();
  equation->setEquation("f(x) = x");
  equation->setStart("1");
  equation->setEnd("2");

  auto* column = zg::mathWorld->addMathObject(zg::MathObject::DATASHEET)->getDataSheet()->addColumn();
  column->setName("c");
  column->setStart("1");
  column->setEnd("2");

  int updates = 0;
  QObject::connect(zg::mathWorld, &zg::MathWorld::updated, [&] { updates++; });

  // the graph redraws on MathWorld::updated()
  for (zg::Base* curve: {static_cast<zg::Base*>(equation), static_cast<zg::Base*>(column)})
  {
    updates = 0;
    curve->setCoordinateSystem(zg::CoordinateSystem::Polar);
    app.settle();
    test(updates > 0);

    updates = 0;
    curve->style.setLineWidth(5);
    app.settle();
    test(updates > 0);
  }

  // a pan moves four bounds, and syncs once
  updates = 0;
  information->graphSettings.range.interactiveUpdate(
    {.x = {.min = {-3.}, .max = {4.}}, .y = {.min = {-5.}, .max = {6.}}}, QSizeF(800, 600));
  app.settle();
  test(updates == 1);

  // the objects that use a deleted constant see it leave
  auto* constant = zg::mathWorld->addMathObject(zg::MathObject::CONSTANT);
  constant->getConstant()->setName("a");
  constant->getConstant()->setValue(2);
  equation->setEquation("f(x) = a * x");
  app.settle();
  test(equation->isValid());

  zg::mathWorld->removeMathObject(constant);
  app.settle();
  test(not equation->isValid());

  return 0;
}
