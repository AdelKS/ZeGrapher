#include "state.h"

namespace zg {

State::State(QObject *parent): QObject(parent)
{}

void State::update(const std::optional<zc::Error>& err)
{
  if (err)
  {
    if (err->type == zc::Error::EMPTY_EXPRESSION)
      setNeutral();
    else
      setInvalid(zg::zcErrorToStr(*err), err->token);
  }
  else setValid();

}

void State::setValid()
{
  bool changed = status != State::INVALID or errorToken.has_value() or not errorMsg.isEmpty();

  status = State::VALID;
  errorToken.reset();
  errorMsg.clear();

  if (changed)
    emit updated();
}

void State::setNeutral()
{
  bool changed = status != State::NEUTRAL or errorToken.has_value() or not errorMsg.isEmpty();

  status = State::NEUTRAL;
  errorToken.reset();
  errorMsg.clear();

  if (changed)
    emit updated();
}

void State::setInvalid(QString errorMsg)
{
  setInvalid(std::move(errorMsg), {});
}

void State::setInvalid(QString errorMsg, std::optional<zc::parsing::tokens::Text> errorToken)
{
  bool changed = status != State::INVALID or this->errorToken != errorToken
                 or this->errorMsg != errorMsg;

  status = State::INVALID;
  this->errorToken = std::move(errorToken);
  this->errorMsg = std::move(errorMsg);

  if (changed)
    emit updated();
}

State::Status State::getStatus() const { return status; }

QString State::getErrorMsg() const { return errorMsg; }

std::optional<zc::parsing::tokens::Text> State::getErrToken() const { return errorToken; }

}
