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


#include "bclient.h"

#include <QTcpSocket>
// #include <qhostaddress.h>
#include <QHostInfo>

#include "bnetworkglobals.h"

BClient::BClient(QTcpSocket* socket, QObject* parent)
    : QObject(parent),
      m_socket(socket),
      m_connectionState(None),
      m_width(0),
      m_height(0)
{
    m_dataStream.setDevice(m_socket);
    connect(m_socket, &QTcpSocket::readyRead, this, &BClient::readData);
    if (m_socket->bytesAvailable() > 0)
        readData();
}

BClient::BClient(QObject* parent)
    : QObject(parent),
      m_socket(nullptr),
      m_connectionState(None),
      m_width(0),
      m_height(0)
{
}

bool BClient::connectTo(const QString& ip, quint16 port)
{
    qDebug() << "connect to:" << ip << port;
    m_socket = new QTcpSocket(this);
    m_socket->connectToHost(ip, port);
    connect(m_socket, &QTcpSocket::connected, this, &BClient::socketConnected);
    return true;
}

void BClient::setLocalScreenSize(int width, int height)
{
    qDebug() << "setLocalScreenSize" << width << height;
    m_dataStream << quint16(BNetworkGlobals::Hello)
                 << QHostInfo::localHostName()
                 << qint32(width)
                 << qint32(height)
                 << qint32(BNetworkGlobals::BSynergyNetworkVersion)
                 << BNetworkGlobals::NewLine;
    m_socket->flush();
}

void BClient::enter(int x, int y, BScreenConfigGlobal::ScreenLayout direction)
{
    m_dataStream << quint16(BNetworkGlobals::Enter)
                 << qint32(x)
                 << qint32(y)
                 << qint32(direction)
                 << BNetworkGlobals::NewLine;
}

void BClient::leave()
{
    m_dataStream << quint16(BNetworkGlobals::Leave)
                 << BNetworkGlobals::NewLine;
}

void BClient::mouseMove(int x, int y)
{
    m_dataStream << quint16(BNetworkGlobals::MouseMove)
                 << qint32(x)
                 << qint32(y)
                 << BNetworkGlobals::NewLine;
}

void BClient::mousePress(int button)
{
    m_dataStream << quint16(BNetworkGlobals::MousePress)
                 << qint32(button)
                 << BNetworkGlobals::NewLine;
}

void BClient::mouseRelease(int button)
{
    m_dataStream << quint16(BNetworkGlobals::MouseRelease)
                 << qint32(button)
                 << BNetworkGlobals::NewLine;
}

void BClient::keyPress(int keyCode, int modifiers)
{
    m_dataStream << quint16(BNetworkGlobals::KeyPress)
                 << qint32(keyCode)
                 << qint32(modifiers)
                 << BNetworkGlobals::NewLine;
}

void BClient::keyRelease(int keyCode, int modifiers)
{
    m_dataStream << quint16(BNetworkGlobals::KeyRelease)
                 << qint32(keyCode)
                 << qint32(modifiers)
                 << BNetworkGlobals::NewLine;
}

bool BClient::connected()
{
    return (m_connectionState == Connected);
}

void BClient::close()
{
    m_dataStream.setDevice(nullptr);
    disconnect(m_socket, nullptr, nullptr, nullptr);
    m_socket->close();
    delete m_socket;
    m_socket = nullptr;
}

QString BClient::hostname() const
{
    return m_name;
}

int BClient::hostScreenHeight() const
{
    return m_height;
}

int BClient::hostScreenWidth() const
{
    return m_width;
}

void BClient::readData()
{
    qDebug() << "readData";
    if (!m_socket->canReadLine())
    {
        return;
    }
    quint16 type;
    while(m_socket->canReadLine())
    {
        m_dataStream >> type;
        switch (type)
        {
            case BNetworkGlobals::Hello:
            {
                qint32 networkVersion;
                QChar newline;

                m_dataStream >> m_name;
                m_dataStream >> m_width;
                m_dataStream >> m_height;
                m_dataStream >> networkVersion;
                m_dataStream >> newline;

                if (networkVersion != BNetworkGlobals::BSynergyNetworkVersion)
                {
                    qDebug() << "network version mismatch, expected:" << BNetworkGlobals::BSynergyNetworkVersion
                             << " found:" << networkVersion;
                }

                if (newline != BNetworkGlobals::NewLine)
                {
                    qDebug() << "we have a problem";
                }
                else
                {
                    m_connectionState = Connected;
                    emit clientConnected(this);
                }
                break;
            }
            case BNetworkGlobals::MouseMove:
            {
                qint32 xMove, yMove;
                QChar newline;
                m_dataStream >> xMove;
                m_dataStream >> yMove;
                m_dataStream >> newline;
                if (newline != BNetworkGlobals::NewLine)
                {
                    qDebug() << "we have a problem";
                }
                else
                {
                    emit mouseMoved(xMove, yMove);
                }
                break;
            }
            case BNetworkGlobals::MousePress:
            {
                qint32 button;
                QChar newline;
                m_dataStream >> button;
                m_dataStream >> newline;
                if (newline != BNetworkGlobals::NewLine)
                {
                    qDebug() << "we have a problem";
                }
                else
                {
                    emit mousePressed(button);
                }
                break;
            }
            case BNetworkGlobals::MouseRelease:
            {
                qint32 button;
                QChar newline;
                m_dataStream >> button;
                m_dataStream >> newline;
                if (newline != BNetworkGlobals::NewLine)
                {
                    qDebug() << "we have a problem";
                }
                else
                {
                    emit mouseReleased(button);
                }
                break;
            }
            case BNetworkGlobals::KeyPress:
            {
                qint32 keyCode, modifiers;
                QChar newline;
                m_dataStream >> keyCode;
                m_dataStream >> modifiers;
                m_dataStream >> newline;
                if (newline != BNetworkGlobals::NewLine)
                {
                    qDebug() << "we have a problem";
                }
                else
                {
                    emit keyPressed(keyCode, modifiers);
                }
                break;
            }
            case BNetworkGlobals::KeyRelease:
            {
                qint32 keyCode, modifiers;
                QChar newline;
                m_dataStream >> keyCode;
                m_dataStream >> modifiers;
                m_dataStream >> newline;
                if (newline != BNetworkGlobals::NewLine)
                {
                    qDebug() << "we have a problem";
                }
                else
                {
                    emit keyReleased(keyCode, modifiers);
                }
                break;
            }
            case BNetworkGlobals::Enter:
            {
                qint32 xMove, yMove;
                qint32 direction;
                QChar newline;
                m_dataStream >> xMove;
                m_dataStream >> yMove;
                m_dataStream >> direction;
                m_dataStream >> newline;
                if (newline != BNetworkGlobals::NewLine)
                {
                    qDebug() << "we have a problem";
                }
                else
                {
                    emit entered(xMove, yMove, BScreenConfigGlobal::ScreenLayout(direction));
                }
                break;
            }
            case BNetworkGlobals::Leave:
            {
                QChar newline;
                m_dataStream >> newline;
                if (newline != BNetworkGlobals::NewLine)
                {
                    qDebug() << "we have a problem";
                }
                else
                {
                    emit left();
                }
                break;
            }
            default:
            {
                qDebug() << "unknown type" << type;
            }
        }
    }
}

void BClient::socketConnected()
{
    qDebug() << "socketConnected";
    m_dataStream.setDevice(m_socket);
    connect(m_socket, &QTcpSocket::readyRead, this, &BClient::readData);
    emit clientSocketConnected();
    if (m_socket->bytesAvailable() > 0)
        readData();
}

void BClient::socketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    
    qDebug() << "socketError";
    emit connectionError(m_socket->errorString());
}

#include "moc_bclient.cpp"
