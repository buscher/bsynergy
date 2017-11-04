/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2010  Bernd Buschinski <b.buschinski@web.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef BSYNERGYSERVER_H
#define BSYNERGYSERVER_H

#include <QObject>

#include "base/bscreenconfigglobal.h"

class IScreen;
class BListenServer;
class BClient;

class BSynergyServer : public QObject
{
    Q_OBJECT
public:
    explicit BSynergyServer(const BScreenConfigGlobal::ScreenLayoutMap& rules, QObject* parent = nullptr);

    void close();

protected slots:
    void mouseMoved(int, int);

    void newClient(BClient* client);
    void clientConnected(BClient*);

private:
    inline QString findRule(const QString& host, BScreenConfigGlobal::ScreenLayout direction);

    inline void connectServerClientSignals(BClient* client);
    inline void disconnectServerClientSignals(BClient* client);

    IScreen* m_screen;
    BListenServer* m_listenServer;

    BScreenConfigGlobal::ScreenLayoutMap m_rules;

    int m_x;
    int m_y;
    int m_width;
    int m_height;

    QString m_hostname;
    QString m_currentScreen;

    BClient* m_currentClient;
};

#endif // BSYNERGYSERVER_H
