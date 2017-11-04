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


#include "bscreenlayoutwidget.h"

#include <QPaintEvent>
#include <QDropEvent>
#include <QPainter>
#include <QHostInfo>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>


static const QLatin1String Mimetype = QLatin1String("image/x-screen-computer");
static const int ScreenCount = 5;

BScreenLayoutWidget::BScreenLayoutWidget(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle(QLatin1String("BSynegry - ScreenLayout"));
    setAcceptDrops(true);
    setMinimumSize(400, 400);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_dragMode = false;

    ScreenStruct host;
    host.x = 2;
    host.y = 2;
    host.name = QHostInfo::localHostName();
    m_computerList.append(host);

    ScreenStruct host2;
    host2.x = 1;
    host2.y = 2;
    host2.name = QLatin1String("LEFT");
    m_computerList.append(host2);

    ScreenStruct host3;
    host3.x = 3;
    host3.y = 2;
    host3.name = QLatin1String("RIGHT");
    m_computerList.append(host3);

    updateDropAble();
}

BScreenConfigGlobal::ScreenLayoutMap BScreenLayoutWidget::writeRules(QString *errorMessage)
{
    BScreenConfigGlobal::ScreenLayoutMap rules;

    foreach(const ScreenStruct& computer, m_computerList)
    {
        int indexUp = findScreenByIndex(computer.x, computer.y-1);
        if (indexUp != -1)
        {
            ScreenStruct computerTo = m_computerList.at(indexUp);
            addRule(&rules, computer.name, computerTo.name, BScreenConfigGlobal::AboveOf);
        }
        int indexDown = findScreenByIndex(computer.x, computer.y+1);
        if (indexDown != -1)
        {
            ScreenStruct computerTo = m_computerList.at(indexDown);
            addRule(&rules, computer.name, computerTo.name, BScreenConfigGlobal::BelowOf);
        }
        int indexRight = findScreenByIndex(computer.x+1, computer.y);
        if (indexRight != -1)
        {
            ScreenStruct computerTo = m_computerList.at(indexRight);
            addRule(&rules, computer.name, computerTo.name, BScreenConfigGlobal::RightOf);
        }
        int indexLeft = findScreenByIndex(computer.x-1, computer.y);
        if (indexLeft != -1)
        {
            ScreenStruct computerTo = m_computerList.at(indexLeft);
            addRule(&rules, computer.name, computerTo.name, BScreenConfigGlobal::LeftOf);
        }
    }

    foreach(const ScreenStruct& computer, m_computerList)
    {
//         map.values(k).foo ; // Use QMap::equal_range(k) instead
        if (rules.values(computer.name).isEmpty())
        {
            if (errorMessage)
                *errorMessage = QLatin1String("No rules found for host: ") + computer.name;
            break;
        }
    }

    return rules;
}

QStringList BScreenLayoutWidget::names()
{
    QStringList computerNames;
    foreach(const ScreenStruct& computer, m_computerList)
    {
        computerNames << computer.name;
    }
    return computerNames;
}

void BScreenLayoutWidget::paintEvent(QPaintEvent* event)
{
//     qDebug() << "paintEvent";
    QPainter painter;
    painter.begin(this);
    painter.fillRect(event->rect(), Qt::white);

    if (m_highLightRect.isValid())
    {
        painter.setBrush(QColor(0xd7, 0xe6, 0xff)); /*"#d7e6ff"*/ /*Qt::cyan*/ /*QColor("#ffcccc")*/
        painter.setPen(Qt::NoPen);
        painter.drawRect(m_highLightRect.adjusted(0, 0, -1, -1));
    }

    if (m_dragMode)
    {
        for (int i = 0; i < m_dropAreas.count(); ++i)
        {
            painter.setBrush(Qt::transparent);
            painter.setPen(QPen(Qt::red, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
            painter.drawRect(m_dropAreas[i]);
        }
    }

    for (int i = 0; i < m_computerList.size(); ++i)
    {
        ScreenStruct computer = m_computerList.at(i);
        QRect target = targetSquare(computer.x, computer.y);
        QColor fillColor;
        if (computer.x == 2 && computer.y == 2)
        {
            fillColor = Qt::red;
        }
        else
        {
            fillColor = Qt::green;
        }
        painter.setPen(Qt::black);
        painter.setBrush(fillColor);
        painter.drawRect(target);
        painter.drawText(target, Qt::AlignCenter, computer.name);
    }
/*
     if (highlightedRect.isValid()) {
         painter.setBrush(QColor("#ffcccc"));
         painter.setPen(Qt::NoPen);
         painter.drawRect(highlightedRect.adjusted(0, 0, -1, -1));
     }

     for (int i = 0; i < pieceRects.size(); ++i) {
         painter.drawPixmap(pieceRects[i], piecePixmaps[i]);
     }*/
    painter.end();
//     qDebug() << "paintEvent END";
}

void BScreenLayoutWidget::dragEnterEvent(QDragEnterEvent* event)
{
//     qDebug() << "dragEnterEvent";
    if (event->mimeData()->hasFormat(Mimetype))
    {
        m_dragMode = true;
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    }
    else
    {
        m_dragMode = false;
        event->ignore();
    }
//     qDebug() << "dragEnterEvent END";
}

void BScreenLayoutWidget::dragMoveEvent(QDragMoveEvent* event)
{
//     qDebug() << "dragMoveEvent " << event->pos();
    if (event->mimeData()->hasFormat(Mimetype) &&
        positionInDropArea(event->pos()))
    {
        m_dragMode = true;
        m_highLightRect = targetSquare(event->pos().x() / (width() / ScreenCount), event->pos().y() / (height() / ScreenCount));

        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    }
    else
    {
        m_highLightRect = QRect();
        //m_dragMode = false;
        event->ignore();
    }

    update();
//     qDebug() << "dragMoveEvent END";
}

void BScreenLayoutWidget::dragLeaveEvent(QDragLeaveEvent* event)
{
//     qDebug() << "dragLeaveEvent";
    m_highLightRect = QRect();
    m_dragMode = false;
    event->accept();
    update();
//     qDebug() << "dragLeaveEvent END";
}

void BScreenLayoutWidget::dropEvent(QDropEvent* event)
{
    qDebug() << "dropEvent";
    if (event->mimeData()->hasFormat(Mimetype) &&
        positionInDropArea(event->pos()))
    {
        QByteArray pieceData = event->mimeData()->data(Mimetype);
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);

        QString name;
        dataStream >> name;
        qDebug() << name;
        ScreenStruct computer;
        computer.x = event->pos().x() / (width() / ScreenCount);
        computer.y = event->pos().y() / (height() / ScreenCount);
        computer.name = name;

        m_computerList.append(computer);
        updateDropAble();
        update();

        event->setDropAction(Qt::MoveAction);
        event->accept();

    }
    else
    {
        event->ignore();
    }
    m_highLightRect = QRect();
    m_dragMode = false;
}

void BScreenLayoutWidget::resizeEvent(QResizeEvent* event)
{
    updateDropAble();
    QWidget::resizeEvent(event);
}

void BScreenLayoutWidget::mousePressEvent(QMouseEvent* event)
{
//     qDebug() << "mousePressEvent";
    int const index = findPiece(event->pos());
    if (index == -1)
    {
        return;
    }

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    ScreenStruct computer = m_computerList[index];
    dataStream << m_computerList[index].name << QPoint(0, 0);
    QMimeData * const mimeData = new QMimeData();
    mimeData->setData(Mimetype, itemData);
    mimeData->setText(computer.name);

    m_computerList.removeAt(index);
    updateDropAble();

    QDrag * const drag = new QDrag(this);
    drag->setMimeData(mimeData);

    QPixmap dragPix(width() / ScreenCount, height() / ScreenCount);
    QPainter painter(&dragPix);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::green);
    painter.drawRect(dragPix.rect().adjusted(0, 0, -1, -1));
    painter.drawText(dragPix.rect(), Qt::AlignCenter, computer.name);
    painter.end();

    drag->setHotSpot(/*event->pos() -*/ dragPix.rect().topLeft());
    drag->setPixmap(dragPix);

    update();
    event->accept();
    if (drag->exec(Qt::MoveAction, Qt::MoveAction) != Qt::MoveAction)
    {
        m_computerList.append(computer);
        updateDropAble();
    }
    update();
//     qDebug() << "mousePressEvent END";
}

const QRect BScreenLayoutWidget::targetSquare(int x, int y) const
{
    int tWidth = width() / ScreenCount;
    int tHeight = height() / ScreenCount;
    return QRect(x * tWidth, y * tHeight, tWidth, tHeight);
}

int BScreenLayoutWidget::findPiece(const QPoint& piecePoint) const
{
    for (int i = 0; i < m_computerList.count(); ++i)
    {
        if (m_computerList[i].x == 2 && m_computerList[i].y == 2)
        {
            continue;
        }

        if (targetSquare(m_computerList[i].x,m_computerList[i].y).contains(piecePoint))
        {
            return i;
        }
    }
    return -1;
}

int BScreenLayoutWidget::findScreenByIndex(int x, int y) const
{
    for (int i = 0; i < m_computerList.count(); ++i)
    {
        if (m_computerList[i].x == x && m_computerList[i].y == y)
        {
            return i;
        }
    }
    return -1;
}

QVector< QRect > BScreenLayoutWidget::findDropables(int x, int y)
{
    QVector< QRect > returnList;

    if (areaIsFree(x,y))
    {
        return returnList;
    }

    if (areaIsFree(x-1, y))
    {
        returnList.append(targetSquare(x-1,y));
    }
    if (areaIsFree(x, y-1))
    {
        returnList.append(targetSquare(x,y-1));
    }
    if (areaIsFree(x+1, y))
    {
        returnList.append(targetSquare(x+1,y));
    }
    if (areaIsFree(x, y+1))
    {
        returnList.append(targetSquare(x,y+1));
    }

    return returnList;
}

bool BScreenLayoutWidget::areaIsFree(int x, int y)
{
    if (!validArea(x,y))
        return false;

    for (int i = 0; i < m_computerList.count(); ++i)
    {
        ScreenStruct computer = m_computerList.at(i);
        if (computer.x == x && computer.y == y)
        {
            return false;
        }
    }
    return true;
}

bool BScreenLayoutWidget::validArea(int x, int y)
{
    if (x >= 0 && x < ScreenCount && y >= 0 && y < ScreenCount)
    {
        return true;
    }
    return false;
}

bool BScreenLayoutWidget::positionInDropArea(const QPoint& pos)
{
    //qDebug() << pos;
    for (int i = 0; i < m_dropAreas.count(); ++i)
    {
        //qDebug() << m_dropAreas[i];
        if (m_dropAreas[i].contains(pos))
        {
            return true;
        }
    }
    return false;
}

void BScreenLayoutWidget::updateDropAble()
{
//     qDebug() << "updateDropAble";
    m_dropAreas.clear();

    for (int x = 0; x < ScreenCount; ++x)
    {
        for (int y = 0; y < ScreenCount; ++y)
        {
            QVector<QRect> dropable = findDropables(x,y);

            for (int i = 0; i < dropable.count(); ++i)
            {
                if (!m_dropAreas.contains(dropable[i]))
                {
                    m_dropAreas.append(dropable[i]);
                    //qDebug() << dropable[i];
                }
            }
        }
    }
//     qDebug() << "updateDropAble END";
}

void BScreenLayoutWidget::addRule(BScreenConfigGlobal::ScreenLayoutMap *rules, const QString& computer, const QString& computerTo, BScreenConfigGlobal::ScreenLayout direction)
{
    BScreenConfigGlobal::ScreenLayoutStruct layout;
    layout.connection = direction;
    layout.name = computerTo;
    rules->insertMulti(computer, layout);
}

#include "moc_bscreenlayoutwidget.cpp"
