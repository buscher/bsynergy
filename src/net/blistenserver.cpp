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


#include "blistenserver.h"

#include <QTcpServer>
#include <QTcpSocket>

#include "bclient.h"

BListenServer::BListenServer(QObject* parent)
    : QObject(parent),
      m_server(nullptr)
{

}

BListenServer::~BListenServer()
{
    if (m_server)
    {
        disconnect(m_server, nullptr, nullptr, nullptr);
        delete m_server;
    }
}


bool BListenServer::listen(quint16 port)
{
    if (m_server)
    {
        return true;
    }

    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &BListenServer::newConnection);
    return m_server->listen(QHostAddress::Any, port);
}

void BListenServer::newConnection()
{
    qDebug() << "new conenction";
    Q_ASSERT(m_server->hasPendingConnections());

    QTcpSocket* socket = m_server->nextPendingConnection();
    BClient* client = new BClient(socket, this);

    connect(client, &BClient::clientDisconnected, this, &BListenServer::clientDisconnected);

    m_clientList.append(client);
    emit newClient(client);
}

bool BListenServer::clientConnected(const QString& name)
{
    foreach(BClient* client, m_clientList)
    {
        if (client->connected() && client->hostname() == name)
            return true;
    }
    return false;
}

BClient* BListenServer::clientByName(const QString& name)
{
    foreach(BClient* client, m_clientList)
    {
        if (client->connected() && client->hostname() == name)
            return client;
    }
    return nullptr;
}

void BListenServer::clientDisconnected(BClient* client)
{
    m_clientList.removeOne(client);
    client->deleteLater();
}

#include "moc_blistenserver.cpp"
