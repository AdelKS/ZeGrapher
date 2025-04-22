#include "state.h"
#include "Utils/zc-utils.h"


namespace zg {

void State::update(const std::optional<zc::Error>& err)
{
  opt_zc_error = err;

  if (err)
  {
    if (err->type == zc::Error::EMPTY_EXPRESSION)
    {
      status = State::NEUTRAL;
      errorMsg.clear();
    }
    else
    {
      status = State::INVALID;
      errorMsg = zcErrorToStr(*err);
    }
  }
  else
  {
    status = State::VALID;
    errorMsg.clear();
  }
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

State::Status State::getStatus() const { return status; }

}
