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


#ifndef BSCREENLISTWIDGET_H
#define BSCREENLISTWIDGET_H

#include <QListWidget>

class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;

class BScreenListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit BScreenListWidget(QWidget *parent = nullptr);
    void addScreen(const QString& name);

    QStringList names();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void startDrag(Qt::DropActions supportedActions);
};

#endif // BSCREENLISTWIDGET_H
