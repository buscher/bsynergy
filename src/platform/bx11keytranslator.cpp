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


#include "bx11keytranslator.h"

#include <Qt>
#include <QDebug>

#define XK_LATIN1
#define XK_MISCELLANY
#define XK_XKB_KEYS
#include <X11/keysymdef.h>
#include <X11/X.h>

// uint accelModMaskX()
// {
// return modXShift() | modXCtrl() | modXAlt() | modXMeta();
// }
// 
// bool xEventToQt( XEvent* e, int* keyQt )
// {
// Q_ASSERT(e->type == KeyPress || e->type == KeyRelease);
// 
// uchar keyCodeX = e->xkey.keycode;
// uint keyModX = e->xkey.state & (accelModMaskX() | MODE_SWITCH);
// 
// KeySym keySym;
// char buffer[16];
// XLookupString( (XKeyEvent*) e, buffer, 15, &keySym, 0 );
// uint keySymX = (uint)keySym;
// 
// // If numlock is active and a keypad key is pressed, XOR the SHIFT state.
// //  e.g., KP_4 => Shift+KP_Left, and Shift+KP_4 => KP_Left.
// if( e->xkey.state & modXNumLock() ) {
// uint sym = XKeycodeToKeysym( QX11Info::display(), keyCodeX, 0 );
// // TODO: what's the xor operator in c++?
// // If this is a keypad key,
// if( sym >= XK_KP_Space && sym <= XK_KP_9 ) {
// switch( sym ) {
// // Leave the following keys unaltered
// // FIXME: The proper solution is to see which keysyms don't change when shifted.
// case XK_KP_Multiply:
// case XK_KP_Add:
// case XK_KP_Subtract:
// case XK_KP_Divide:
// break;
// default:
// if( keyModX & modXShift() )
// keyModX &= ~modXShift();
// else
// keyModX |= modXShift();
// }
// }
// }
// 
// int keyCodeQt;
// int keyModQt;
// symXToKeyQt(keySymX, &keyCodeQt);
// modXToQt(keyModX, &keyModQt);
// 
// *keyQt = keyCodeQt | keyModQt;
// return true;
// }

struct TransKey
{
    int keySymQt;
    unsigned int keySymX;
};

// These are the X equivalents to the Qt keycodes 0x1000 - 0x1026
static const TransKey g_rgQtToSymX[] =
{
    {Qt::Key_0, XK_0},
    {Qt::Key_1, XK_1},
    {Qt::Key_2, XK_2},
    {Qt::Key_3, XK_3},
    {Qt::Key_4, XK_4},
    {Qt::Key_5, XK_5},
    {Qt::Key_6, XK_6},
    {Qt::Key_7, XK_7},
    {Qt::Key_8, XK_8},
    {Qt::Key_9, XK_9},
    {Qt::Key_Colon, XK_colon},
    {Qt::Key_Semicolon, XK_semicolon},
    {Qt::Key_Less, XK_less},
    {Qt::Key_Equal,  XK_equal},
    {Qt::Key_Greater, XK_greater},
    {Qt::Key_Question, XK_question},
    {Qt::Key_At, XK_at},
    {Qt::Key_A, XK_a},
    {Qt::Key_B, XK_b},
    {Qt::Key_C, XK_c},
    {Qt::Key_D, XK_d},
    {Qt::Key_E, XK_e},
    {Qt::Key_F, XK_f},
    {Qt::Key_G, XK_g},
    {Qt::Key_H, XK_h},
    {Qt::Key_I, XK_i},
    {Qt::Key_J, XK_j},
    {Qt::Key_K, XK_k},
    {Qt::Key_L, XK_l},
    {Qt::Key_M, XK_m},
    {Qt::Key_N, XK_n},
    {Qt::Key_O, XK_o},
    {Qt::Key_P, XK_p},
    {Qt::Key_Q, XK_q},
    {Qt::Key_R, XK_r},
    {Qt::Key_S, XK_s},
    {Qt::Key_T, XK_t},
    {Qt::Key_U, XK_u},
    {Qt::Key_V, XK_v},
    {Qt::Key_W, XK_w},
    {Qt::Key_X, XK_x},
    {Qt::Key_Y, XK_y},
    {Qt::Key_Z, XK_z},

    {Qt::Key_Adiaeresis, XK_Adiaeresis},
    {Qt::Key_Udiaeresis, XK_Udiaeresis},
    {Qt::Key_Odiaeresis, XK_Odiaeresis},
    {Qt::Key_ssharp, XK_ssharp},

    {Qt::Key_acute, XK_aacute},
    {Qt::Key_AsciiCircum, XK_asciicircum},

    {Qt::Key_Escape,     XK_Escape },
    {Qt::Key_Tab,        XK_Tab },
    {Qt::Key_Backtab,    XK_ISO_Left_Tab },
    {Qt::Key_Backspace,  XK_BackSpace },
    {Qt::Key_Return,     XK_Return },
    {Qt::Key_Enter,      XK_KP_Enter },
    {Qt::Key_Insert,     XK_Insert },
    {Qt::Key_Delete,     XK_Delete },
    {Qt::Key_Pause,      XK_Pause },
    {Qt::Key_Print,      XK_Print },
    {Qt::Key_SysReq,     XK_Sys_Req },
    {Qt::Key_Home,       XK_Home },
    {Qt::Key_End,        XK_End },
    {Qt::Key_Left,       XK_Left },
    {Qt::Key_Up,         XK_Up },
    {Qt::Key_Right,      XK_Right },
    {Qt::Key_Down,       XK_Down },
    {Qt::Key_PageUp,     XK_Prior },
    {Qt::Key_PageDown,   XK_Next },
    //meta keys
    {Qt::Key_Shift,      XK_Shift_L },
    {Qt::Key_Shift,      XK_Shift_R },
    {Qt::Key_Control,    XK_Control_L },
    {Qt::Key_Control,    XK_Control_R },
    {Qt::Key_Meta,       XK_Meta_L },
    {Qt::Key_Meta,       XK_Meta_R },
    {Qt::Key_Alt,        XK_Alt_L },
    {Qt::Key_Alt,        XK_Alt_R },

    {Qt::Key_CapsLock,   XK_Caps_Lock },
    {Qt::Key_NumLock,    XK_Num_Lock },
    {Qt::Key_ScrollLock, XK_Scroll_Lock },
    {Qt::Key_F1,         XK_F1 },
    {Qt::Key_F2,         XK_F2 },
    {Qt::Key_F3,         XK_F3 },
    {Qt::Key_F4,         XK_F4 },
    {Qt::Key_F5,         XK_F5 },
    {Qt::Key_F6,         XK_F6 },
    {Qt::Key_F7,         XK_F7 },
    {Qt::Key_F8,         XK_F8 },
    {Qt::Key_F9,         XK_F9 },
    {Qt::Key_F10,        XK_F10 },
    {Qt::Key_F11,        XK_F11 },
    {Qt::Key_F12,        XK_F12 },
    {Qt::Key_F13,        XK_F13 },
    {Qt::Key_F14,        XK_F14 },
    {Qt::Key_F15,        XK_F15 },
    {Qt::Key_F16,        XK_F16 },
    {Qt::Key_F17,        XK_F17 },
    {Qt::Key_F18,        XK_F18 },
    {Qt::Key_F19,        XK_F19 },
    {Qt::Key_F20,        XK_F20 },
    {Qt::Key_F21,        XK_F21 },
    {Qt::Key_F22,        XK_F22 },
    {Qt::Key_F23,        XK_F23 },
    {Qt::Key_F24,        XK_F24 },
    {Qt::Key_F25,        XK_F25 },
    {Qt::Key_F26,        XK_F26 },
    {Qt::Key_F27,        XK_F27 },
    {Qt::Key_F28,        XK_F28 },
    {Qt::Key_F29,        XK_F29 },
    {Qt::Key_F30,        XK_F30 },
    {Qt::Key_F31,        XK_F31 },
    {Qt::Key_F32,        XK_F32 },
    {Qt::Key_F33,        XK_F33 },
    {Qt::Key_F34,        XK_F34 },
    {Qt::Key_F35,        XK_F35 },
    {Qt::Key_Super_L,    XK_Super_L },
    {Qt::Key_Super_R,    XK_Super_R },
    {Qt::Key_Menu,       XK_Menu },
    {Qt::Key_Hyper_L,    XK_Hyper_L },
    {Qt::Key_Hyper_R,    XK_Hyper_R },
    {Qt::Key_Help,       XK_Help },
    //{Qt::Key_Direction_L, XK_Direction_L }, These keys don't exist in X11
    //{Qt::Key_Direction_R, XK_Direction_R },

    {'/',                XK_KP_Divide },
    {'*',                XK_KP_Multiply },
    {'-',                XK_KP_Subtract },
    {'+',                XK_KP_Add },
    {Qt::Key_Return,     XK_KP_Enter },
    {Qt::Key_Delete,     XK_KP_Delete },

// the next lines are taken from XFree > 4.0 (X11/XF86keysyms.h), defining some special
// multimedia keys. They are included here as not every system has them.
#define XF86XK_Standby          0x1008FF10
#define XF86XK_AudioLowerVolume 0x1008FF11
#define XF86XK_AudioMute        0x1008FF12
#define XF86XK_AudioRaiseVolume 0x1008FF13
#define XF86XK_AudioPlay        0x1008FF14
#define XF86XK_AudioStop        0x1008FF15
#define XF86XK_AudioPrev        0x1008FF16
#define XF86XK_AudioNext        0x1008FF17
#define XF86XK_HomePage         0x1008FF18
#define XF86XK_Calculator       0x1008FF1D
#define XF86XK_Mail             0x1008FF19
#define XF86XK_Start            0x1008FF1A
#define XF86XK_Search           0x1008FF1B
#define XF86XK_AudioRecord      0x1008FF1C
#define XF86XK_Back             0x1008FF26
#define XF86XK_Forward          0x1008FF27
#define XF86XK_Stop             0x1008FF28
#define XF86XK_Refresh          0x1008FF29
#define XF86XK_Favorites        0x1008FF30
#define XF86XK_AudioPause       0x1008FF31
#define XF86XK_AudioMedia       0x1008FF32
#define XF86XK_MyComputer       0x1008FF33
#define XF86XK_OpenURL          0x1008FF38
#define XF86XK_Launch0          0x1008FF40
#define XF86XK_Launch1          0x1008FF41
#define XF86XK_Launch2          0x1008FF42
#define XF86XK_Launch3          0x1008FF43
#define XF86XK_Launch4          0x1008FF44
#define XF86XK_Launch5          0x1008FF45
#define XF86XK_Launch6          0x1008FF46
#define XF86XK_Launch7          0x1008FF47
#define XF86XK_Launch8          0x1008FF48
#define XF86XK_Launch9          0x1008FF49
#define XF86XK_LaunchA          0x1008FF4A
#define XF86XK_LaunchB          0x1008FF4B
#define XF86XK_LaunchC          0x1008FF4C
#define XF86XK_LaunchD          0x1008FF4D
#define XF86XK_LaunchE          0x1008FF4E
#define XF86XK_LaunchF          0x1008FF4F
// end of XF86keysyms.h

    {Qt::Key_Standby,    XF86XK_Standby },
    {Qt::Key_VolumeDown, XF86XK_AudioLowerVolume },
    {Qt::Key_VolumeMute, XF86XK_AudioMute },
    {Qt::Key_VolumeUp,   XF86XK_AudioRaiseVolume },
    {Qt::Key_MediaPlay,  XF86XK_AudioPlay },
    {Qt::Key_MediaStop,  XF86XK_AudioStop },
    {Qt::Key_MediaPrevious,  XF86XK_AudioPrev },
    {Qt::Key_MediaNext,  XF86XK_AudioNext },
    {Qt::Key_HomePage,   XF86XK_HomePage },
    {Qt::Key_LaunchMail, XF86XK_Mail },
    {Qt::Key_Search,     XF86XK_Search },
    {Qt::Key_MediaRecord, XF86XK_AudioRecord },
    {Qt::Key_LaunchMedia, XF86XK_AudioMedia },
    {Qt::Key_Launch1,    XF86XK_Calculator },
    {Qt::Key_Back,       XF86XK_Back },
    {Qt::Key_Forward,    XF86XK_Forward },
    {Qt::Key_Stop,       XF86XK_Stop },
    {Qt::Key_Refresh,    XF86XK_Refresh },
    {Qt::Key_Favorites,  XF86XK_Favorites },
    {Qt::Key_Launch0,    XF86XK_MyComputer },
    {Qt::Key_OpenUrl,    XF86XK_OpenURL },
    {Qt::Key_Launch2,    XF86XK_Launch0 },
    {Qt::Key_Launch3,    XF86XK_Launch1 },
    {Qt::Key_Launch4,    XF86XK_Launch2 },
    {Qt::Key_Launch5,    XF86XK_Launch3 },
    {Qt::Key_Launch6,    XF86XK_Launch4 },
    {Qt::Key_Launch7,    XF86XK_Launch5 },
    {Qt::Key_Launch8,    XF86XK_Launch6 },
    {Qt::Key_Launch9,    XF86XK_Launch7 },
    {Qt::Key_LaunchA,    XF86XK_Launch8 },
    {Qt::Key_LaunchB,    XF86XK_Launch9 },
    {Qt::Key_LaunchC,    XF86XK_LaunchA },
    {Qt::Key_LaunchD,    XF86XK_LaunchB },
    {Qt::Key_LaunchE,    XF86XK_LaunchC },
    {Qt::Key_LaunchF,    XF86XK_LaunchD },
    {0, 0}
};

int BX11KeyTranslator::xKeyCodeToQt(unsigned int xKeyCode)
{
    int i = 0;
    TransKey trans;
    do {
        trans = g_rgQtToSymX[i];
        if (trans.keySymX == xKeyCode)
        {
            return trans.keySymQt;
        }
        ++i;
    } while (trans.keySymQt != 0 && trans.keySymX != 0);
    return -1;
}

unsigned int BX11KeyTranslator::qtKeyCodeToKeySym(int qtKeyCode)
{
    int i = 0;
    TransKey trans;
    do {
        trans = g_rgQtToSymX[i];
        if (trans.keySymQt == qtKeyCode)
        {
            return trans.keySymX;
        }
        ++i;
    } while (trans.keySymQt != 0 && trans.keySymX != 0);
    return -1;
}

int BX11KeyTranslator::xKeyCodeToQtModifier(int xKeySym)
{
    int ret = 0;
    if (xKeySym & ShiftMask)
    {
        qDebug() << "shift";
        //Qt::ShiftModifier   0x02000000  A Shift key on the keyboard is pressed.
        ret |= Qt::ShiftModifier;
    }
    if (xKeySym & Mod2Mask)
    {
        qDebug() << "keypad";
        //Qt::KeypadModifier  0x20000000  A keypad button is pressed.
        ret |= Qt::KeypadModifier;
    }
    if (xKeySym & ControlMask)
    {
        qDebug() << "ctrl";
        //Qt::ControlModifier 0x04000000  A Ctrl key on the keyboard is pressed.
        ret |= Qt::ControlModifier;
    }
    if (xKeySym & Mod1Mask)
    {
        qDebug() << "alt";
        //Qt::AltModifier 0x08000000  An Alt key on the keyboard is pressed.
        ret |= Qt::AltModifier;
    }
    if (xKeySym & Mod4Mask)
    {
        qDebug() << "meta";
        //Qt::MetaModifier    0x10000000  A Meta key on the keyboard is pressed.
        ret |= Qt::MetaModifier;
    }
    if (xKeySym & 0xc0)
    {
        qDebug() << "iso_shift_level3";
        //Qt::GroupSwitchModifier 0x40000000  X11 only. A Mode_switch key on the keyboard is pressed.
        ret |= Qt::GroupSwitchModifier;
    }
    return ret;
// Qt::NoModifier  0x00000000  No modifier key is pressed.
}

int BX11KeyTranslator::qtModifierToXKeyState(int qtModifiers)
{
    int ret = 0;
    if (qtModifiers & Qt::ShiftModifier)
    {
        ret |= ShiftMask;
    }
    if (qtModifiers & Qt::KeypadModifier)
    {
        ret |= Mod2Mask;
    }
    if (qtModifiers & Qt::ControlModifier)
    {
        ret |= ControlMask;
    }
    if (qtModifiers & Qt::AltModifier)
    {
        ret |= Mod1Mask;
    }
    if (qtModifiers & Qt::MetaModifier)
    {
        ret |= Mod4Mask;
    }
    if (qtModifiers & Qt::GroupSwitchModifier)
    {
        ret |= 0xc0;
    }
    return ret;
}
