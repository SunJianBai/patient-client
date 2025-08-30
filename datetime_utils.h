#ifndef DATETIME_UTILS_H
#define DATETIME_UTILS_H

#include <QString>
#include <QDateTime>

inline QString getCurrentDateTimeString() {
    return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
}

#endif // DATETIME_UTILS_H
