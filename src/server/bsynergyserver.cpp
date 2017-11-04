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


#include "bsynergyserver.h"

#include <QDebug>
#include <QHostInfo>

#include <net/blistenserver.h>
#include <net/bclient.h>

#if defined(Q_OS_WIN32)
#   include <platform/bmswindowsscreen.h>
#elif defined(Q_OS_LINUX)
#   include <platform/bx11screen.h>
#else
#   error "unsupported OS"
#endif

BSynergyServer::BSynergyServer(const BScreenConfigGlobal::ScreenLayoutMap& rules, QObject* parent)
    : QObject(parent),
      m_rules(rules),
      m_currentClient(nullptr)
{
#if defined(Q_OS_WIN32)
    m_screen = new BMSWindowsScreen(true, this);
#elif defined(Q_OS_LINUX)
    m_screen = new BX11Screen(true, this);
#else
#   error "unsupported OS"
#endif

    QString errorMsg;
    if (!m_screen->open(&errorMsg))
    {
        qDebug() << errorMsg;
        Q_ASSERT(false);
        return;
    }

    QRect screenRect = m_screen->screenSize();
    m_x = screenRect.x();
    m_y = screenRect.y();
    m_width = screenRect.width();
    m_height = screenRect.height();

    m_listenServer = new BListenServer(this);
    if (!m_listenServer->listen())
    {
        Q_ASSERT(false);
        return;
    }

    m_hostname = QHostInfo::localHostName();
    m_currentScreen = m_hostname;

    connect(m_screen, SIGNAL(mouseMoved(int, int)), this, SLOT(mouseMoved(int, int)));
    // TODO: this does not work, figure out how to do it...
//     connect(m_screen, &IScreen::mouseMoved, this, &BSynergyServer::mouseMoved);

    connect(m_listenServer, &BListenServer::newClient, this, &BSynergyServer::newClient);
}

void BSynergyServer::mouseMoved(int x, int y)
{
    QString destHostname;
    BScreenConfigGlobal::ScreenLayout direction;
    qDebug() << "m_currentScreen" << m_currentScreen;
    if (x <= m_x)
    {
        destHostname = findRule(m_currentScreen, BScreenConfigGlobal::LeftOf);
        direction = BScreenConfigGlobal::LeftOf;
    }
    else if (x >= m_width-1)
    {
        destHostname = findRule(m_currentScreen, BScreenConfigGlobal::RightOf);
        direction = BScreenConfigGlobal::RightOf;
    }
    else if (y <= m_y)
    {
        destHostname = findRule(m_currentScreen, BScreenConfigGlobal::AboveOf);
        direction = BScreenConfigGlobal::AboveOf;
    }
    else if (y >= m_height-1)
    {
        destHostname = findRule(m_currentScreen, BScreenConfigGlobal::BelowOf);
        direction = BScreenConfigGlobal::BelowOf;
    }
    else
    {
        qDebug() << "mouse did not move? " << x << y;
        return;
    }
    qDebug() << destHostname << destHostname.isEmpty() << x << y;

    if (!destHostname.isEmpty())
    {
        if (destHostname == m_hostname)
        {
            if (m_currentClient)
            {
                disconnectServerClientSignals(m_currentClient);
                m_currentClient->leave();
            }
            m_screen->ungrabControls(direction);
            m_currentClient = nullptr;
            m_currentScreen = m_hostname;
        }
        else if (m_listenServer->clientConnected(destHostname))
        {
            if (m_currentClient)
            {
                disconnectServerClientSignals(m_currentClient);
                m_currentClient->leave();
            }
            m_screen->grabControls(direction);
            m_currentClient = m_listenServer->clientByName(destHostname);
            m_currentClient->enter(x, y, direction);
            m_currentScreen = m_currentClient->hostname();
            connectServerClientSignals(m_currentClient);
        }
        else if (!m_listenServer->clientConnected(destHostname))
        {
            qDebug() << "found destHostName:" << destHostname << "but it is not connected";
        }
        return;
    }

    if (m_currentClient)
    {
        m_currentClient->mouseMove(x, y);
    }
}

void BSynergyServer::newClient(BClient* client)
{
    qDebug() << "new client, send local screen size";
    connect(client, &BClient::clientConnected, this, &BSynergyServer::clientConnected);
    client->setLocalScreenSize(m_width, m_height);
}

void BSynergyServer::clientConnected(BClient* client)
{
    qDebug() << "connected:" << client->hostname() << client->hostScreenWidth() << client->hostScreenHeight();
}

void BSynergyServer::close()
{
    disconnect(m_listenServer, nullptr, nullptr, nullptr);
    delete m_listenServer;
    m_listenServer = nullptr;

    disconnect(m_screen, nullptr, nullptr, nullptr);
    m_screen->close();
    delete m_screen;
    m_screen = nullptr;
}

QString BSynergyServer::findRule(const QString& host, BScreenConfigGlobal::ScreenLayout direction)
{
    BScreenConfigGlobal::ScreenLayoutList list = m_rules.values(host);
    foreach (const BScreenConfigGlobal::ScreenLayoutStruct& layout, list)
    {
        if (layout.connection == direction)
        {
            return layout.name;
        }
    }

    return QString();
}

void BSynergyServer::connectServerClientSignals(BClient* client)
{
    connect(m_screen, SIGNAL(mousePressed(int)), client, SLOT(mousePress(int)));
    connect(m_screen, SIGNAL(mouseReleased(int)), client, SLOT(mouseRelease(int)));

    connect(m_screen, SIGNAL(keyPressed(int,int)), client, SLOT(keyPress(int,int)));
    connect(m_screen, SIGNAL(keyReleased(int,int)), client, SLOT(keyRelease(int, int)));
}

void BSynergyServer::disconnectServerClientSignals(BClient* client)
{
    disconnect(m_screen, SIGNAL(mousePressed(int)), client, SLOT(mousePress(int)));
    disconnect(m_screen, SIGNAL(mouseReleased(int)), client, SLOT(mouseRelease(int)));

    disconnect(m_screen, SIGNAL(keyPressed(int,int)), client, SLOT(keyPress(int,int)));
    disconnect(m_screen, SIGNAL(keyReleased(int,int)), client, SLOT(keyRelease(int, int)));
}

#include "moc_bsynergyserver.cpp"
