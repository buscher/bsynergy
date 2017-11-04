
#ifndef BSCREENCONFIGGLOBAL_H
#define BSCREENCONFIGGLOBAL_H

#include <QMap>
#include <QString>
#include <QList>

namespace BScreenConfigGlobal
{
    enum ScreenLayout
    {
        RightOf = 1,
        LeftOf,
        BelowOf,
        AboveOf
    };

    struct ScreenLayoutStruct
    {
        QString name;
        ScreenLayout connection;
    };

    typedef QMap<QString, ScreenLayoutStruct> ScreenLayoutMap;
    typedef QList<ScreenLayoutStruct> ScreenLayoutList;
}

#endif //BSCREENCONFIGGLOBAL_H
