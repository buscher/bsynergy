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


#ifndef BX11CONTROLTHREAD_H
#define BX11CONTROLTHREAD_H

#include <X11/Xlib.h>

#include <QThread>
#include <QMutex>
#include <QSet>
#include <QRect>

#include "base/bscreenconfigglobal.h"

class BX11ControlThread : public QThread
{
    Q_OBJECT
public:
    BX11ControlThread(Display *display, bool host, QObject* parent);
    ~BX11ControlThread();

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

protected:
    void onDestroyNotify(XEvent* event);
    void onMapNotify(XEvent* event);
    void onMotionNotify(XEvent* event);
    void onKeyPress(XEvent* event);
    void onKeyRelease(XEvent* event);
    void onButtonPress(XEvent* event);
    void onButtonRelease(XEvent* event);

signals:
    void mouseMoved(int x, int y);
    void mousePressed(int button);
    void mouseReleased(int button);

    void keyPressed(int keyCode, int modifiers = 0);
    void keyReleased(int keyCode, int modifiers = 0);

private:
    void updateNotifyMask();
    void selectMotionMask(Display* display, Window window);
    void sendDummyEvent();
    Cursor createBlankCursor() const;
    void moveMouseToDirection(BScreenConfigGlobal::ScreenLayout direction);

    Display *m_display;
    Window m_root;
    bool m_host;
    bool m_run;
    bool m_grabbed;

    int m_x;
    int m_y;
    int m_width;
    int m_height;

    int m_hostWidth;
    int m_hostHeight;

    bool m_trackSubstructureNotify;

    long int m_notifyMask;

    QMutex m_mutex;
    QSet<Window> m_windowList;
    Window m_fakeWindow;
};

#endif // BX11CONTROLTHREAD_H
