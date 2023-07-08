#pragma once

#include <QSyntaxHighlighter>
#include <QQuickTextDocument>

/// @brief class that handles computing the value of expressions
///        and syntax highlighting
class ExprEditBackend: public QSyntaxHighlighter
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(State state MEMBER state NOTIFY stateChanged)
  Q_PROPERTY(QString errorMsg MEMBER errorMsg NOTIFY errorMsgChanged)
  Q_PROPERTY(double value MEMBER value NOTIFY valueChanged)
  Q_PROPERTY(QQuickTextDocument* textDocument MEMBER textDocument WRITE setDocument)

public:

  enum struct State {NEUTRAL, VALID, INVALID};
  Q_ENUM(State)

  State state;
  QString errorMsg;
  double value;
  QQuickTextDocument* textDocument = nullptr;

  ExprEditBackend(QObject* parent = nullptr): QSyntaxHighlighter(parent) {}

  void highlightBlock(const QString &text) override final;

  Q_INVOKABLE void setDocument(QQuickTextDocument* doc);

signals:
  void stateChanged(State);
  void valueChanged(double);
  void errorMsgChanged(QString);

};
