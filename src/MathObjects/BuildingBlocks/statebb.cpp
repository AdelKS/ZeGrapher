#include "statebb.h"

namespace zg {
namespace mathobj {
namespace shared {

StateBB::StateBB(QObject *parent)
  : QObject(parent)
{
}

bool StateBB::isValid() const
{
  if (state)
    return state->getStatus() == State::VALID;
  else return false;
}

}
}
}
