#include <QMetaEnum>
#include <QString>
#include <QStringView>
#include <optional>

template <typename T>
QString enumValToLowercaseStr(T v)
{
  QMetaEnum metaEnum = QMetaEnum::fromType<T>();
  return QString(metaEnum.valueToKey(v)).toLower();
}

template <typename T>
std::optional<T> strToEnumVal(QStringView str)
{
  QMetaEnum me = QMetaEnum::fromType<T>();
  for (int i = 0; i < me.keyCount(); ++i)
    if (str.compare(me.key(i), Qt::CaseInsensitive) == 0)
      return me.value(i);
  return {};
}
