#include "expreditbackend.h"
#include "information.h"

#include <zecalculator/zecalculator.h>

QString zcErrorToStr(const zc::Error& err)
{
  switch (err.type)
  {
    case zc::Error::CPP_INCORRECT_ARGNUM:
      return QObject::tr("Programmatically calling C++ math function with wrong number of arguments");

    case zc::Error::NOT_MATH_OBJECT_DEFINITION:
      return QObject::tr("Not a correct math object definition");

    case zc::Error::OBJECT_NOT_IN_WORLD:
      return QObject::tr("Object not registered in current math world");

    case zc::Error::NAME_ALREADY_TAKEN:
      return QObject::tr("Name already taken");

    case zc::Error::EMPTY:
      return QObject::tr("Empty");

    case zc::Error::UNKNOWN:
      return QObject::tr("Unkown error");

    case zc::Error::WRONG_FORMAT:
      return QObject::tr("Wrong format");

    case zc::Error::UNEXPECTED:
      return QObject::tr("Unexpected");

    case zc::Error::MISSING:
      return QObject::tr("Missing");

    case zc::Error::UNDEFINED_VARIABLE:
      return QObject::tr("Undefined variable");

    case zc::Error::UNDEFINED_FUNCTION:
      return QObject::tr("Undefined function");

    case zc::Error::CALLING_FUN_ARG_COUNT_MISMATCH:
      return QObject::tr("Calling function with wrong number of arguments");

    case zc::Error::NOT_IMPLEMENTED:
      return QObject::tr("Feature not implemented");

    case zc::Error::EMPTY_EXPRESSION:
      return QObject::tr("Empty expression");

    case zc::Error::RECURSION_DEPTH_OVERFLOW:
      return QObject::tr("Maximum recursion depth has been attained");

    case zc::Error::WRONG_OBJECT_TYPE:
      return QObject::tr("Objet has the wrong type");

    case zc::Error::OBJECT_INVALID_STATE:
      return QObject::tr("Objet in invalid state");

    default:
      return QObject::tr("Unsupported error code");
  }
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
    setFormat(err.token.begin, err.token.substr.size(), invalidFormat);

    if (errorMsg != zcErrorToStr(err))
    {
      errorMsg = zcErrorToStr(err);
      emit errorMsgChanged(errorMsg);
    }

    qDebug() << errorMsg + ", token: '" + QString::fromStdString(err.token.substr) + "' at "
                  + QString::number(err.token.begin);

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
  else if (eval.error().type == zc::Error::EMPTY)
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
