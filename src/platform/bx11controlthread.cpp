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

#include <X11/XKBlib.h>
#include <X11/extensions/XTest.h>

#include "bx11controlthread.h"
#include "bx11keytranslator.h"


BX11ControlThread::BX11ControlThread(Display* display, bool host, QObject* parent)
    : QThread(parent),
      m_display(display),
      m_host(host),
      m_run(true),
      m_grabbed(false),
      m_x(0),
      m_y(0),
      m_width(0),
      m_height(0),
      m_hostWidth(0),
      m_hostHeight(0),
      m_trackSubstructureNotify(true),
      m_notifyMask(0),
      m_fakeWindow(0)
{
    m_root = DefaultRootWindow(display);
    updateNotifyMask();
    if (m_host)
    {
        selectMotionMask(m_display, m_root);
    }
}

BX11ControlThread::~BX11ControlThread()
{

}

bool BX11ControlThread::host()
{
    return m_host;
}

void BX11ControlThread::setHostScreenSize(int width, int height)
{
    m_hostWidth = width;
    m_hostHeight = height;
}

void BX11ControlThread::canStop()
{
    qDebug() << "canStop";

    m_mutex.lock();
    m_run = false;
    m_mutex.unlock();

    qDebug() << "canStop END";
}

bool BX11ControlThread::grabControls(BScreenConfigGlobal::ScreenLayout direction)
{
    qDebug() << "BX11ControlThread::grabControls";


    m_run = false;
    qDebug() << "BX11ControlThread::grabControls::sendDummyEvent";
    sendDummyEvent();

    qDebug() << "BX11ControlThread::grabControls::lock mutex";
    m_mutex.lock();
    //int ret = XGrabKeyboard(m_display, m_root, True, GrabModeAsync, GrabModeAsync, CurrentTime);


    if (!m_fakeWindow)
    {
        qDebug() << "m_fakeWindow";
        XSetWindowAttributes attr;
        attr.do_not_propagate_mask = 0;
        attr.override_redirect = True;
        attr.cursor = createBlankCursor();
        attr.event_mask = PointerMotionMask |
                             ButtonPressMask | ButtonReleaseMask |
                             KeyPressMask | KeyReleaseMask |
                             KeymapStateMask | PropertyChangeMask;
        qDebug() << "BX11ControlThread::grabControls::xcreatewindow";
        m_fakeWindow = XCreateWindow(m_display, m_root, 0, 0, m_width, m_height, 0, 0,
                            InputOnly, nullptr /*CopyFromParent*/,
                            CWDontPropagate | CWEventMask |
                            CWOverrideRedirect | CWCursor,
                            &attr);
    }

    qDebug() << "BX11ControlThread::grabControls::moveMouseToDirection";
    moveMouseToDirection(direction);

    int ret = 0;
    if (!m_grabbed)
    {
        qDebug() << "BX11ControlThread::grabControls::xmapwindow";
        XMapWindow(m_display, m_fakeWindow);

        ret = XGrabPointer(m_display, m_root, True, PointerMotionMask|ButtonPressMask|ButtonReleaseMask, GrabModeAsync, GrabModeAsync, m_fakeWindow, None, CurrentTime);
        qDebug() << "BX11ControlThread::grabControls GRABBED";
        //int ret2 = XSelectInput(m_display, m_root, KeyPressMask | KeyReleaseMask);

        int retKeyb = XGrabKeyboard(m_display, m_root, True, GrabModeAsync, GrabModeAsync, CurrentTime);
        qDebug() << "keyboard grab" << retKeyb;

        //PointerMotionMask | PointerMotionHintMask | Button1MotionMask | Button2MotionMask | ButtonMotionMask | EnterWindowMask | LeaveWindowMask |
        int ret2 = XSelectInput(m_display, m_fakeWindow, ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask);
        Q_UNUSED(ret2);
        ret2 = XSelectInput(m_display, m_root, PointerMotionHintMask | KeyPressMask | KeyReleaseMask);
        Q_UNUSED(ret2);
        m_grabbed = true;
    }

    qDebug() << "BX11ControlThread::unlock mutex";
    m_mutex.unlock();

    qDebug() << "start";
    m_run = true;
    start();

    qDebug() << ret;
    qDebug() << "BX11ControlThread::grabControls END";
    return ret == GrabSuccess;
}

bool BX11ControlThread::ungrabControls(BScreenConfigGlobal::ScreenLayout direction)
{
    qDebug() << "BX11ControlThread::ungrabControls";
    int ret = 0;
    sendDummyEvent();
    m_mutex.lock();
    if (m_grabbed)
    {
        int retKeyboard = XUngrabKeyboard(m_display, CurrentTime);
        Q_UNUSED(retKeyboard);
        ret = XUngrabPointer(m_display, CurrentTime);

        moveMouseToDirection(direction);

        XUnmapWindow(m_display, m_fakeWindow);

        XSelectInput(m_display, m_root, MotionNotify | m_notifyMask);
        m_grabbed = false;
    }
    m_mutex.unlock();
    qDebug() << "BX11ControlThread::ungrabControls END";

    return ret == Success;
}

QRect BX11ControlThread::screenSize()
{
    qDebug() << "screenSize START";
    //TODO: support xinerama
    m_x = 0;
    m_y = 0;
    m_mutex.lock();
    m_width = WidthOfScreen(DefaultScreenOfDisplay(m_display));
    m_height = HeightOfScreen(DefaultScreenOfDisplay(m_display));
    m_mutex.unlock();

    qDebug() << "screenSize END";
    return QRect(m_x, m_y, m_width, m_height);
}

void BX11ControlThread::fakeKeyPressed(int keyCode, int modifiers)
{
//     unsigned int xKeyCode = BX11KeyTranslator::qtKeyCodeToKeySym(keyCode);
//     qDebug() << "fakeKeyPressed" << keyCode << xKeyCode << XKeysymToString(xKeyCode);
//     if (xKeyCode != -1)
//     {
//         int ret = XTestFakeKeyEvent(m_display, XKeysymToKeycode(m_display, xKeyCode), True,  CurrentTime);
//     //             XTestFakeKeyEvent(m_display, keycode, False, CurrentTime);
//         qDebug() << ret;
//         XFlush(m_display);
//     }
//     else
//     {
//         qDebug() << "unknown keycode:" << keyCode;
//     }

    unsigned int xkeyCode = BX11KeyTranslator::qtKeyCodeToKeySym(keyCode);
    qDebug() << "fakeKeyPressed" << keyCode << xkeyCode << XKeysymToString(xkeyCode);

    Window winFocus;
    int    revert;
    XGetInputFocus(m_display, &winFocus, &revert);

    int xKeyCode = BX11KeyTranslator::qtKeyCodeToKeySym(keyCode);
    if (xKeyCode == -1)
    {
        qDebug() << "unknown press keycode:" << keyCode;
        return;
    }
    int xModifier = BX11KeyTranslator::qtModifierToXKeyState(modifiers);
    qDebug() << "xModifier" << xModifier << modifiers;

    XKeyEvent event;
    event.display = m_display;
    event.window = winFocus;
    event.root = m_root;
    event.subwindow = None;
    event.time = CurrentTime;
    event.x = 1;
    event.y = 1;
    event.x_root = 1;
    event.y_root = 1;
    event.same_screen = True;
    event.keycode = XKeysymToKeycode(m_display, xKeyCode);
    event.state = xModifier;
    event.type = KeyPress;

    XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
    XFlush(m_display);
}

void BX11ControlThread::fakeKeyReleased(int keyCode, int modifiers)
{
//     unsigned int xKeyCode = BX11KeyTranslator::qtKeyCodeToKeySym(keyCode);
//     qDebug() << "fakeKeyReleased" << keyCode << xKeyCode << XKeysymToString(xKeyCode);
//     if (xKeyCode != -1)
//     {
//         int ret = XTestFakeKeyEvent(m_display, XKeysymToKeycode(m_display, xKeyCode), False,  CurrentTime);
//         qDebug() << ret;
//         XFlush(m_display);
//     }
//     else
//     {
//         qDebug() << "unknown keycode:" << keyCode;
//     }

    unsigned int xkeyCode = BX11KeyTranslator::qtKeyCodeToKeySym(keyCode);
    qDebug() << "fakeKeyReleased" << keyCode << xkeyCode << XKeysymToString(xkeyCode);

    Window winFocus;
    int    revert;
    XGetInputFocus(m_display, &winFocus, &revert);

    int xKeyCode = BX11KeyTranslator::qtKeyCodeToKeySym(keyCode);
    if (xKeyCode == -1)
    {
        qDebug() << "unknown release keycode:" << keyCode;
        return;
    }
    int xModifier = BX11KeyTranslator::qtModifierToXKeyState(modifiers);
    qDebug() << "xModifier" << xModifier << modifiers;

    XKeyEvent event;
    event.display = m_display;
    event.window = winFocus;
    event.root = m_root;
    event.subwindow = None;
    event.time = CurrentTime;
    event.x = 1;
    event.y = 1;
    event.x_root = 1;
    event.y_root = 1;
    event.same_screen = True;
    event.keycode = XKeysymToKeycode(m_display, xKeyCode);
    event.state = xModifier;
    event.type = KeyRelease;

    XSendEvent(event.display, event.window, True, KeyReleaseMask, (XEvent *)&event);
    XFlush(m_display);
}

void BX11ControlThread::fakeMouseMove(int x, int y)
{
    qDebug() << "fakeMouseMove";
    qDebug() << "m_display" << m_display << "m_root" << m_root;
//     Window root, child;
//     int rootX, rootY, winX, winY;
    int destX, destY;
//     unsigned int mask;
//     bool retPointer = XQueryPointer(m_display, m_root, &root, &child, &rootX, &rootY, &winX, &winY, &mask);
//     if (retPointer == False)
//     {
//         qDebug() << "query pointer failed, hm.. what now?" << retPointer;
//     }

    destX = (float)x * ((float)m_width / (float)m_hostWidth);
    destY = (float)y * ((float)m_height / (float)m_hostHeight);

    qDebug() << x << y << m_width << m_height << m_hostWidth << m_hostHeight;
    qDebug() << "destX/Y" << destX << destY;

    XWarpPointer(m_display, m_root, m_root, 0, 0, 0, 0, destX, destY);
    XFlush(m_display);
}

void BX11ControlThread::fakeMousePressed(int button)
{
    XTestFakeButtonEvent(m_display, button, True, CurrentTime);
    XFlush(m_display);
}

void BX11ControlThread::fakeMouseReleased(int button)
{
    XTestFakeButtonEvent(m_display, button, False, CurrentTime);
    XFlush(m_display);
}

void BX11ControlThread::run()
{
    XEvent event;
    qDebug() << m_run;

    int x11_fd;
    fd_set in_fds;
    struct timeval tv;
    x11_fd = ConnectionNumber(m_display);

    while (m_run)
    {
//         qDebug() << "NEXT";

        // Create a File Description Set containing x11_fd
        FD_ZERO(&in_fds);
        FD_SET(x11_fd, &in_fds);

        tv.tv_usec = 1 * 100 * 1000;
        tv.tv_sec = 0;

        // Wait for X Event or a Timer
        int num_ready_fds = select(x11_fd + 1, &in_fds, nullptr, nullptr, &tv);
        if (num_ready_fds > 0)
        {
            qDebug() << "Event Received!";
        }
        else if (num_ready_fds == 0)
        {
            // Handle timer here
//             qDebug() << "Timer Fired!";
            continue;
        }
        else
        {
            qDebug() << "An error occured!";
            return;
        }

        while (XPending(m_display) && m_run)
        {
            m_mutex.lock();
            XNextEvent(m_display, &event);
            m_mutex.unlock();

    //         m_mutex.lock();
    //         qDebug() << "XNextEvent";
    //         XNextEvent(m_display, &event);
    //         m_mutex.unlock();

            //m_mutex.lock();
            qDebug() << "switch (event.type)" << event.type;
            switch (event.type)
            {
            case KeyPress:
                qDebug() << "KeyPress";
    //             prologue (&event, "KeyPress");
    //             do_KeyPress (&event);
                onKeyPress(&event);
                break;
            case KeyRelease:
                qDebug() << "KeyRelease";
                onKeyRelease(&event);
    //             prologue (&event, "KeyRelease");
    //             do_KeyRelease (&event);
                break;
            case ButtonPress:
                qDebug() << "ButtonPress";
                onButtonPress(&event);
    //             prologue (&event, "ButtonPress");
    //             do_ButtonPress (&event);
                break;
            case ButtonRelease:
                qDebug() << "ButtonRelease";
                onButtonRelease(&event);
    //             prologue (&event, "ButtonRelease");
    //             do_ButtonRelease (&event);
                break;
            case MotionNotify:
    //             prologue (&event, "MotionNotify");
    //             do_MotionNotify (&event);
                //qDebug() << "MotionNotify";
                onMotionNotify(&event);
                break;
            case EnterNotify:
                qDebug() << "EnterNotify";
    //             prologue (&event, "EnterNotify");
    //             do_EnterNotify (&event);
                break;
            case LeaveNotify:
                qDebug() << "LeaveNotify";
    //             prologue (&event, "LeaveNotify");
    //             do_LeaveNotify (&event);
                break;
            case FocusIn:
    //             prologue (&event, "FocusIn");
    //             do_FocusIn (&event);
                break;
            case FocusOut:
    //             prologue (&event, "FocusOut");
    //             do_FocusOut (&event);
                break;
            case KeymapNotify:
    //             prologue (&event, "KeymapNotify");
    //             do_KeymapNotify (&event);
                break;
            case Expose:
    //             prologue (&event, "Expose");
    //             do_Expose (&event);
                break;
            case GraphicsExpose:
    //             prologue (&event, "GraphicsExpose");
    //             do_GraphicsExpose (&event);
                break;
            case NoExpose:
    //             prologue (&event, "NoExpose");
    //             do_NoExpose (&event);
                break;
            case VisibilityNotify:
    //             prologue (&event, "VisibilityNotify");
    //             do_VisibilityNotify (&event);
                break;
            case CreateNotify:
    //             prologue (&event, "CreateNotify");
    //             do_CreateNotify (&event);
                break;
            case DestroyNotify:
    //             prologue (&event, "DestroyNotify");
    //             do_DestroyNotify (&event);
                onDestroyNotify(&event);
                break;
            case UnmapNotify:
    //             prologue (&event, "UnmapNotify");
    //             do_UnmapNotify (&event);
                break;
            case MapNotify:
    //             prologue (&event, "MapNotify");
                qDebug() << "MapNotify";
                onMapNotify(&event);
                break;
            case MapRequest:
    //             prologue (&event, "MapRequest");
    //             do_MapRequest (&event);
                break;
            case ReparentNotify:
    //             prologue (&event, "ReparentNotify");
    //             do_ReparentNotify (&event);
                break;
            case ConfigureNotify:
    //             prologue (&event, "ConfigureNotify");
    //             do_ConfigureNotify (&event);
                break;
            case ConfigureRequest:
    //             prologue (&event, "ConfigureRequest");
    //             do_ConfigureRequest (&event);
                break;
            case GravityNotify:
    //             prologue (&event, "GravityNotify");
    //             do_GravityNotify (&event);
                break;
            case ResizeRequest:
    //             prologue (&event, "ResizeRequest");
    //             do_ResizeRequest (&event);
                break;
            case CirculateNotify:
    //             prologue (&event, "CirculateNotify");
    //             do_CirculateNotify (&event);
                break;
            case CirculateRequest:
    //             prologue (&event, "CirculateRequest");
    //             do_CirculateRequest (&event);
                break;
            case PropertyNotify:
    //             prologue (&event, "PropertyNotify");
    //             do_PropertyNotify (&event);
                break;
            case SelectionClear:
    //             prologue (&event, "SelectionClear");
    //             do_SelectionClear (&event);
                break;
            case SelectionRequest:
    //             prologue (&event, "SelectionRequest");
    //             do_SelectionRequest (&event);
                break;
            case SelectionNotify:
    //             prologue (&event, "SelectionNotify");
    //             do_SelectionNotify (&event);
                break;
            case ColormapNotify:
    //             prologue (&event, "ColormapNotify");
    //             do_ColormapNotify (&event);
                break;
            case ClientMessage:
                qDebug() << "ClientMessage";
    //             prologue (&event, "ClientMessage");
    //             do_ClientMessage (&event);
                break;
            case MappingNotify:
    //             qDebug() << "MappingNotify";
    //             do_MappingNotify (&event);
                break;
            default:
                qDebug() <<"Unknown event type" << event.type;
                break;
            }
        }

        //m_mutex.unlock();
    }
    qDebug() << "RUN END";
}

void BX11ControlThread::onDestroyNotify(XEvent* event)
{
    Q_ASSERT(host() == true);

    XDestroyWindowEvent *e = (XDestroyWindowEvent*)event;
    m_windowList.remove(e->window);
}

void BX11ControlThread::onMapNotify(XEvent* event)
{
    Q_ASSERT(host() == true);

    XMapEvent *e = reinterpret_cast<XMapEvent*>(event);
    //Window eventWindow = e->window;
    selectMotionMask(m_display, e->window);
}

void BX11ControlThread::onMotionNotify(XEvent* event)
{
    Q_ASSERT(host() == true);

    XMotionEvent *e = (XMotionEvent*)event;
    //e->x_root, e->y_root
    emit mouseMoved(e->x_root, e->y_root);
}

void BX11ControlThread::onKeyPress(XEvent* event)
{
    Q_ASSERT(host() == true);

    XKeyPressedEvent *e = (XKeyPressedEvent*)event;
    KeySym sym = XkbKeycodeToKeysym(m_display, e->keycode, 0, 0);
    qDebug() << sym << XKeysymToString(sym);
    qDebug() << "e->state" << e->state;
    qDebug() << "BX11KeyTranslator::xKeyCodeToQtModifier(e->state)" << BX11KeyTranslator::xKeyCodeToQtModifier(e->state);
    emit keyPressed(BX11KeyTranslator::xKeyCodeToQt(sym), BX11KeyTranslator::xKeyCodeToQtModifier(e->state));
}

void BX11ControlThread::onKeyRelease(XEvent* event)
{
    Q_ASSERT(host() == true);

    XKeyReleasedEvent *e = (XKeyReleasedEvent*)event;
//     e->state
    KeySym sym = XkbKeycodeToKeysym(m_display, e->keycode, 0, 0);
    qDebug() << sym << XKeysymToString(sym);
//     if (BX11KeyTranslator::isModifier(sym))
//     {
//         m_keyModifiers |= BX11KeyTranslator::xKeyCodeToQt(sym);
//     }
    qDebug() << "e->state" << e->state;
    qDebug() << "BX11KeyTranslator::xKeyCodeToQtModifier(e->state)" << BX11KeyTranslator::xKeyCodeToQtModifier(e->state);
    emit keyReleased(BX11KeyTranslator::xKeyCodeToQt(sym), BX11KeyTranslator::xKeyCodeToQtModifier(e->state));
}

void BX11ControlThread::onButtonPress(XEvent* event)
{
    Q_ASSERT(host() == true);

    XButtonEvent *e = (XButtonEvent*)event;
    emit mousePressed(e->button);
}

void BX11ControlThread::onButtonRelease(XEvent* event)
{
    Q_ASSERT(host() == true);
    XButtonEvent *e = (XButtonEvent*)event;
    emit mouseReleased(e->button);
}

void BX11ControlThread::updateNotifyMask()
{
    if (!host())
        return;

    m_mutex.lock();

    m_notifyMask = 0;
    if (m_trackSubstructureNotify)
        m_notifyMask |= SubstructureNotifyMask;


    XSelectInput(m_display, m_root, m_notifyMask);

    m_mutex.unlock();
}

void BX11ControlThread::selectMotionMask(Display* display, Window window)
{
    // we want to track the mouse everywhere on the display. to achieve
    // that we select PointerMotionMask on every window.  we also select
    // SubstructureNotifyMask in order to get CreateNotify events so we
    // select events on new windows too.
    //
    // note that this can break certain clients due a design flaw of X.
    // X will deliver a PointerMotion event to the deepest window in the
    // hierarchy that contains the pointer and has PointerMotionMask
    // selected by *any* client.  if another client doesn't select
    // motion events in a subwindow so the parent window will get them
    // then by selecting for motion events on the subwindow we break
    // that client because the parent will no longer get the events.

    if (m_windowList.contains(window))
    {
        qDebug() << "already in list";
        return;
    }

    // select events of interest.  do this before querying the tree so
    // we'll get notifications of children created after the XQueryTree()
    // so we won't miss them.
    qDebug() << "selecting pointer mask for window:" << window;
    XSelectInput(display, window, PointerMotionMask | m_notifyMask);
    m_windowList.insert(window);

    // recurse on child windows
    Window rw, pw, *cw;
    unsigned int nc;
    if (XQueryTree(display, window, &rw, &pw, &cw, &nc))
    {
        for (unsigned int i = 0; i < nc; ++i)
        {
            qDebug() << "childs: " << i;
            selectMotionMask(display, cw[i]);
        }
        XFree(cw);
    }
}

void BX11ControlThread::sendDummyEvent()
{
    qDebug() << "sendDummyEvent";
    int ret = 0;
    // Send a dummy event that XNextEvent does not block anymore
    // it is important as XGrabKeyboard and XSelectInput can't run
    // as XNextEvent is still blocking

    XEvent event;
    memset(&event, 0, sizeof(event));
    event.type = ClientMessage;
    ret = XSendEvent(m_display, m_root, false, 0, &event);

//     XClientMessageEvent event;
//     memset(&event, 0, sizeof(event));
//     event.type = ClientMessage;
//     event.display = m_display;
//     event.window = m_root;
//     event.send_event = True;
//     event.format = 32;
//     ret = XSendEvent(event.display, event.window, False, 0, (XEvent *)&event);
    qDebug() << "XSendEvent:" << ret << "Success:" << (int)(ret == Success)
             << " BadValue:" << (int)(ret == BadValue)
             << " BadWindow:" << (int)(ret == BadWindow)
             << " BadRequest:" << (int)(ret == BadRequest);

    ret = XFlush(m_display);
    qDebug() << "XFlush:" << ret;
    qDebug() << "sendDummyEvent END";
}

Cursor BX11ControlThread::createBlankCursor() const
{
    // this seems just a bit more complicated than really necessary

    // get the closet cursor size to 1x1
    unsigned int w, h;
    XQueryBestCursor(m_display, m_root, 1, 1, &w, &h);

    // make bitmap data for cursor of closet size.  since the cursor
    // is blank we can use the same bitmap for shape and mask:  all
    // zeros.
    const int size = ((w + 7) >> 3) * h;
    char* const data = new char[size];
    memset(data, 0, size);

    // make bitmap
    Pixmap bitmap = XCreateBitmapFromData(m_display, m_root, data, w, h);

    // need an arbitrary color for the cursor
    XColor color;
    memset(&color, 0, sizeof(color));
    color.pixel = 0;
    color.red   = color.green = color.blue = 0;
    color.flags = DoRed | DoGreen | DoBlue;

    // make cursor from bitmap
    Cursor cursor = XCreatePixmapCursor(m_display, bitmap, bitmap, &color, &color, 0, 0);

    // don't need bitmap or the data anymore
    delete[] data;
    XFreePixmap(m_display, bitmap);

    return cursor;
}

void BX11ControlThread::moveMouseToDirection(BScreenConfigGlobal::ScreenLayout direction)
{
    Window root, child;
    int rootX, rootY, winX, winY;
    int destX, destY;
    unsigned int mask;

    qDebug() << "BX11ControlThread::moveMouseToDirection::XFlush";
    XFlush(m_display);

    XEvent event;
    qDebug() << "BX11ControlThread::moveMouseToDirection::XCheckWindowEvent";
    //XNextEvent(m_display, &event);
    bool gotEvents;
    do
    {
        gotEvents = XCheckWindowEvent(m_display, m_root, 0xffffff, &event);
        qDebug() << gotEvents;
        //XNextEvent(m_display, &event);
    } while(gotEvents);

    qDebug() << "BX11ControlThread::moveMouseToDirection::XQueryPointer";
    bool retPointer = XQueryPointer(m_display, m_root, &root, &child, &rootX, &rootY, &winX, &winY, &mask);
    if (retPointer == False)
    {
        qDebug() << "query pointer failed, hm.. what now? bail out for now..";
        return;
    }

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
        default:
        {
            qDebug() << "unknown direction, bail out!";
            return;
        }
    }

    qDebug() << "BX11ControlThread::moveMouseToDirection::XWarpPointer";
    XWarpPointer(m_display, m_root, m_root, 0, 0, 0, 0, destX, destY);
    XFlush(m_display);
}

#include "moc_bx11controlthread.cpp"
