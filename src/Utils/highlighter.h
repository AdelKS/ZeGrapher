#pragma once

#include <QSyntaxHighlighter>
#include <QQuickTextDocument>

#include "MathObjects/BuildingBlocks/highlighted.h"

/// @brief backend that highlights parsing errors in math expressions in LineEdit
class Highlighter: public QSyntaxHighlighter
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QQuickTextDocument* textDocument WRITE setDocument MEMBER textDocument)
  Q_PROPERTY(zg::Highlighted* backend WRITE setBackend MEMBER backend)
  Q_PROPERTY(int offset MEMBER offset)

public:

  Highlighter(QObject* parent = nullptr): QSyntaxHighlighter(parent) {}

  void highlightBlock(const QString &text) override final;

  Q_INVOKABLE void setBackend(zg::Highlighted*);
  Q_INVOKABLE void setDocument(QQuickTextDocument* doc);

signals:
  void valueChanged(double);
  void errorMsgChanged(QString);

protected:
  QQuickTextDocument* textDocument = nullptr;
  zg::Highlighted* backend = nullptr;
  int offset = 0;

};
