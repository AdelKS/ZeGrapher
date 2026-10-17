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

#include "zc-utils.h"

#include <QObject>

namespace  zg {

QString zcErrorToStr(const zc::Error& err)
{
  // the piece of the expression the error is about. The parser fills it in for
  // every error below that names it, and leaves it empty for the others
  const QString token = QString::fromStdString(err.token.substr).toHtmlEscaped();

  switch (err.type)
  {
    case zc::Error::CPP_INCORRECT_ARGNUM:
      return QObject::tr("Internal error: C++ math function called with the wrong number of arguments");

    case zc::Error::NOT_MATH_OBJECT_DEFINITION:
      return QObject::tr("Not a math object definition, write it as \"name = expression\"");

    case zc::Error::OBJECT_NOT_IN_WORLD:
      return QObject::tr("Object not defined");

    case zc::Error::NAME_ALREADY_TAKEN:
      //: %1 is the name the user wrote
      return QObject::tr("Name \"%1\" already taken").arg(token);

    case zc::Error::UNKNOWN:
      return QObject::tr("Unknown error");

    case zc::Error::WRONG_FORMAT:
      //: %1 is the piece of the expression that has the wrong format
      return QObject::tr("Wrong format: \"%1\"").arg(token);

    case zc::Error::UNEXPECTED:
      //: %1 is the piece of the expression that cannot be there
      return QObject::tr("Unexpected \"%1\"").arg(token);

    case zc::Error::UNEXPECTED_END_OF_EXPRESSION:
      return QObject::tr("Unexpected end of expression");

    case zc::Error::MISSING_CLOSING_PTH:
      return QObject::tr("Missing \")\"");

    case zc::Error::UNDEFINED_VARIABLE:
      //: %1 is the name the user wrote
      return QObject::tr("Undefined variable \"%1\"").arg(token);

    case zc::Error::UNDEFINED_FUNCTION:
      //: %1 is the name the user wrote
      return QObject::tr("Undefined function \"%1\"").arg(token);

    case zc::Error::CALLING_FUN_ARG_COUNT_MISMATCH:
      //: %1 is the name of the function the user called
      return QObject::tr("Function \"%1\" called with the wrong number of arguments").arg(token);

    case zc::Error::NOT_IMPLEMENTED:
      //: %1 is the piece of the expression that ZeGrapher does not compute yet
      return QObject::tr("Not implemented yet: \"%1\"").arg(token);

    case zc::Error::EMPTY_EXPRESSION:
      return QObject::tr("Empty expression");

    case zc::Error::RECURSION_DEPTH_OVERFLOW:
      return QObject::tr("Maximum recursion depth reached");

    case zc::Error::WRONG_OBJECT_TYPE:
      //: %1 is the name the user wrote, used as a variable when it is a function or the other way around
      return QObject::tr("Object \"%1\" used as the wrong type").arg(token);

    case zc::Error::OBJECT_INVALID_STATE:
      //: %1 is the name of an object whose own definition is invalid
      return QObject::tr("Object \"%1\" is in an invalid state").arg(token);

    default:
      return QObject::tr("Unknown error");
  }
}

}
