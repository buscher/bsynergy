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


#ifndef BMSWINDOWSSCREEN_H
#define BMSWINDOWSSCREEN_H

#include "interface/iscreen.h"

class BMSWindowsThread;

class BMSWindowsScreen : public IScreen
{
    Q_OBJECT
public:
    BMSWindowsScreen(bool host, QObject* parent = 0);
    ~BMSWindowsScreen();

    bool host();

    bool open(QString *errorMessage = 0);
    bool close();

    bool grabControls(BScreenConfigGlobal::ScreenLayout direction);
    bool ungrabControls(BScreenConfigGlobal::ScreenLayout direction);

    void setHostScreenSize(int width, int height);

    QRect screenSize();

public slots:
    void fakeMouseMove(int x, int y);
    void fakeMousePressed(int button);
    void fakeMouseReleased(int button);

    void fakeKeyPressed(int keyCode, int modifiers = 0);
    void fakeKeyReleased(int keyCode, int modifiers = 0);

private:
    bool m_host;

    BMSWindowsThread* m_controlThread;

    //Display* m_display;

    //BX11ControlThread* m_controlThread;
};

#endif // BMSWINDOWSSCREEN_H
