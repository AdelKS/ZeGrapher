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

#include "Utils/sampling.h"
#include "app.h"
#include "utils.h"

int main()
{
  TestApp app;

  auto* column = zg::mathWorld->addMathObject(zg::MathObject::DATASHEET)->getDataSheet()->addColumn();

  zg::mathobj::Sampling sampling;
  sampling.setColumn(column);
  sampling.getStartPtr()->setExpression("0");
  sampling.getEndPtr()->setExpression("2");
  sampling.getStepPtr()->setExpression("1");

  // the source can name an object that the user defines later
  sampling.getSourcePtr()->setName("g");
  test(not sampling.getSourcePtr()->isValid());

  auto* g = zg::mathWorld->addMathObject(zg::MathObject::EQUATION);
  g->getEquation()->setEquation("g(x) = 2*x");
  app.settle();
  test(sampling.getSourcePtr()->isValid());

  test(sampling.apply().isEmpty());
  test(column->getData() == std::vector<std::string>{"0", "2", "4"});

  zg::mathWorld->removeMathObject(g);
  app.settle();
  test(not sampling.getSourcePtr()->isValid());

  return 0;
}
