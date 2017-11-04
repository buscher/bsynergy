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


#ifndef BMSWINDOWSTHREAD_H
#define BMSWINDOWSTHREAD_H

#include <windows.h>

#include <QThread>
#include <QMutex>
#include <QSet>
#include <QRect>

#include "base/bscreenconfigglobal.h"

class Q_DECL_EXPORT BMSWindowsThread : public QThread
{
    Q_OBJECT
public:
    BMSWindowsThread(bool host, QObject* parent);
    ~BMSWindowsThread();

    static BMSWindowsThread* self();

    bool open();

    bool host();
    void setHostScreenSize(int width, int height);

    void run();

    void canStop();

    bool grabControls(BScreenConfigGlobal::ScreenLayout direction);
    bool ungrabControls(BScreenConfigGlobal::ScreenLayout direction);

    QRect screenSize();

    void fakeMouseMove(int x, int y);
    void fakeMousePressed(int button);
    void fakeMouseReleased(int button);

    void fakeKeyPressed(int keyCode, int modifiers = 0);
    void fakeKeyReleased(int keyCode, int modifiers = 0);

    bool onMouseEvent(WPARAM wParam, qint32 x, qint32 y, qint32 data);
    bool onKeyBoardEvent(WPARAM wParam, LPARAM lParam);

protected:
    void onMotionNotify(int x, int y);
    void onKeyPress(int keycode, int modifier);
    void onKeyRelease(int keycode, int modifier);
    void onButtonPress(int button);
    void onButtonRelease(int button);

signals:
    void mouseMoved(int x, int y);
    void mousePressed(int button);
    void mouseReleased(int button);

    void keyPressed(int keyCode, int modifiers = 0);
    void keyReleased(int keyCode, int modifiers = 0);

private:
    inline void moveMouseToDirection(BScreenConfigGlobal::ScreenLayout direction);

    static BMSWindowsThread* m_instance;

    bool m_host;
    bool m_run;
    bool m_grabbed;

    int m_x;
    int m_y;
    int m_width;
    int m_height;

    int m_hostWidth;
    int m_hostHeight;

    QMutex m_mutex;
};

#endif // BMSWINDOWSTHREAD_H
