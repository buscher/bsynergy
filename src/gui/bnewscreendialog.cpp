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


#include "bnewscreendialog.h"

#include <QAbstractButton>

BNewScreenDialog::BNewScreenDialog(QStringList names, QWidget* parent)
    : QDialog(parent),
      m_names(names)
{
    setupUi(this);
    QList<QAbstractButton *> buttonList = m_buttonBox->buttons();
    buttonList.first()->setEnabled(false);
//     m_buttonBox->buttons().first()->setEnabled(false);
    connect(m_nameLineEdit, &QLineEdit::textChanged, this, &BNewScreenDialog::textChanged);
}

QString BNewScreenDialog::name() const
{
    return m_name;
}

void BNewScreenDialog::textChanged(const QString& text)
{
    QList<QAbstractButton *> buttonList = m_buttonBox->buttons();

    if (!text.isEmpty() && !m_names.contains(text))
    {
//         m_buttonBox->buttons().first()->setEnabled(true);
        buttonList.first()->setEnabled(true);
        m_name = text;
    }
    else
    {
//         m_buttonBox->buttons().first()->setEnabled(false);
        buttonList.first()->setEnabled(false);
    }
}

#include "moc_bnewscreendialog.cpp"
