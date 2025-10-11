#pragma once

#include "MathObjects/mathworld.h"
#include "information.h"

namespace zg {


inline MathWorld mathWorld;

/// @brief register the 'information' global variable with QML
/// @note  see https://doc.qt.io/qt-6/qml-singleton.html#exposing-an-existing-object-as-a-singleton

struct MathWorldForeign
{
  Q_GADGET
  QML_FOREIGN(MathWorld)
  QML_SINGLETON
  QML_NAMED_ELEMENT(MathWorld)

public:

  static MathWorld *create(QQmlEngine *, QJSEngine *engine)
  {
    // The engine has to have the same thread affinity as the singleton.
    Q_ASSERT(engine->thread() == mathWorld.thread());

    // There can only be one engine accessing the singleton.
    if (s_engine)
      Q_ASSERT(engine == s_engine);
    else
      s_engine = engine;

    // Explicitly specify C++ ownership so that the engine doesn't delete
    // the instance.
    QJSEngine::setObjectOwnership(&mathWorld,
                                  QJSEngine::CppOwnership);
    return &mathWorld;
  }

private:
  inline static QJSEngine *s_engine = nullptr;
};

}

inline Information information;

/// @brief register the 'information' global variable with QML
/// @note  see https://doc.qt.io/qt-6/qml-singleton.html#exposing-an-existing-object-as-a-singleton

struct InformationForeign
{
  Q_GADGET
  QML_FOREIGN(Information)
  QML_SINGLETON
  QML_NAMED_ELEMENT(Information)

public:

  static Information *create(QQmlEngine *, QJSEngine *engine)
  {
    // The engine has to have the same thread affinity as the singleton.
    Q_ASSERT(engine->thread() == information.thread());

    // There can only be one engine accessing the singleton.
    if (s_engine)
      Q_ASSERT(engine == s_engine);
    else
      s_engine = engine;

    // Explicitly specify C++ ownership so that the engine doesn't delete
    // the instance.
    QJSEngine::setObjectOwnership(&information,
                                  QJSEngine::CppOwnership);
    return &information;
  }

private:
  inline static QJSEngine *s_engine = nullptr;
};
