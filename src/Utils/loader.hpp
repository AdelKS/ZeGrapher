#include <QSettings>

class Loader
{
public:
    enum Direction {TO_DISK, FROM_DISK};

    Loader() = default;
    Loader(Direction direction): direction(direction) {}
    Loader(Direction direction, QString prefix): direction(direction), prefix(prefix) {}

    inline void set_direction(const Direction dir) { direction = dir; }
    inline void set_prefix(QString p)
    {
        prefix = std::move(p);
        if (not prefix.endsWith('/'))
            prefix += "/";
    }

    void operator() (auto &internal, const QString& name, const auto& default_val) const
    {
        QSettings settings;
        using InternalType = std::remove_cvref_t<decltype(internal)>;
        if (direction == FROM_DISK)
            internal = settings.value(prefix + name, QVariant::fromValue(default_val)).template value<InternalType>();
        else settings.setValue(prefix + name, QVariant::fromValue(internal));
    };

protected:
    Direction direction = TO_DISK;
    QString prefix;
};
