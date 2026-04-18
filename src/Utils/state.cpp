#include "state.h"
#include "Utils/zc-utils.h"


namespace zg {

State State::from(const std::optional<zc::Error>& err)
{
  State state;
  state.opt_zc_error = err;

  if (err)
  {
    if (err->type == zc::Error::EMPTY_EXPRESSION)
    {
      state.status = State::NEUTRAL;
      state.errorMsg.clear();
    }
    else
    {
      state.status = State::INVALID;
      state.errorMsg = zcErrorToStr(*err);
    }
  }
  else
  {
    state.status = State::VALID;
    state.errorMsg.clear();
  }

  return state;
}


void State::setValid()
{
  status = State::VALID;
  opt_zc_error.reset();
  errorMsg.clear();
}

void State::setNeutral()
{
  status = State::NEUTRAL;
  opt_zc_error.reset();
  errorMsg.clear();
}

QString State::getErrorMsg() const { return errorMsg; };

void State::setInvalid(QString zgErrorMsg)
{
  opt_zc_error.reset();
  errorMsg = zgErrorMsg;
  status = INVALID;
}

bool State::isValid() const
{
  return status == VALID;
}

State::Status State::getStatus() const { return status; }

}
