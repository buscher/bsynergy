/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2010  Bernd Buschinski <b.buschinski@web.de>

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


#ifndef BSCREENLAYOUTCONFIG_H
#define BSCREENLAYOUTCONFIG_H

#include <QDialog>

#include "base/bscreenconfigglobal.h"


class QWidget;
class QPushButton;
class QListWidgetItem;
class BScreenLayoutWidget;
class BScreenListWidget;

class BScreenLayoutConfig : public QDialog
{
    Q_OBJECT
public:
    explicit BScreenLayoutConfig(QWidget* parent = nullptr);

    BScreenConfigGlobal::ScreenLayoutMap rules(QString* errorMsg = nullptr);

protected slots:
    void newScreen();
    void deleteScreen();
    void currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);

private:
    BScreenListWidget* m_screenListWidget;
    BScreenLayoutWidget* m_screenLayoutWidget;
    QPushButton* m_newButton;
    QPushButton* m_deleteButton;
    QListWidgetItem* m_deletedItem;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
};

#endif // BSCREENLAYOUTCONFIG_H
