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


#ifndef BX11SCREEN_H
#define BX11SCREEN_H

#include "interface/iscreen.h"

#include <X11/Xlib.h>

class BX11ControlThread;

class BX11Screen : public IScreen
{
    Q_OBJECT
    Q_INTERFACES(IScreen)
public:
    BX11Screen(bool host, QObject* parent = nullptr);
    ~BX11Screen();

    bool host();

    bool open(QString *errorMessage = nullptr);
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

signals:
    void mouseMoved(int x, int y);
    void mousePressed(int button);
    void mouseReleased(int button);

    void keyPressed(int keyCode, int modifiers = 0);
    void keyReleased(int keyCode, int modifiers = 0);

private:
    bool m_host;

    Display* m_display;

    BX11ControlThread* m_controlThread;
};

#endif // BX11SCREEN_H
