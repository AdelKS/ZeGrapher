#pragma once

#include <QSyntaxHighlighter>
#include <QQuickTextDocument>

#include "Utils/state.h"

/// @brief backend that highlights parsing errors in math expressions in LineEdit
class Highlighter: public QSyntaxHighlighter
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QQuickTextDocument* textDocument WRITE setDocument MEMBER textDocument)
  Q_PROPERTY(zg::State state WRITE setState MEMBER state)
  Q_PROPERTY(int offset MEMBER offset)

public:

  Highlighter(QObject* parent = nullptr): QSyntaxHighlighter(parent) {}

  void highlightBlock(const QString &text) override final;

  Q_INVOKABLE void setDocument(QQuickTextDocument* doc);
  Q_INVOKABLE void setState(zg::State state);

signals:
  void valueChanged(double);
  void errorMsgChanged(QString);

protected:
  QQuickTextDocument* textDocument = nullptr;
  zg::State state;
  int offset = 0;

};
