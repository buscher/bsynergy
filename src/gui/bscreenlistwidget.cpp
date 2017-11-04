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


#include "bscreenlistwidget.h"

#include <QDebug>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QVariant>

static const QLatin1String Mimetype = QLatin1String("image/x-screen-computer");

BScreenListWidget::BScreenListWidget(QWidget* parent)
    : QListWidget(parent)
{
     setDragEnabled(true);
     setViewMode(QListView::ListMode);
     setIconSize(QSize(140,80));
     setSpacing(10);
     setAcceptDrops(true);
     setDropIndicatorShown(true);
}

void BScreenListWidget::addScreen(const QString& name)
{
    QPixmap pixmap(140, 80);
    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::green);
    painter.drawRect(pixmap.rect().adjusted(0, 0, -1, -1));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, name);
    painter.end();

    QListWidgetItem * const item = new QListWidgetItem(this);
    item->setIcon(pixmap);
    item->setData(Qt::UserRole, QVariant(name));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
}

QStringList BScreenListWidget::names()
{
    QStringList computerNames;
    QListWidgetItem* widgetItem;
    for (int i = 0; i < count(); ++i)
    {
        widgetItem = item(i);
        if (!widgetItem)
            continue;
        QVariant variant = widgetItem->data(Qt::UserRole);
        if (variant.type() == QVariant::String)
        {
            computerNames.append(variant.toString());
        }
    }
    return computerNames;
}

void BScreenListWidget::dragEnterEvent(QDragEnterEvent* event)
{
     if (event->mimeData()->hasFormat(Mimetype))
         event->accept();
     else
         event->ignore();
}

void BScreenListWidget::dragMoveEvent(QDragMoveEvent* event)
{
     if (event->mimeData()->hasFormat(Mimetype))
     {
         event->setDropAction(Qt::MoveAction);
         event->accept();
     }
     else
     {
         event->ignore();
     }
}

void BScreenListWidget::dropEvent(QDropEvent* event)
{
     if (event->mimeData()->hasFormat(Mimetype))
     {
         QByteArray drop_data = event->mimeData()->data(Mimetype);
         QDataStream dataStream(&drop_data, QIODevice::ReadOnly);
         QString name;

         dataStream >> name;

         addScreen(name);

         event->setDropAction(Qt::MoveAction);
         event->accept();
     }
     else
     {
         event->ignore();
     }
}

void BScreenListWidget::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions);
    
    QListWidgetItem * const item = currentItem();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QString name = item->data(Qt::UserRole).toString();

    dataStream << name;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(Mimetype, itemData);

    QPixmap pixmap(120, 80);
    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::green);
    painter.drawRect(pixmap.rect().adjusted(0, 0, -1, -1));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, name);
    painter.end();

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(pixmap.rect().topLeft());
    drag->setPixmap(pixmap);

    if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
    {
        delete takeItem(row(item));
    }
}

#include "moc_bscreenlistwidget.cpp"
