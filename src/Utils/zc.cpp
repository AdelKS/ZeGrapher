#include "zc.h"

#include <QObject>

namespace  zg {

QString zcErrorToStr(const zc::Error& err)
{
  switch (err.type)
  {
    case zc::Error::CPP_INCORRECT_ARGNUM:
      return QObject::tr("Programmatically calling C++ math function with wrong number of arguments");

    case zc::Error::NOT_MATH_OBJECT_DEFINITION:
      return QObject::tr("Not a correct math object definition");

    case zc::Error::OBJECT_NOT_IN_WORLD:
      return QObject::tr("Object not registered in current math world");

    case zc::Error::NAME_ALREADY_TAKEN:
      return QObject::tr("Name already taken");

    case zc::Error::UNKNOWN:
      return QObject::tr("Unkown error");

    case zc::Error::WRONG_FORMAT:
      return QObject::tr("Wrong format");

    case zc::Error::UNEXPECTED:
      return QObject::tr("Unexpected");

    case zc::Error::UNEXPECTED_END_OF_EXPRESSION:
      return QObject::tr("Unexpected end of expression");

    case zc::Error::MISSING:
      return QObject::tr("Missing");

    case zc::Error::UNDEFINED_VARIABLE:
      return QObject::tr("Undefined variable");

    case zc::Error::UNDEFINED_FUNCTION:
      return QObject::tr("Undefined function");

    case zc::Error::CALLING_FUN_ARG_COUNT_MISMATCH:
      return QObject::tr("Calling function with wrong number of arguments");

    case zc::Error::NOT_IMPLEMENTED:
      return QObject::tr("Feature not implemented");

    case zc::Error::EMPTY_EXPRESSION:
      return QObject::tr("Empty expression");

    case zc::Error::RECURSION_DEPTH_OVERFLOW:
      return QObject::tr("Maximum recursion depth has been attained");

    case zc::Error::WRONG_OBJECT_TYPE:
      return QObject::tr("Objet has the wrong type");

    case zc::Error::OBJECT_INVALID_STATE:
      return QObject::tr("Objet in invalid state");

    default:
      return QObject::tr("Unsupported error code");
  }
}

}
