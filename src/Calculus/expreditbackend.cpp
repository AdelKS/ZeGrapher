#include "expreditbackend.h"
#include "Utils/zc.h"
#include "information.h"

#include <zecalculator/zecalculator.h>

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

    if (errorMsg != zg::zcErrorToStr(err))
    {
      errorMsg = zg::zcErrorToStr(err);
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
  information.emitUpdateSignal();
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
