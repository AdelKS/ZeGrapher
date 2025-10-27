#pragma once

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

#include <QObject>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>

#include <zecalculator/error.h>
#include <zecalculator/parsing/data_structures/token.h>

namespace zg {

/// @brief QML-exposed "state" wrapper
struct State {
  Q_GADGET
  QML_ELEMENT

  Q_PROPERTY(QString errorMsg READ getErrorMsg)
  Q_PROPERTY(Status status READ getStatus)

public:
  explicit State() = default;

  enum Status {NEUTRAL, VALID, INVALID};
  Q_ENUM(Status)

  /// @brief update state using this expected
  template <class T>
  State& update(const tl::expected<T, zc::Error>& exp);

  State& update(const std::optional<zc::Error>& err);

  Q_INVOKABLE Status getStatus() const;
  Q_INVOKABLE QString getErrorMsg() const;

  std::optional<zc::parsing::tokens::Text> getErrToken() const { if (opt_zc_error) return opt_zc_error->token; else return {}; }

  bool operator == (const State&) const = default;

  Q_INVOKABLE void setValid();
  Q_INVOKABLE void setNeutral();

  /// @brief set an error message coming from ZG
  Q_INVOKABLE void setInvalid(QString zgErrorMsg);

  Q_INVOKABLE bool isValid() const;

protected:
  QString errorMsg;
  /// @brief error that comes from the ZC backend
  /// @note errors can come from ZG too, in which case this one will be empty
  std::optional<zc::Error> opt_zc_error;
  Status status = NEUTRAL;
};

template <class T>
State& State::update(const tl::expected<T, zc::Error>& exp)
{
  return update(exp.has_value() ? std::optional<zc::Error>{} : exp.error());
}

}
