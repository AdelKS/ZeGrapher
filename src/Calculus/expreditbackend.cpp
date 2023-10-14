#include "expreditbackend.h"
#include "information.h"

#include <zecalculator/zecalculator.h>

const QString& zcErrorToStr(const zc::Error& err)
{
  const static std::map<zc::Error::Type, QString> errMap =
  {
    {zc::Error::UNKNOWN, QObject::tr("Unkown error")},
    {zc::Error::WRONG_FORMAT, QObject::tr("Wrong format")},
    {zc::Error::UNEXPECTED, QObject::tr("Unexpected")},
    {zc::Error::MISSING, QObject::tr("Missing")},
    {zc::Error::UNDEFINED_VARIABLE, QObject::tr("Undefined variable")},
    {zc::Error::UNDEFINED_FUNCTION, QObject::tr("Undefined function")},
    {zc::Error::CALLING_FUN_ARG_COUNT_MISMATCH, QObject::tr("Calling function with wrong number of arguments")},
    {zc::Error::NOT_IMPLEMENTED, QObject::tr("Feature not implemented")},
    {zc::Error::EMPTY_EXPRESSION, QObject::tr("Empty expression")},
    {zc::Error::INVALID_FUNCTION, QObject::tr("Invalid function")},
    {zc::Error::CALLING_INVALID_FUNCTION, QObject::tr("Calling invalid function")},
    {zc::Error::RECURSION_DEPTH_OVERFLOW, QObject::tr("Maximum recursion depth has been attained")},
    {zc::Error::WRONG_OBJECT_TYPE, QObject::tr("Objet has the wrong type")},
  };

  return errMap.at(err.error_type);
}

void ExprEditBackend::highlightBlock(const QString &text)
{
  QTextCharFormat invalidFormat;
  invalidFormat.setForeground(information.getAppSettings().invalidSyntax);
  invalidFormat.setFontUnderline(true);
  invalidFormat.setUnderlineColor(information.getAppSettings().invalidSyntax);
  invalidFormat.setUnderlineStyle(QTextCharFormat::UnderlineStyle::WaveUnderline);

  auto setErrorState = [&](const zc::Error& err)
  {
    setFormat(err.token.substr_info.begin, err.token.substr_info.size, invalidFormat);

    if (errorMsg != zcErrorToStr(err))
    {
      errorMsg = zcErrorToStr(err);
      emit errorMsgChanged(errorMsg);
    }

    qDebug() << errorMsg << ", " << err.token.substr_info.begin << ", " << err.token.substr_info.size;

    if (state != State::INVALID)
    {
      state = State::INVALID;
      emit stateChanged(state);
    }
  };

  qDebug() << "Evaluating: " << text;
  const auto eval = information.getMathWorld().evaluate(text.toStdString());

  if (eval)
  {
    if (state != State::VALID)
    {
      state = State::VALID;
      emit stateChanged(state);
    }

    if (value != eval.value())
    {
      value = eval.value();
      emit valueChanged(value);
    }

    if (not errorMsg.isEmpty())
    {
      errorMsg.clear();
      emit errorMsgChanged(errorMsg);
    }
  }
  else if (eval.error().error_type == zc::Error::EMPTY)
  {
    if (not errorMsg.isEmpty())
    {
      errorMsg.clear();
      emit errorMsgChanged(errorMsg);
    }

    if (state != State::NEUTRAL)
    {
      state = State::NEUTRAL;
      emit stateChanged(state);
    }
  }
  else setErrorState(eval.error());
}

void ExprEditBackend::setDocument(QQuickTextDocument* doc)
{
  textDocument = doc;
  QSyntaxHighlighter::setDocument(doc->textDocument());
}
