#include "MathObjects/parametric.h"
#include "information.h"

namespace zg {
namespace mathobj {

void setName(QString inputName, tl::expected<zc::parsing::LHS, zc::Error>& nameLHS, QString& name, State* state)
{
  auto oldNameLHS = nameLHS;

  std::string std_input_name = inputName.toStdString();
  nameLHS = zc::parsing::parse_lhs(std_input_name, std_input_name);

  if (bool(nameLHS) and not nameLHS->input_vars.empty())
    nameLHS = tl::unexpected(zc::Error::unexpected(nameLHS->input_vars.front(), std_input_name));

  if (bool(nameLHS))
    name = QString::fromStdString(nameLHS->name.substr);
  else name = "";

  if (bool(nameLHS) and not information.getMathWorld().get(nameLHS->name.substr))
    nameLHS = tl::unexpected(zc::Error::undefined_function(nameLHS->name, std_input_name));

  if (state and oldNameLHS != nameLHS)
    state->update(nameLHS);
}

Parametric::Parametric(QObject *parent)
  : QObject(parent)
{}

void Parametric::refresh()
{
  setFirstName(inputFirstName);
  setSecondName(inputSecondName);
}

void Parametric::setFirstName(QString input_name)
{
  inputFirstName = input_name;
  setName(input_name, firstNameLHS, firstName, firstState);
}

void Parametric::setSecondName(QString input_name)
{
  inputSecondName = input_name;
  setName(input_name, secondNameLHS, secondName, secondState);
}

}
}
