#pragma once

#include <QSyntaxHighlighter>
#include <QQuickTextDocument>
#include "Utils/opterror.h"

/// @brief backend that highlights parsing errors in math expressions in LineEdit
class Highlighter: public QSyntaxHighlighter
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QQuickTextDocument* textDocument WRITE setDocument MEMBER textDocument)
  Q_PROPERTY(zg::OptError optError WRITE setOptError MEMBER optError)
  Q_PROPERTY(int offset MEMBER offset)

public:

  Highlighter(QObject* parent = nullptr): QSyntaxHighlighter(parent) {}

  void highlightBlock(const QString &text) override final;

  Q_INVOKABLE void setOptError(zg::OptError optError);
  Q_INVOKABLE void setDocument(QQuickTextDocument* doc);

signals:
  void valueChanged(double);
  void errorMsgChanged(QString);

protected:
  QQuickTextDocument* textDocument = nullptr;
  zg::OptError optError;
  int offset = 0;

};
