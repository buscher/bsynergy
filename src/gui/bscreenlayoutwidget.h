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


#ifndef BSCREENLAYOUTWIDGET_H
#define BSCREENLAYOUTWIDGET_H

#include "base/bscreenconfigglobal.h"

#include <QWidget>
#include <QPoint>
#include <QVector>

struct ScreenStruct
{
    int x;
    int y;
    QString name;
};

class BScreenLayoutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BScreenLayoutWidget(QWidget* parent = nullptr);

    BScreenConfigGlobal::ScreenLayoutMap writeRules(QString *errorMessage = nullptr);

    QStringList names();

protected:
    virtual void paintEvent(QPaintEvent* );
    virtual void dragEnterEvent(QDragEnterEvent* );
    virtual void dragMoveEvent(QDragMoveEvent* );
    virtual void dragLeaveEvent(QDragLeaveEvent* );
    virtual void dropEvent(QDropEvent* );
    virtual void resizeEvent(QResizeEvent* );
    virtual void mousePressEvent(QMouseEvent*);

private:
    inline int findPiece(const QPoint& piecePoint) const;
    inline int findScreenByIndex(int x, int y) const;
    inline const QRect targetSquare(int x, int y) const;
    QVector< QRect > findDropables(int x, int y);
    inline bool areaIsFree(int x, int y);
    inline bool validArea(int x, int y);
    inline bool positionInDropArea(const QPoint& pos);
    void updateDropAble();
    inline void addRule(BScreenConfigGlobal::ScreenLayoutMap* rules, const QString& computer, const QString& computerTo, BScreenConfigGlobal::ScreenLayout direction);

    QVector<QRect> m_dropAreas;
    QVector<ScreenStruct> m_computerList;
    QRect m_highLightRect;
    bool m_dragMode;
};

#endif // BSCREENLAYOUTWIDGET_H
