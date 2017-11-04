/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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


#ifndef BLISTENSERVER_H
#define BLISTENSERVER_H

#include <QObject>
#include <QVector>

#include "bnetworkglobals.h"

class BClient;
class QTcpServer;
class QTcpSocket;

class BListenServer : public QObject
{
    Q_OBJECT
public:
    explicit BListenServer(QObject* parent = nullptr);
    virtual ~BListenServer();

    bool listen(quint16 port = BNetworkGlobals::BSynergyPort);

    bool clientConnected(const QString& name);
    BClient* clientByName(const QString& name);

signals:
    void newClient(BClient*);

protected slots:
    void newConnection();
    void clientDisconnected(BClient* client);

private:
    QTcpServer* m_server;
    QVector<BClient*> m_clientList;
};

#endif // BLISTENSERVER_H
