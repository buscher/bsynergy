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


#ifndef BCLIENT_H
#define BCLIENT_H

#include <QObject>
#include <QDataStream>
#include <QAbstractSocket>

#include <base/bscreenconfigglobal.h>
#include "bnetworkglobals.h"

class QTcpSocket;

class BClient : public QObject
{
    Q_OBJECT
public:
    enum CommunicationSate
    {
        None = 0,
        Hello = 1,
        Connected = 2
    };

    explicit BClient(QTcpSocket* socket, QObject* parent = nullptr);
    explicit BClient(QObject* parent = nullptr);

    bool connectTo(const QString& ip, quint16 port = BNetworkGlobals::BSynergyPort);

    void setLocalScreenSize(int width, int height);

    void enter(int x, int y, BScreenConfigGlobal::ScreenLayout direction);
    void leave();

    QString hostname() const;

    int hostScreenWidth() const;
    int hostScreenHeight() const;

    bool connected();

    void close();

public slots:
    void mouseMove(int x, int y);
    void mousePress(int button);
    void mouseRelease(int button);

    void keyPress(int keyCode, int modifiers = 0);
    void keyRelease(int keyCode, int modifiers = 0);

signals:
    void clientDisconnected(BClient* client);
    void clientConnected(BClient* client);
    void clientSocketConnected();
    void connectionError(const QString& error);

    void entered(int x, int y, BScreenConfigGlobal::ScreenLayout direction);
    void left();

    void mouseMoved(int x, int y);
    void mousePressed(int button);
    void mouseReleased(int button);

    void keyPressed(int keyCode, int modifiers = 0);
    void keyReleased(int keyCode, int modifiers = 0);

protected slots:
    void readData();
    void socketConnected();
    void socketError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket* m_socket;
    QDataStream m_dataStream;
    QByteArray m_buffer;
    CommunicationSate m_connectionState;

    QString m_name;
    qint32 m_width;
    qint32 m_height;
};

#endif // BCLIENT_H
