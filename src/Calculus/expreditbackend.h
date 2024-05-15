#pragma once

#include "information.h"
#include <QSyntaxHighlighter>
#include <QQuickTextDocument>
#include <zecalculator/zecalculator.h>

/// @brief class that handles computing the value of expressions
///        and syntax highlighting
class ExprEditBackend: public QSyntaxHighlighter
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(Type type MEMBER type WRITE setType NOTIFY typeChanged)
  Q_PROPERTY(State state MEMBER state NOTIFY stateChanged)
  Q_PROPERTY(QString errorMsg MEMBER errorMsg NOTIFY errorMsgChanged)
  Q_PROPERTY(double value MEMBER value NOTIFY valueChanged)
  Q_PROPERTY(QQuickTextDocument* textDocument MEMBER textDocument WRITE setDocument)

public:

  enum struct State {NEUTRAL, VALID, INVALID};
  Q_ENUM(State)

  enum struct Type {VALUE, OBJECT_AUTO, OBJECT_FUNCTION, OBJECT_SEQUENCE, OBJECT_GLOBAL_CONSTANT};
  Q_ENUM(Type)

  Type type = Type::VALUE;
  State state = State::NEUTRAL;
  QString errorMsg = {};
  double value = 0;
  QQuickTextDocument* textDocument = nullptr;
  zc::DynMathObject<zc_t>* math_obj = nullptr;

  ExprEditBackend(QObject* parent = nullptr): QSyntaxHighlighter(parent) {}

  void highlightBlock(const QString &text) override final;

  Q_INVOKABLE void setDocument(QQuickTextDocument* doc);
  Q_INVOKABLE void setType(Type t);

signals:
  void typeChanged(Type);
  void stateChanged(State);
  void valueChanged(double);
  void errorMsgChanged(QString);

};
