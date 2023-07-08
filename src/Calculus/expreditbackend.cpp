#include "expreditbackend.h"
#include "information.h"

#include <zecalculator/zecalculator.h>

const QString& zcErrorToStr(const zc::EvaluationError& err)
{
  const static std::map<zc::EvaluationError::Type, QString> errMap =
  {
    {zc::EvaluationError::Type::UNDEFINED, QObject::tr("Undefined evaluation error")},
    {zc::EvaluationError::Type::UNDEFINED_VARIABLE, QObject::tr("Undefined variable")},
    {zc::EvaluationError::Type::UNDEFINED_FUNCTION, QObject::tr("Undefined function")},
    {zc::EvaluationError::Type::CALLING_FUN_ARG_COUNT_MISMATCH, QObject::tr("Calling function with wrong number of arguments")},
    {zc::EvaluationError::Type::CALLED_FUN_ARG_COUNT_MISMATCH, QObject::tr("This function has been called with wrong number of arguments")},
    {zc::EvaluationError::Type::NOT_IMPLEMENTED, QObject::tr("Feature not implemented")},
    {zc::EvaluationError::Type::EMPTY_EXPRESSION, QObject::tr("Empty expression")},
    {zc::EvaluationError::Type::INVALID_FUNCTION, QObject::tr("Invalid function")},
    {zc::EvaluationError::Type::CALLING_INVALID_FUNCTION, QObject::tr("Calling invalid function")},
    {zc::EvaluationError::Type::RECURSION_DEPTH_OVERFLOW, QObject::tr("Maximum recursion depth has been attained")},
  };

  return errMap.at(err.error_type);
}

QString zcErrorToStr(const zc::ParsingError& err)
{
  auto tokenTypeToName = [](const zc::Token& token)
  {
    return std::visit(overloaded{[&](const zc::tokens::Unkown&) { return  QObject::tr("unkown"); },
                                 [&](const zc::tokens::Number&) { return QObject::tr("number"); },
                                 [&](const zc::tokens::Variable&) { return QObject::tr("variable"); },
                                 [&](const zc::tokens::Function&) { return QObject::tr("function"); },
                                 [&](const zc::tokens::Operator&) { return QObject::tr("operator"); },
                                 [&](const zc::tokens::OpeningParenthesis&) { return QString("\"(\""); },
                                 [&](const zc::tokens::ClosingParenthesis&) { return QString("\")\""); },
                                 [&](const zc::tokens::FunctionCallStart&) { return QObject::tr("function opening bracket"); },
                                 [&](const zc::tokens::FunctionCallEnd&) { return QObject::tr("function closing bracket"); },
                                 [&](const zc::tokens::FunctionArgumentSeparator&) { return QObject::tr("function argument separator"); },
                                 [&](const zc::tokens::EndOfExpression&) { return QObject::tr("end of expression"); },
                                 },
                      token);
  };

  const static std::map<zc::ParsingError::Type, QString> errMap =
  {
    {zc::ParsingError::Type::UNDEFINED, QObject::tr("Undefined parsing error on")},
    {zc::ParsingError::Type::WRONG_FORMAT, QObject::tr("Wrong format on")},
    {zc::ParsingError::Type::UNEXPECTED, QObject::tr("Unexpected")},
    {zc::ParsingError::Type::MISSING, QObject::tr("Missing")},
  };

  return errMap.at(err.error_type) + " " + tokenTypeToName(err.token) + ".";
}

void ExprEditBackend::highlightBlock(const QString &text)
{
  QTextCharFormat invalidFormat;
  invalidFormat.setForeground(information.getAppSettings().invalidSyntax);
  invalidFormat.setFontUnderline(true);
  invalidFormat.setUnderlineColor(information.getAppSettings().invalidSyntax);
  invalidFormat.setUnderlineStyle(QTextCharFormat::UnderlineStyle::WaveUnderline);

  auto setErrorState = [&](const auto& err, const zc::SubstrInfo substrInfo)
  {
    setFormat(substrInfo.begin, substrInfo.size, invalidFormat);

    if (errorMsg != zcErrorToStr(err))
    {
      errorMsg = zcErrorToStr(err);
      emit errorMsgChanged(errorMsg);
    }

    qDebug() << errorMsg << ", " << substrInfo.begin << ", " << substrInfo.size;

    if (state != State::INVALID)
    {
      state = State::INVALID;
      emit stateChanged(state);
    }
  };

  qDebug() << "Evaluating: " << text;
  const zc::Expression exprEvaluator(text.toStdString());
  const auto expr_state = exprEvaluator.parsing_status();

  if (std::holds_alternative<zc::Function::Empty>(expr_state))
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
    return;
  }
  else if (std::holds_alternative<zc::ParsingError>(expr_state))
  {
    const zc::ParsingError& parsingErr = std::get<zc::ParsingError>(expr_state);
    setErrorState(parsingErr, zc::substr_info(parsingErr.token));
    return;
  }

  auto eval = exprEvaluator.evaluate(information.getMathWorld());
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
  else
  {
    const zc::EvaluationError& evalErr = eval.error();
    setErrorState(evalErr, zc::substr_info(evalErr.node));
  };

}

void ExprEditBackend::setDocument(QQuickTextDocument* doc)
{
  textDocument = doc;
  QSyntaxHighlighter::setDocument(doc->textDocument());
}
