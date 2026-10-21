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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "initresources.h"

#include <QtGlobal>

extern void qml_register_types_ZeGrapher();

void initResources()
{
  Q_INIT_RESOURCE(resources);
  Q_INIT_RESOURCE(translations);
  Q_INIT_RESOURCE(website);
  Q_INIT_RESOURCE(release_notes);
  Q_INIT_RESOURCE(shaders);
  Q_INIT_RESOURCE(ZeGrapher);
  Q_INIT_RESOURCE(qmlcache_ZeGrapher);
  qml_register_types_ZeGrapher();
}
