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


#include "bmswindowskeytranslator.h"

#include <Qt>
#include <QDebug>

#include <windows.h>
#include <winuser.h>

struct TransKey
{
    int keyCodeQt;
    unsigned int keyCodeMS;
};

// These are the X equivalents to the Qt keycodes 0x1000 - 0x1026
static const TransKey g_rgQtToMSKeyCode[] =
{
    {Qt::Key_0, 48},
    {Qt::Key_1, 49},
    {Qt::Key_2, 50},
    {Qt::Key_3, 51},
    {Qt::Key_4, 52},
    {Qt::Key_5, 53},
    {Qt::Key_6, 54},
    {Qt::Key_7, 55},
    {Qt::Key_8, 56},
    {Qt::Key_9, 57},
    {Qt::Key_Colon, 188},
    {Qt::Key_Semicolon, VK_OEM_1},
    {Qt::Key_Less, 226},
//    {Qt::Key_Equal,  XK_equal},
//    {Qt::Key_Greater, XK_greater},
//    {Qt::Key_Question, 219},
//    {Qt::Key_At, XK_at},
    {Qt::Key_A, 65},
    {Qt::Key_B, 66},
    {Qt::Key_C, 67},
    {Qt::Key_D, 68},
    {Qt::Key_E, 69},
    {Qt::Key_F, 70},
    {Qt::Key_G, 71},
    {Qt::Key_H, 72},
    {Qt::Key_I, 73},
    {Qt::Key_J, 74},
    {Qt::Key_K, 75},
    {Qt::Key_L, 76},
    {Qt::Key_M, 77},
    {Qt::Key_N, 78},
    {Qt::Key_O, 79},
    {Qt::Key_P, 80},
    {Qt::Key_Q, 81},
    {Qt::Key_R, 82},
    {Qt::Key_S, 83},
    {Qt::Key_T, 84},
    {Qt::Key_U, 85},
    {Qt::Key_V, 86},
    {Qt::Key_W, 87},
    {Qt::Key_X, 88},
    {Qt::Key_Y, 89},
    {Qt::Key_Z, 90},

//    {Qt::Key_Adiaeresis, XK_Adiaeresis},
//    {Qt::Key_Udiaeresis, XK_Udiaeresis},
//    {Qt::Key_Odiaeresis, XK_Odiaeresis},
    {Qt::Key_ssharp, 219},

//    {Qt::Key_acute, XK_aacute},
//    {Qt::Key_AsciiCircum, XK_asciicircum},

    {Qt::Key_Escape,     VK_ESCAPE },
    {Qt::Key_Tab,        VK_TAB },
//    {Qt::Key_Backtab,    XK_ISO_Left_Tab },
    {Qt::Key_Backspace,  VK_BACK },
    {Qt::Key_Return,     VK_RETURN },
    {Qt::Key_Enter,      VK_RETURN },
    {Qt::Key_Insert,     VK_INSERT },
    {Qt::Key_Delete,     VK_DELETE },
    {Qt::Key_Pause,      VK_PAUSE },
    {Qt::Key_Print,      VK_PRINT },
//    {Qt::Key_SysReq,     XK_Sys_Req },
    {Qt::Key_Home,       VK_HOME },
    {Qt::Key_End,        VK_END },
    {Qt::Key_Left,       VK_LEFT },
    {Qt::Key_Up,         VK_UP },
    {Qt::Key_Right,      VK_RIGHT },
    {Qt::Key_Down,       VK_DOWN },
    {Qt::Key_PageUp,     VK_PRIOR },
    {Qt::Key_PageDown,   VK_NEXT },
//    //meta keys
    {Qt::Key_Shift,      VK_SHIFT },
    {Qt::Key_Shift,      VK_LSHIFT },
    {Qt::Key_Shift,      VK_RSHIFT },
    {Qt::Key_Control,    VK_CONTROL },
    {Qt::Key_Control,    VK_LCONTROL },
    {Qt::Key_Control,    VK_RCONTROL },
    {Qt::Key_Meta,       VK_LWIN },
    {Qt::Key_Meta,       VK_RWIN },
    {Qt::Key_Alt,        VK_MENU },

    {Qt::Key_CapsLock,   VK_CAPITAL },
    {Qt::Key_NumLock,    VK_NUMLOCK },
    {Qt::Key_ScrollLock, VK_SCROLL },
    {Qt::Key_F1,         VK_F1  },
    {Qt::Key_F2,         VK_F2  },
    {Qt::Key_F3,         VK_F3  },
    {Qt::Key_F4,         VK_F4  },
    {Qt::Key_F5,         VK_F5  },
    {Qt::Key_F6,         VK_F6  },
    {Qt::Key_F7,         VK_F7  },
    {Qt::Key_F8,         VK_F8  },
    {Qt::Key_F9,         VK_F9  },
    {Qt::Key_F10,        VK_F10 },
    {Qt::Key_F11,        VK_F11 },
    {Qt::Key_F12,        VK_F12 },
    {Qt::Key_F13,        VK_F13 },
    {Qt::Key_F14,        VK_F14 },
    {Qt::Key_F15,        VK_F15 },
    {Qt::Key_F16,        VK_F16 },
    {Qt::Key_F17,        VK_F17 },
    {Qt::Key_F18,        VK_F18 },
    {Qt::Key_F19,        VK_F19 },
    {Qt::Key_F20,        VK_F20 },
    {Qt::Key_F21,        VK_F21 },
    {Qt::Key_F22,        VK_F22 },
    {Qt::Key_F23,        VK_F23 },
    {Qt::Key_F24,        VK_F24 },
//    {Qt::Key_F25,        XK_F25 },
//    {Qt::Key_F26,        XK_F26 },
//    {Qt::Key_F27,        XK_F27 },
//    {Qt::Key_F28,        XK_F28 },
//    {Qt::Key_F29,        XK_F29 },
//    {Qt::Key_F30,        XK_F30 },
//    {Qt::Key_F31,        XK_F31 },
//    {Qt::Key_F32,        XK_F32 },
//    {Qt::Key_F33,        XK_F33 },
//    {Qt::Key_F34,        XK_F34 },
//    {Qt::Key_F35,        XK_F35 },
//    {Qt::Key_Super_L,    XK_Super_L },
//    {Qt::Key_Super_R,    XK_Super_R },
    {Qt::Key_Menu,       VK_LMENU },
    {Qt::Key_Menu,       VK_RMENU },
//    {Qt::Key_Hyper_L,    XK_Hyper_L },
//    {Qt::Key_Hyper_R,    XK_Hyper_R },
    {Qt::Key_Help,       VK_HELP },
//    //{Qt::Key_Direction_L, XK_Direction_L }, These keys don't exist in X11
//    //{Qt::Key_Direction_R, XK_Direction_R },

    {'/',                VK_DIVIDE },
    {'*',                VK_MULTIPLY },
    {'-',                VK_SUBTRACT },
    {'+',                VK_ADD },
//    {Qt::Key_Return,     XK_KP_Enter },
    {Qt::Key_Delete,     VK_DELETE },

//// the next lines are taken from XFree > 4.0 (X11/XF86keysyms.h), defining some special
//// multimedia keys. They are included here as not every system has them.
//#define XF86XK_Standby          0x1008FF10
//#define XF86XK_AudioLowerVolume 0x1008FF11
//#define XF86XK_AudioMute        0x1008FF12
//#define XF86XK_AudioRaiseVolume 0x1008FF13
//#define XF86XK_AudioPlay        0x1008FF14
//#define XF86XK_AudioStop        0x1008FF15
//#define XF86XK_AudioPrev        0x1008FF16
//#define XF86XK_AudioNext        0x1008FF17
//#define XF86XK_HomePage         0x1008FF18
//#define XF86XK_Calculator       0x1008FF1D
//#define XF86XK_Mail             0x1008FF19
//#define XF86XK_Start            0x1008FF1A
//#define XF86XK_Search           0x1008FF1B
//#define XF86XK_AudioRecord      0x1008FF1C
//#define XF86XK_Back             0x1008FF26
//#define XF86XK_Forward          0x1008FF27
//#define XF86XK_Stop             0x1008FF28
//#define XF86XK_Refresh          0x1008FF29
//#define XF86XK_Favorites        0x1008FF30
//#define XF86XK_AudioPause       0x1008FF31
//#define XF86XK_AudioMedia       0x1008FF32
//#define XF86XK_MyComputer       0x1008FF33
//#define XF86XK_OpenURL          0x1008FF38
//#define XF86XK_Launch0          0x1008FF40
//#define XF86XK_Launch1          0x1008FF41
//#define XF86XK_Launch2          0x1008FF42
//#define XF86XK_Launch3          0x1008FF43
//#define XF86XK_Launch4          0x1008FF44
//#define XF86XK_Launch5          0x1008FF45
//#define XF86XK_Launch6          0x1008FF46
//#define XF86XK_Launch7          0x1008FF47
//#define XF86XK_Launch8          0x1008FF48
//#define XF86XK_Launch9          0x1008FF49
//#define XF86XK_LaunchA          0x1008FF4A
//#define XF86XK_LaunchB          0x1008FF4B
//#define XF86XK_LaunchC          0x1008FF4C
//#define XF86XK_LaunchD          0x1008FF4D
//#define XF86XK_LaunchE          0x1008FF4E
//#define XF86XK_LaunchF          0x1008FF4F
//// end of XF86keysyms.h

//    {Qt::Key_Standby,    XF86XK_Standby },
//    {Qt::Key_VolumeDown, XF86XK_AudioLowerVolume },
//    {Qt::Key_VolumeMute, XF86XK_AudioMute },
//    {Qt::Key_VolumeUp,   XF86XK_AudioRaiseVolume },
//    {Qt::Key_MediaPlay,  XF86XK_AudioPlay },
//    {Qt::Key_MediaStop,  XF86XK_AudioStop },
//    {Qt::Key_MediaPrevious,  XF86XK_AudioPrev },
//    {Qt::Key_MediaNext,  XF86XK_AudioNext },
//    {Qt::Key_HomePage,   XF86XK_HomePage },
//    {Qt::Key_LaunchMail, XF86XK_Mail },
//    {Qt::Key_Search,     XF86XK_Search },
//    {Qt::Key_MediaRecord, XF86XK_AudioRecord },
//    {Qt::Key_LaunchMedia, XF86XK_AudioMedia },
//    {Qt::Key_Launch1,    XF86XK_Calculator },
//    {Qt::Key_Back,       XF86XK_Back },
//    {Qt::Key_Forward,    XF86XK_Forward },
//    {Qt::Key_Stop,       XF86XK_Stop },
//    {Qt::Key_Refresh,    XF86XK_Refresh },
//    {Qt::Key_Favorites,  XF86XK_Favorites },
//    {Qt::Key_Launch0,    XF86XK_MyComputer },
//    {Qt::Key_OpenUrl,    XF86XK_OpenURL },
//    {Qt::Key_Launch2,    XF86XK_Launch0 },
//    {Qt::Key_Launch3,    XF86XK_Launch1 },
//    {Qt::Key_Launch4,    XF86XK_Launch2 },
//    {Qt::Key_Launch5,    XF86XK_Launch3 },
//    {Qt::Key_Launch6,    XF86XK_Launch4 },
//    {Qt::Key_Launch7,    XF86XK_Launch5 },
//    {Qt::Key_Launch8,    XF86XK_Launch6 },
//    {Qt::Key_Launch9,    XF86XK_Launch7 },
//    {Qt::Key_LaunchA,    XF86XK_Launch8 },
//    {Qt::Key_LaunchB,    XF86XK_Launch9 },
//    {Qt::Key_LaunchC,    XF86XK_LaunchA },
//    {Qt::Key_LaunchD,    XF86XK_LaunchB },
//    {Qt::Key_LaunchE,    XF86XK_LaunchC },
//    {Qt::Key_LaunchF,    XF86XK_LaunchD },
    {0, 0}
};

int BMSWindowsKeyTranslator::msKeyCodeToQt(unsigned int msKeyCode)
{
    int i = 0;
    TransKey trans;
    do {
        trans = g_rgQtToMSKeyCode[i];
        if (trans.keyCodeMS == msKeyCode)
        {
            return trans.keyCodeQt;
        }
        ++i;
    } while (trans.keyCodeMS != 0 && trans.keyCodeQt != 0);
    return -1;
}

unsigned int BMSWindowsKeyTranslator::qtKeyCodeToMSKeyCode(int qtKeyCode)
{
    int i = 0;
    TransKey trans;
    do {
        trans = g_rgQtToMSKeyCode[i];
        if (trans.keyCodeQt == qtKeyCode)
        {
            return trans.keyCodeMS;
        }
        ++i;
    } while (trans.keyCodeMS != 0 && trans.keyCodeQt != 0);
    return -1;
}

int BMSWindowsKeyTranslator::msKeyCodeToQtModifier(int msKeyCode)
{
    int ret = 0;
    if (GetKeyState(VK_SHIFT))
    {
        qDebug() << "shift";
        //Qt::ShiftModifier   0x02000000  A Shift key on the keyboard is pressed.
        ret |= Qt::ShiftModifier;
    }
//    if (xKeySym & Mod2Mask)
//    {
//        qDebug() << "keypad";
//        //Qt::KeypadModifier  0x20000000  A keypad button is pressed.
//        ret |= Qt::KeypadModifier;
//    }
    if (0 && GetKeyState(VK_CONTROL))
    {
        qDebug() << "ctrl";
        //Qt::ControlModifier 0x04000000  A Ctrl key on the keyboard is pressed.
        ret |= Qt::ControlModifier;
    }
    if (GetKeyState(VK_MENU))
    {
        qDebug() << "alt";
        //Qt::AltModifier 0x08000000  An Alt key on the keyboard is pressed.
        ret |= Qt::AltModifier;
    }
    if (GetKeyState(VK_LWIN) || GetKeyState(VK_RWIN))
    {
        qDebug() << "meta";
        //Qt::MetaModifier    0x10000000  A Meta key on the keyboard is pressed.
        ret |= Qt::MetaModifier;
    }
    if (GetKeyState(VK_MENU) && GetKeyState(VK_CONTROL))
    {
        qDebug() << "iso_shift_level3";
        //Qt::GroupSwitchModifier 0x40000000  X11 only. A Mode_switch key on the keyboard is pressed.
        ret |= Qt::GroupSwitchModifier;
    }
    return ret;
}

int BMSWindowsKeyTranslator::qtModifierToMSKeyState(int qtModifiers)
{
    int ret = 0;
//    if (qtModifiers & Qt::ShiftModifier)
//    {
//        ret |= ShiftMask;
//    }
//    if (qtModifiers & Qt::KeypadModifier)
//    {
//        ret |= Mod2Mask;
//    }
//    if (qtModifiers & Qt::ControlModifier)
//    {
//        ret |= ControlMask;
//    }
//    if (qtModifiers & Qt::AltModifier)
//    {
//        ret |= Mod1Mask;
//    }
//    if (qtModifiers & Qt::MetaModifier)
//    {
//        ret |= Mod4Mask;
//    }
//    if (qtModifiers & Qt::GroupSwitchModifier)
//    {
//        ret |= 0xc0;
//    }
    return ret;
}
