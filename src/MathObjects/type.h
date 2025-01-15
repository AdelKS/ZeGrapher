#pragma once

#include <QQmlEngine>

namespace zg {
namespace mathobj {

  Q_NAMESPACE

  enum Type {EQUATION, CONSTANT, EXPR, PARAMETRIC};
  Q_ENUM_NS(Type)
  QML_NAMED_ELEMENT(ObjectType)

}
}
