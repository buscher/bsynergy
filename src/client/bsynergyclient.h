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


#ifndef BSYNERGYCLIENT_H
#define BSYNERGYCLIENT_H

#include <QObject>

#include <net/bnetworkglobals.h>
#include <base/bscreenconfigglobal.h>

class BClient;
class IScreen;

class BSynergyClient : public QObject
{
    Q_OBJECT
public:
    explicit BSynergyClient(QObject* parent = nullptr);

    bool connectTo(const QString& ip, quint16 port = BNetworkGlobals::BSynergyPort);
    bool openScreen();

    void close();

signals:
    void connectionError(const QString& error);

protected slots:
    void clientSocketConnected();
    void clientConnected(BClient* client);

    void entered(int x, int y, BScreenConfigGlobal::ScreenLayout direction);

    void clientError(const QString& text);

private:
    IScreen *m_screen;
    BClient *m_client;
};

#endif // BSYNERGYCLIENT_H
