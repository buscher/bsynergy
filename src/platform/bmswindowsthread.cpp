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

#include "bmswindowsthread.h"
#include "bmswindowskeytranslator.h"

#include <zmouse.h>
#include <tchar.h>
#include <windows.h>
#include <winable.h>

// X button stuff
#if !defined(WM_XBUTTONDOWN)
#define WM_XBUTTONDOWN		0x020B
#define WM_XBUTTONUP		0x020C
#define WM_XBUTTONDBLCLK	0x020D
#define WM_NCXBUTTONDOWN	0x00AB
#define WM_NCXBUTTONUP		0x00AC
#define WM_NCXBUTTONDBLCLK	0x00AD
#define MOUSEEVENTF_XDOWN	0x0080
#define MOUSEEVENTF_XUP		0x0100
#define XBUTTON1			0x0001
#define XBUTTON2			0x0002
#endif

static HHOOK g_keyboardLL = NULL;
static HHOOK g_mouseLL = NULL;



static
LRESULT CALLBACK
keyboardLLHook(int code, WPARAM wParam, LPARAM lParam)
{
    qDebug() << "keyboardLLHook" << code;
    if (code >= 0)
    {
        // decode the message
        KBDLLHOOKSTRUCT* info = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        WPARAM wParam = info->vkCode;
        LPARAM lParam = 1;							// repeat code
        lParam      |= (info->scanCode << 16);		// scan code
        if (info->flags & LLKHF_EXTENDED)
        {
                lParam  |= (1lu << 24);					// extended key
        }
        if (info->flags & LLKHF_ALTDOWN)
        {
                lParam  |= (1lu << 29);					// context code
        }
        if (info->flags & LLKHF_UP)
        {
                lParam  |= (1lu << 31);					// transition
        }
        // FIXME -- bit 30 should be set if key was already down but
        // we don't know that info.  as a result we'll never generate
        // key repeat events.

        // handle the message
        BMSWindowsThread* thread = BMSWindowsThread::self();
        if (thread && thread->onKeyBoardEvent(wParam, lParam))
        {
            return 1;
        }
    }

    return CallNextHookEx(g_keyboardLL, code, wParam, lParam);
}


static
LRESULT CALLBACK
mouseLLHook(int code, WPARAM wParam, LPARAM lParam)
{
    qDebug() << "mouseLLHook" << code;
    if (code >= 0)
    {
        // decode the message
        MSLLHOOKSTRUCT* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        qint32 x = static_cast<qint32>(info->pt.x);
        qint32 y = static_cast<qint32>(info->pt.y);
        qint32 w = static_cast<qint32>(HIWORD(info->mouseData));

        // handle the message
        BMSWindowsThread* thread = BMSWindowsThread::self();
        if (thread && thread->onMouseEvent(wParam, x, y, w))
        {
                return 1;
        }
    }

    return CallNextHookEx(g_mouseLL, code, wParam, lParam);
}



BMSWindowsThread* BMSWindowsThread::m_instance = 0;

BMSWindowsThread::BMSWindowsThread(bool host, QObject* parent)
    : QThread(parent),
      m_host(host),
      m_run(true),
      m_grabbed(false),
      m_x(0),
      m_y(0),
      m_width(0),
      m_height(0),
      m_hostWidth(0),
      m_hostHeight(0)
{
    m_instance = this;
}

BMSWindowsThread::~BMSWindowsThread()
{
    m_instance = 0;
}

BMSWindowsThread* BMSWindowsThread::self()
{
    if (m_instance)
    {
        return m_instance;
    }
    else
    {
        return 0;
    }
}

bool BMSWindowsThread::open()
{
    qDebug() << "BMSWindowsThread::open";
    if (!g_mouseLL)
        g_mouseLL = SetWindowsHookEx(WH_MOUSE_LL,
                                 &mouseLLHook,
                                 GetModuleHandle(0),
                                 0);
    if (!g_keyboardLL)
        g_keyboardLL = SetWindowsHookEx(WH_KEYBOARD_LL,
                                    &keyboardLLHook,
                                    GetModuleHandle(0),
                                    0);
    if (g_mouseLL == NULL || g_keyboardLL == NULL)
    {
        if (g_keyboardLL != NULL)
        {
            qDebug() << "keyboard hook failed";
            UnhookWindowsHookEx(g_keyboardLL);
            g_keyboardLL = NULL;
        }
        if (g_mouseLL != NULL)
        {
            qDebug() << "maus hook failed";
            UnhookWindowsHookEx(g_mouseLL);
            g_mouseLL = NULL;
        }
    }
    return true;
}

bool BMSWindowsThread::host()
{
    return m_host;
}

void BMSWindowsThread::setHostScreenSize(int width, int height)
{
    m_hostWidth = width;
    m_hostHeight = height;
}

void BMSWindowsThread::canStop()
{
    m_mutex.lock();
    m_run = false;
    m_mutex.unlock();
}

bool BMSWindowsThread::grabControls(BScreenConfigGlobal::ScreenLayout direction)
{
    qDebug() << "BMSWindowsThread::grabControls";
    moveMouseToDirection(direction);
    m_grabbed = true;
    
// int WINAPI ShowCursor(
//   __in  BOOL bShow
// );
    
    return true;
}

bool BMSWindowsThread::ungrabControls(BScreenConfigGlobal::ScreenLayout direction)
{
    qDebug() << "BMSWindowsThread::ungrabControls";
    moveMouseToDirection(direction);
    m_grabbed = false;
    return true;
}

QRect BMSWindowsThread::screenSize()
{
    qDebug() << "screenSize START";
    m_x = 0;
    m_y = 0;
    m_mutex.lock();

    RECT rc;
    GetWindowRect(GetDesktopWindow(), &rc);

    m_width = static_cast<qint32>(rc.right);
    m_height = static_cast<qint32>(rc.bottom);
    m_mutex.unlock();

    qDebug() << "screenSize END" << m_x << m_y << m_width << m_height;
    return QRect(m_x, m_y, m_width, m_height);
}

void BMSWindowsThread::fakeKeyPressed(int keyCode, int modifiers)
{
}

void BMSWindowsThread::fakeKeyReleased(int keyCode, int modifiers)
{
    //MapVirtualKey(BMSWindowsKeyTranslator::qtKeyCodeToMSKeyCode(keyCode), )
    //send input, first modifiers down, then char and modifiers up...
}

void BMSWindowsThread::fakeMouseMove(int x, int y)
{
    SetCursorPos(x, y);
    qDebug() << "fakeMouseMove";
}

void BMSWindowsThread::fakeMousePressed(int button)
{
    if (button <= 3)
    {
        INPUT Input[1];
        ZeroMemory(Input, sizeof(INPUT));
        Input[0].type = INPUT_MOUSE;
        int flag = 0;
        switch (button)
        {
        case 1: flag = MOUSEEVENTF_LEFTDOWN; break;
        case 2: flag = MOUSEEVENTF_MIDDLEDOWN; break;
        case 3: flag = MOUSEEVENTF_RIGHTDOWN; break;
        }
        Input[0].mi.dwFlags = flag;

        SendInput(1, Input, sizeof(INPUT));
    }
    else
    {
        INPUT Input[1];
        ZeroMemory(Input, sizeof(INPUT));
        Input[0].type = INPUT_MOUSE;
        int flag = 0;
        switch (button)
        {
        case 4: flag = 120; break;
        case 5: flag = 65416; break;
        }
        Input[0].mi.dwFlags = MOUSEEVENTF_WHEEL;

        Input[0].mi.mouseData = flag;

        SendInput(1, Input, sizeof(INPUT));
    }
}

void BMSWindowsThread::fakeMouseReleased(int button)
{
    if (button <= 3)
    {
        INPUT Input[1];
        ZeroMemory(Input, sizeof(INPUT));
        Input[0].type = INPUT_MOUSE;
        int flag = 0;
        switch (button)
        {
        case 1: flag = MOUSEEVENTF_LEFTDOWN; break;
        case 2: flag = MOUSEEVENTF_MIDDLEDOWN; break;
        case 3: flag = MOUSEEVENTF_RIGHTDOWN; break;
        }
        Input[0].mi.dwFlags = flag;

        SendInput(1, Input, sizeof(INPUT));
    }
    else
    {
        INPUT Input[1];
        ZeroMemory(Input, sizeof(INPUT));
        Input[0].type = INPUT_MOUSE;
        int flag = 0;
        switch (button)
        {
        case 4: flag = 120; break;
        case 5: flag = 65416; break;
        }
        Input[0].mi.dwFlags = MOUSEEVENTF_WHEEL;

        Input[0].mi.mouseData = flag;

        SendInput(1, Input, sizeof(INPUT));
    }
}

bool BMSWindowsThread::onMouseEvent(WPARAM wParam, qint32 x, qint32 y, qint32 data)
{
    switch (wParam)
    {
    case WM_LBUTTONDOWN:
        qDebug() << "down" << 1;
        onButtonPress(1);
        return m_grabbed;
    case WM_MBUTTONDOWN:
        qDebug() << "down" << 2;
        onButtonPress(2);
        return m_grabbed;
    case WM_RBUTTONDOWN:
        qDebug() << "down" << 3;
        onButtonPress(3);
        return m_grabbed;
    case WM_XBUTTONDOWN:
        qDebug() << "down" << data;
        onButtonPress(data);
        return m_grabbed;

    case WM_NCLBUTTONDOWN:
        qDebug() << "down" << 1;
        onButtonPress(1);
        return m_grabbed;
    case WM_NCMBUTTONDOWN:
        qDebug() << "down" << 2;
        onButtonPress(2);
        return m_grabbed;
    case WM_NCRBUTTONDOWN:
        qDebug() << "down" << 3;
        onButtonPress(3);
        return m_grabbed;
    case WM_NCXBUTTONDOWN:
        qDebug() << "down" << data;
        onButtonPress(data);
        return m_grabbed;

    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_XBUTTONDBLCLK:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCMBUTTONDBLCLK:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCXBUTTONDBLCLK:
        qDebug() << "click" << data;
        return false;

    case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
        qDebug() << "up" << 1;
        onButtonRelease(1);
        return m_grabbed;
    case WM_MBUTTONUP:
    case WM_NCMBUTTONUP:
        qDebug() << "up" << 2;
        onButtonRelease(2);
        return m_grabbed;
    case WM_RBUTTONUP:
    case WM_NCRBUTTONUP:
        qDebug() << "up" << 3;
        onButtonRelease(3);
        return m_grabbed;
    case WM_XBUTTONUP:
    case WM_NCXBUTTONUP:
        qDebug() << "up" << data;
        onButtonRelease(data);
        return m_grabbed;

    case WM_MOUSEWHEEL:
        if (data <= 120)
        {
            onButtonPress(4);
            onButtonRelease(4);
        }
        else
        {
            onButtonPress(5);
            onButtonRelease(5);
        }
        qDebug() << "wheel" <<  data;
        return m_grabbed;

    case WM_NCMOUSEMOVE:
    case WM_MOUSEMOVE:
        qDebug() << "move" << x << y << data;
        onMotionNotify(x, y);
        return m_grabbed;
    }

    // pass the event
    return false;
}

bool BMSWindowsThread::onKeyBoardEvent(WPARAM wParam, LPARAM lParam)
{
    qDebug() << wParam << lParam;
    if (lParam > 0)
    {
        onKeyPress(BMSWindowsKeyTranslator::msKeyCodeToQt(wParam), BMSWindowsKeyTranslator::msKeyCodeToQtModifier());
    }
    else
    {
        onKeyRelease(BMSWindowsKeyTranslator::msKeyCodeToQt(wParam), BMSWindowsKeyTranslator::msKeyCodeToQtModifier());
    }

    return m_grabbed;
}

void BMSWindowsThread::run()
{
    qDebug() << "RUN END";
}

void BMSWindowsThread::onMotionNotify(int x, int y)
{
    Q_ASSERT(host() == true);
    emit mouseMoved(x, y);
}

void BMSWindowsThread::onKeyPress(int keycode, int modifier)
{
    Q_ASSERT(host() == true);
    emit keyPressed(keycode, modifier);
}

void BMSWindowsThread::onKeyRelease(int keycode, int modifier)
{
    Q_ASSERT(host() == true);
    emit keyPressed(keycode, modifier);
}

void BMSWindowsThread::onButtonPress(int button)
{
    Q_ASSERT(host() == true);

    emit mousePressed(button);
}

void BMSWindowsThread::onButtonRelease(int button)
{
    Q_ASSERT(host() == true);

    emit mouseReleased(button);
}

void BMSWindowsThread::moveMouseToDirection(BScreenConfigGlobal::ScreenLayout direction)
{
    POINT out;
    GetCursorPos(&out);
    int destX, rootX = out.x;
    int destY, rootY = out.y;

    switch (direction)
    {
        case BScreenConfigGlobal::AboveOf:
        {
            destX = rootX;
            destY = m_height-2;
            break;
        }
        case BScreenConfigGlobal::BelowOf:
        {
            destX = rootX;
            destY = 1;
            break;
        }
        case BScreenConfigGlobal::RightOf:
        {
            destX = 1;
            destY = rootY;
            break;
        }
        case BScreenConfigGlobal::LeftOf:
        {
            destX = m_width-2;
            destY = rootY;
            break;
        }
    }

    qDebug() << "SetCursorPos(destX, destY);";
    SetCursorPos(destX, destY);
}

#include "moc_bmswindowsthread.cpp"
