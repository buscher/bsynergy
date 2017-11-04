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

#include "bmswindowsscreen.h"

#include "bmswindowsthread.h"

BMSWindowsScreen::BMSWindowsScreen(bool host, QObject* parent)
    : m_controlThread(0)
{
    m_host = host;
}

BMSWindowsScreen::~BMSWindowsScreen()
{
    close();
}

bool BMSWindowsScreen::host()
{
    return m_host;
}

bool BMSWindowsScreen::open(QString *errorMessage)
{
    m_controlThread = new BMSWindowsThread(m_host, this);
    connect(m_controlThread, &BMSWindowsThread::mouseMoved, this, &BMSWindowsScreen::mouseMoved);
    connect(m_controlThread, &BMSWindowsThread::mousePressed, this, &BMSWindowsScreen::mousePressed);
    connect(m_controlThread, &BMSWindowsThread::mouseReleased, this, &BMSWindowsScreen::mouseReleased);

    connect(m_controlThread, &BMSWindowsThread::keyPressed, this, &BMSWindowsScreen::keyPressed);
    connect(m_controlThread, &BMSWindowsThread::keyReleased, this, &BMSWindowsScreen::keyReleased);

    //if (host())
    //{
    //    m_controlThread->start();
    //}

    return m_controlThread->open();
}

bool BMSWindowsScreen::close()
{
    return true;
}

bool BMSWindowsScreen::grabControls(BScreenConfigGlobal::ScreenLayout direction)
{
    qDebug() << "grabControls";
    return m_controlThread->grabControls(direction);
}

bool BMSWindowsScreen::ungrabControls(BScreenConfigGlobal::ScreenLayout direction)
{
    qDebug() << "ungrabControls";
    return m_controlThread->ungrabControls(direction);
}

void BMSWindowsScreen::setHostScreenSize(int width, int height)
{
    m_controlThread->setHostScreenSize(width, height);
}

QRect BMSWindowsScreen::screenSize()
{
    return m_controlThread->screenSize();
}

void BMSWindowsScreen::fakeKeyPressed(int keyCode, int modifiers)
{
    m_controlThread->fakeKeyPressed(keyCode, modifiers);
}

void BMSWindowsScreen::fakeKeyReleased(int keyCode, int modifiers)
{
    m_controlThread->fakeKeyReleased(keyCode, modifiers);
}

void BMSWindowsScreen::fakeMouseMove(int x, int y)
{
    m_controlThread->fakeMouseMove(x, y);
}

void BMSWindowsScreen::fakeMousePressed(int button)
{
    m_controlThread->fakeMousePressed(button);
}

void BMSWindowsScreen::fakeMouseReleased(int button)
{
    m_controlThread->fakeMouseReleased(button);
}

#include "moc_bmswindowsscreen.cpp"
