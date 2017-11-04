
#ifndef BNETWORKGLOBALS_H
#define BNETWORKGLOBALS_H

#include <QChar>

namespace BNetworkGlobals
{
    enum NetworkCommands
    {
        Hello = 0,
        Enter,
        Leave,
        MouseMove,
        MousePress,
        MouseRelease,
        KeyPress,
        KeyRelease
    };

    static constexpr QChar NewLine = QLatin1Char('\n');

    static constexpr quint16 BSynergyPort = 37193;

    static constexpr quint16 BSynergyNetworkVersion = 1;
}

#endif //BNETWORKGLOBALS_H
