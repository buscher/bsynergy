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


#include "bsynergyclient.h"

#include <QDebug>

#include <net/bclient.h>

#if defined(Q_OS_WIN32)
#   include <platform/bmswindowsscreen.h>
#elif defined(Q_OS_LINUX)
#   include <platform/bx11screen.h>
#else
#   error "unsupported OS"
#endif

BSynergyClient::BSynergyClient(QObject* parent)
    : QObject(parent)
{
#if defined(Q_OS_WIN32)
    m_screen = new BMSWindowsScreen(false, this);
#elif defined(Q_OS_LINUX)
    m_screen = new BX11Screen(false, this);
#else
#   error "unsupported OS"
#endif
    m_client = new BClient(this);
    connect(m_client, &BClient::clientSocketConnected, this, &BSynergyClient::clientSocketConnected);
    connect(m_client, &BClient::clientConnected, this, &BSynergyClient::clientConnected);

    connect(m_client, &BClient::mouseMoved, m_screen, &IScreen::fakeMouseMove);
    connect(m_client, &BClient::mousePressed, m_screen, &IScreen::fakeMousePressed);
    connect(m_client, &BClient::mouseReleased, m_screen, &IScreen::fakeMouseReleased);

    connect(m_client, &BClient::keyPressed, m_screen, &IScreen::fakeKeyPressed);
    connect(m_client, &BClient::keyReleased, m_screen, &IScreen::fakeKeyReleased);

    connect(m_client, &BClient::entered, this, &BSynergyClient::entered);

    connect(m_client, &BClient::connectionError, this, &BSynergyClient::connectionError);
    connect(m_client, &BClient::connectionError, this, &BSynergyClient::clientError);
}

bool BSynergyClient::connectTo(const QString& ip, quint16 port)
{
    return m_client->connectTo(ip, port);
}

bool BSynergyClient::openScreen()
{
    return m_screen->open();
}

void BSynergyClient::close()
{
    disconnect(m_screen, nullptr, nullptr, nullptr);
    m_screen->close();
    delete m_screen;
    m_screen = nullptr;
}

void BSynergyClient::clientSocketConnected()
{
    qDebug() << "clientSocketConnected";
    QRect size = m_screen->screenSize();
    qDebug() << "clientSocketConnected: about to setLocalScreenSize";
    m_client->setLocalScreenSize(size.width(), size.height());
}

void BSynergyClient::clientConnected(BClient* client)
{
    Q_ASSERT(client == m_client);

    int width = m_client->hostScreenWidth();
    int height = m_client->hostScreenHeight();
    m_screen->setHostScreenSize(width, height);
}

void BSynergyClient::entered(int x, int y, BScreenConfigGlobal::ScreenLayout direction)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(direction);

    qDebug() << "entered, what now?";
}

void BSynergyClient::clientError(const QString& text)
{
    qDebug() << "ERROR:" << text;
}


#include "moc_bsynergyclient.cpp"
