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


#ifndef BSYNERGYWIDGET_H
#define BSYNERGYWIDGET_H

#include "ui_bsyngerywidget.h"

#include <QWidget>

#include <base/bscreenconfigglobal.h>

class BSynergyClient;
class BSynergyServer;

class BSynergyWidget : public QWidget, public Ui::BSynergyWidget
{
    Q_OBJECT
public:
    explicit BSynergyWidget(QWidget* parent = nullptr);
    ~BSynergyWidget();

protected slots:
    void startClicked();
    void stopClicked();
    void ipLineEditChanged(const QString& text);
    void configureServerClicked();
    void serverRadioButtonChecked(bool checked);
    void clientRadioButtonChecked(bool checked);

private:
    BScreenConfigGlobal::ScreenLayoutMap m_rules;

    BSynergyServer* m_server;
    BSynergyClient* m_client;
};

#endif // BSYNERGYWIDGET_H
