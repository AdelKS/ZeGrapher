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

#pragma once

#include "Animation/animationconductor.h"
#include "DataPlot/datatablemodel.h"
#include "information.h"

#include <optional>

/// @brief builds the objects that the whole app shares, and destroys them in
///        reverse order
/// @note a Data column leaves the table model in its destructor, and
///       Information's settings hold math world expressions. So the table model
///       outlives the math world, which outlives Information
/// @note must be instanced after QGuiApplication, and before the QML engine
class Globals
{
public:
  Globals();
  ~Globals();

  Globals(const Globals&) = delete;
  Globals& operator=(const Globals&) = delete;

protected:
  std::optional<zg::DataTableModel> tableModel;
  std::optional<zg::AnimationConductor> conductor;
  std::optional<zg::MathWorld> world;
  std::optional<Information> info;
};
