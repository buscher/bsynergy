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


#include "bscreenlayoutconfig.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPointer>
#include <QDialog>
#include <QListWidgetItem>

#include "bscreenlistwidget.h"
#include "bscreenlayoutwidget.h"
#include "bnewscreendialog.h"

BScreenLayoutConfig::BScreenLayoutConfig(QWidget* parent)
    : QDialog(parent)
{
#if (QT_VERSION == QT_VERSION_CHECK(5, 9, 2))
//     https://bugreports.qt.io/browse/QTBUG-63846
    setWindowModality(Qt::WindowModal);
#endif

    QHBoxLayout *hbuttonbox = new QHBoxLayout();
    m_newButton = new QPushButton(QLatin1String("New"));
    hbuttonbox->addWidget(m_newButton);
    m_deleteButton = new QPushButton(QLatin1String("Delete"));
    m_deleteButton->setEnabled(false);
    hbuttonbox->addWidget(m_deleteButton);

    QVBoxLayout* vListBox = new QVBoxLayout();
    m_screenListWidget = new BScreenListWidget();
    m_screenListWidget->setMaximumWidth(200);
    vListBox->addWidget(m_screenListWidget);
    vListBox->addLayout(hbuttonbox);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addLayout(vListBox);

    m_screenLayoutWidget = new BScreenLayoutWidget();
    hbox->addWidget(m_screenLayoutWidget);

    m_okButton = new QPushButton(QLatin1String("OK"));
    m_cancelButton = new QPushButton(QLatin1String("Cancel"));
    QHBoxLayout *hbuttonbox2 = new QHBoxLayout();
    hbuttonbox2->addStretch();
    hbuttonbox2->addWidget(m_okButton);
    hbuttonbox2->addWidget(m_cancelButton);

    QVBoxLayout* vBox = new QVBoxLayout();
    vBox->addLayout(hbox);
    vBox->addLayout(hbuttonbox2);

    setLayout(vBox);

    connect(m_newButton, &QPushButton::clicked, this, &BScreenLayoutConfig::newScreen);
    connect(m_deleteButton, &QPushButton::clicked, this, &BScreenLayoutConfig::deleteScreen);
    connect(m_screenListWidget, &BScreenListWidget::currentItemChanged,
            this, &BScreenLayoutConfig::currentItemChanged);

    connect(m_okButton, &QPushButton::clicked, this, &BScreenLayoutConfig::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &BScreenLayoutConfig::reject);
}

void BScreenLayoutConfig::newScreen()
{
    QStringList names;
    names << m_screenListWidget->names();
    names << m_screenLayoutWidget->names();
    QPointer<BNewScreenDialog> dialog = new BNewScreenDialog(names, this);

    if (dialog->exec() == QDialog::Accepted)
    {
        m_deletedItem = nullptr;
        m_screenListWidget->addScreen(dialog->name());
    }
}

void BScreenLayoutConfig::deleteScreen()
{
    qDebug() << m_screenListWidget->currentItem();
    if (m_screenListWidget->currentItem() != nullptr)
    {
        //BUG: remember removed item, as a Qt will tell us it is the active item again
        m_deletedItem = m_screenListWidget->currentItem();
        delete m_screenListWidget->takeItem(m_screenListWidget->row(m_screenListWidget->currentItem()));
        //m_screenListWidget->removeItemWidget(m_screenListWidget->currentItem());
    }
}

void BScreenLayoutConfig::currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    Q_UNUSED(previous);

    qDebug() << current;
    if (current == nullptr)
    {
        m_deleteButton->setEnabled(false);
    }
    else if (m_deletedItem != current)
    {
        qDebug() << current->data(Qt::UserRole).toString();
        m_deleteButton->setEnabled(true);
    }
}

BScreenConfigGlobal::ScreenLayoutMap BScreenLayoutConfig::rules(QString* errorMsg)
{
    return m_screenLayoutWidget->writeRules(errorMsg);
}

#include "moc_bscreenlayoutconfig.cpp"
