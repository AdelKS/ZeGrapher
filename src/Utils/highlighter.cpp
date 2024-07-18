#include "highlighter.h"
#include "information.h"

#include <zecalculator/zecalculator.h>

void Highlighter::setOptError(zg::OptError optError)
{
  this->optError = std::move(optError);
  rehighlight();
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

  if (optError.getType() == zg::OptError::INVALID)
  {
    const auto& err = *optError.value;

    setFormat(err.token.begin - offset, err.token.substr.size(), invalidFormat);

    qDebug() << "Highlighted token: '" + QString::fromStdString(err.token.substr) + "' at "
                  + QString::number(err.token.begin - offset);
  };
}

void Highlighter::setDocument(QQuickTextDocument* doc)
{
  textDocument = doc;
  QSyntaxHighlighter::setDocument(doc->textDocument());
}
