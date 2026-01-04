#include "highlighter.h"
#include "globalvars.h"

#include <zecalculator/zecalculator.h>

Highlighter::Highlighter(QObject* parent): QSyntaxHighlighter(parent) {
  connect(information.appSettings, &ZeAppSettings::invalidSyntaxChanged, this, &Highlighter::rehighlight);
  connect(qGuiApp->styleHints(), &QStyleHints::colorSchemeChanged, this, &Highlighter::rehighlight);
}

void Highlighter::setState(zg::State s)
{
  if (state != s)
  {
    state = s;
    rehighlight();
  }
}

void Highlighter::highlightBlock(const QString &text)
{
  qDebug() << "Highlighting string: " << text << "\n"
           << "Highlighter address: " << this;

  QTextCharFormat invalidFormat;
  invalidFormat.setForeground(information.appSettings->getInvalidSyntax());
  invalidFormat.setFontUnderline(true);
  invalidFormat.setUnderlineColor(information.appSettings->getInvalidSyntax());
  invalidFormat.setUnderlineStyle(QTextCharFormat::UnderlineStyle::WaveUnderline);

  if (auto opt_err_tok = state.getErrToken(); opt_err_tok)
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
