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

    case zc::Error::UNKNOWN:
      return QObject::tr("Unkown error");

    case zc::Error::WRONG_FORMAT:
      return QObject::tr("Wrong format");

    case zc::Error::UNEXPECTED:
      return QObject::tr("Unexpected");

    case zc::Error::UNEXPECTED_END_OF_EXPRESSION:
      return QObject::tr("Unexpected end of expression");

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
  qDebug() << "Parsing expression: " << text;
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

  std::optional<zc::Error> opt_err;
  if (type == Type::VALUE)
  {
    const auto eval = information.getMathWorld().evaluate(text.toStdString());
    if (eval)
    {
      if (value != eval.value())
      {
        value = eval.value();
        emit valueChanged(value);
      }
    }
    else opt_err = std::move(eval.error());
  }
  else
  {
    Q_ASSERT(math_obj);
    switch (type)
    {
    case Type::OBJECT_AUTO:
      *math_obj = text.toStdString();
      break;
    case Type::OBJECT_FUNCTION:
      *math_obj = zc::As<zc::Function<zc_t>>{text.toStdString()};
      break;
    case Type::OBJECT_GLOBAL_CONSTANT:
      *math_obj = zc::As<zc::GlobalConstant>{text.toStdString()};
      break;
    case Type::OBJECT_SEQUENCE:
      *math_obj = zc::As<zc::Sequence<zc_t>>{text.toStdString()};
      break;
    default:
      qDebug() << "Case not handled";
      break;
    }

    if (not math_obj->has_value())
      opt_err = math_obj->error();
  }

  if (opt_err)
  {
    if (opt_err->type == zc::Error::EMPTY_EXPRESSION)
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
    else setErrorState(*opt_err);
  }
  else
  {
    if (state != State::VALID)
    {
      state = State::VALID;
      emit stateChanged(state);
    }

    if (not errorMsg.isEmpty())
    {
      errorMsg.clear();
      emit errorMsgChanged(errorMsg);
    }
  }
}

void ExprEditBackend::setDocument(QQuickTextDocument* doc)
{
  textDocument = doc;
  QSyntaxHighlighter::setDocument(doc->textDocument());
}

void ExprEditBackend::setType(Type t)
{
  [[maybe_unused]] QMetaEnum metaEnum = QMetaEnum::fromType<Type>();
  qDebug() << "Setting type: " << metaEnum.valueToKey(int(t));

  switch (t)
  {
  case Type::VALUE:
    if (math_obj)
    {
      information.getMathWorld().erase(*math_obj);
      math_obj = nullptr;
    }
    break;
  case Type::OBJECT_AUTO:
  case Type::OBJECT_FUNCTION:
  case Type::OBJECT_GLOBAL_CONSTANT:
  case Type::OBJECT_SEQUENCE:
    if (not math_obj)
      math_obj = &information.getMathWorld().new_object();
  }
  type = t;
  rehighlight();
}
