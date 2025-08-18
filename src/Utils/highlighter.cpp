#include "highlighter.h"
#include "information.h"

#include <zecalculator/zecalculator.h>

void Highlighter::setBackend(zg::Highlighted* b)
{
  qDebug() << "Highlighter: setting backend pointer" << b;
  qDebug() << "Highlighter: address" << this;
  this->backend = b;
  rehighlight();
}

void Highlighter::highlightBlock(const QString &text)
{
  qDebug() << "Highlighting string: " << text << "\n"
           << "Highlighter address: " << this;

  if (not backend)
  {
    qDebug() << "Math object still undefined in highlighter";
    return;
  }

  QTextCharFormat invalidFormat;
  invalidFormat.setForeground(information.getAppSettings().invalidSyntax);
  invalidFormat.setFontUnderline(true);
  invalidFormat.setUnderlineColor(information.getAppSettings().invalidSyntax);
  invalidFormat.setUnderlineStyle(QTextCharFormat::UnderlineStyle::WaveUnderline);

  zg::State new_state = backend->setExpression(text);
  if (auto opt_err_tok = new_state.getErrToken(); opt_err_tok)
  {
    setFormat(opt_err_tok->begin - offset, opt_err_tok->substr.size(), invalidFormat);

    qDebug() << "Highlighted token: '" + QString::fromStdString(opt_err_tok->substr) + "' at "
                  + QString::number(opt_err_tok->begin - offset);
  };
}

void Highlighter::setDocument(QQuickTextDocument* doc)
{
  textDocument = doc;
  QSyntaxHighlighter::setDocument(doc->textDocument());
}
