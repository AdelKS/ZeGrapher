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

#include "globals.h"

Globals::Globals()
{
  zg::dataTableModel = &tableModel.emplace(nullptr);
  zg::animationConductor = &conductor.emplace(nullptr);
  zg::mathWorld = &world.emplace(nullptr);
  information = &info.emplace(nullptr);
}

Globals::~Globals()
{
  info.reset();
  information = nullptr;

  world.reset();
  zg::mathWorld = nullptr;

  conductor.reset();
  zg::animationConductor = nullptr;

  tableModel.reset();
  zg::dataTableModel = nullptr;
}
