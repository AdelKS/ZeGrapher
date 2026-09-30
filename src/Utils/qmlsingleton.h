#pragma once

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

#include <QQmlEngine>

#include <type_traits>

namespace zg {

/// @brief exposes a global object to QML as a singleton, from its class's
///        static create()
/// @note the engine deletes singletons it owns, so mark the object as owned by
///       C++
template <class T>
T* qmlSingleton(T* object)
{
  // QQmlPrivate::singletonConstructionMode() prefers the default constructor
  // over create()
  static_assert(not std::is_default_constructible_v<T>,
                "QML would make its own instance instead of calling create()");

  QJSEngine::setObjectOwnership(object, QJSEngine::CppOwnership);
  return object;
}

}
