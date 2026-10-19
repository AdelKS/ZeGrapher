/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
