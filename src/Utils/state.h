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

#include <zecalculator/parsing/data_structures/token.h>
#include <zecalculator/error.h>

#include "Utils/zc.h"

namespace zg {

/// @brief QML-exposed "state" wrapper
struct State: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString errorMsg READ getErrorMsg)
  Q_PROPERTY(Status status READ getStatus)

public:
  explicit State(QObject *parent = nullptr);

  enum Status {NEUTRAL, VALID, INVALID};
  Q_ENUM(Status)

  /// @brief update state using this expected
  template <class T>
  void update(const tl::expected<T, zc::Error>& exp);

  void setInvalid(QString errorMsg, std::optional<zc::parsing::tokens::Text> errorToken);

  Q_INVOKABLE Status getStatus() const;
  Q_INVOKABLE QString getErrorMsg() const;
  std::optional<zc::parsing::tokens::Text> getErrToken() const;

public slots:
  void setValid();
  void setNeutral();
  void setInvalid(QString errorMsg);

signals:
  void updated();

protected:

  QString errorMsg;
  std::optional<zc::parsing::tokens::Text> errorToken;
  Status status = NEUTRAL;
};

template <class T>
void State::update(const tl::expected<T, zc::Error>& exp)
{
  if (not exp)
  {
    if (exp.error().type == zc::Error::EMPTY_EXPRESSION)
      setNeutral();
    else
      setInvalid(zg::zcErrorToStr(exp.error()), exp.error().token);
  }
  else setValid();
}

}
