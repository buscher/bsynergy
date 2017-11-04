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

#include <QDebug>

#include "bx11screen.h"

#include "bx11controlthread.h"

BX11Screen::BX11Screen(bool host, QObject* parent)
    : m_display(nullptr),
      m_controlThread(nullptr)
{
    Q_UNUSED(parent);

    m_host = host;
}

BX11Screen::~BX11Screen()
{
    if (m_display)
    {
        close();
    }
}

bool BX11Screen::host()
{
    return m_host;
}

bool BX11Screen::open(QString *errorMessage)
{
    // use qgetenv to keep VS2005 (and later) happy
    // as getenv deprecated since VS2005
    QByteArray displayName = qgetenv("DISPLAY");
    if (displayName.isEmpty())
    {
        displayName = ":0.0";
    }
    m_display = XOpenDisplay(displayName.constData());
    if (m_display == nullptr)
    {
        if (errorMessage)
            *errorMessage = QLatin1String("Failed to open Display");
        return false;
    }

    m_controlThread = new BX11ControlThread(m_display, host(), this);

    connect(m_controlThread, &BX11ControlThread::mouseMoved, this, &BX11Screen::mouseMoved);
    connect(m_controlThread, &BX11ControlThread::mousePressed, this, &BX11Screen::mousePressed);
    connect(m_controlThread, &BX11ControlThread::mouseReleased, this, &BX11Screen::mouseReleased);

    connect(m_controlThread, &BX11ControlThread::keyPressed, this, &BX11Screen::keyPressed);
    connect(m_controlThread, &BX11ControlThread::keyReleased, this, &BX11Screen::keyReleased);

    if (host())
    {
        m_controlThread->start();
    }

    return true;
}

bool BX11Screen::close()
{
    qDebug() << "BX11Screen::close";
    m_controlThread->canStop();
    m_controlThread->wait(100);
    delete m_controlThread;

    XCloseDisplay(m_display);
    m_display = nullptr;
    return true;
}

bool BX11Screen::grabControls(BScreenConfigGlobal::ScreenLayout direction)
{
    qDebug() << "grabControls";
    return m_controlThread->grabControls(direction);
}

bool BX11Screen::ungrabControls(BScreenConfigGlobal::ScreenLayout direction)
{
    qDebug() << "ungrabControls";
    return m_controlThread->ungrabControls(direction);
}

void BX11Screen::setHostScreenSize(int width, int height)
{
    m_controlThread->setHostScreenSize(width, height);
}

QRect BX11Screen::screenSize()
{
    Q_ASSERT(m_display != nullptr);

    return m_controlThread->screenSize();
}

void BX11Screen::fakeKeyPressed(int keyCode, int modifiers)
{
    m_controlThread->fakeKeyPressed(keyCode, modifiers);
}

void BX11Screen::fakeKeyReleased(int keyCode, int modifiers)
{
    m_controlThread->fakeKeyReleased(keyCode, modifiers);
}

void BX11Screen::fakeMouseMove(int x, int y)
{
    m_controlThread->fakeMouseMove(x, y);
}

void BX11Screen::fakeMousePressed(int button)
{
    m_controlThread->fakeMousePressed(button);
}

void BX11Screen::fakeMouseReleased(int button)
{
    m_controlThread->fakeMouseReleased(button);
}

#include "moc_bx11screen.cpp"
