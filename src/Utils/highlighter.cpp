#include "highlighter.h"
#include "information.h"

#include <zecalculator/zecalculator.h>

void Highlighter::setState(zg::State* state)
{
  this->state = state;
  connect(state, &zg::State::updated, this, &Highlighter::rehighlight);
}

void Highlighter::highlightBlock(const QString &text)
{
  Q_UNUSED(text);
  qDebug() << "Highlighting called";
  QTextCharFormat invalidFormat;
  invalidFormat.setForeground(information.getAppSettings().invalidSyntax);
  invalidFormat.setFontUnderline(true);
  invalidFormat.setUnderlineColor(information.getAppSettings().invalidSyntax);
  invalidFormat.setUnderlineStyle(QTextCharFormat::UnderlineStyle::WaveUnderline);

  if (state and state->getErrToken())
  {
    setFormat(state->getErrToken()->begin - offset, state->getErrToken()->substr.size(), invalidFormat);

    qDebug() << "Highlighted token: '" + QString::fromStdString(state->getErrToken()->substr) + "' at "
                  + QString::number(state->getErrToken()->begin - offset);
  };
}

void Highlighter::setDocument(QQuickTextDocument* doc)
{
  textDocument = doc;
  QSyntaxHighlighter::setDocument(doc->textDocument());
}
