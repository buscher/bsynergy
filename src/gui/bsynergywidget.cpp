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


#include "bsynergywidget.h"

#include <QDebug>
#include <QPointer>

#include "bscreenlayoutconfig.h"

#include "server/bsynergyserver.h"
#include "client/bsynergyclient.h"

BSynergyWidget::BSynergyWidget(QWidget* parent)
    : QWidget(parent),
      m_server(nullptr),
      m_client(nullptr)
{
    setupUi(this);

    connect(m_startButton, &QPushButton::clicked, this, &BSynergyWidget::startClicked);
    connect(m_configureServer, &QPushButton::clicked, this, &BSynergyWidget::configureServerClicked);

    connect(m_ipLineEdit, &QLineEdit::textChanged, this, &BSynergyWidget::ipLineEditChanged);
    connect(m_serverRadioButton, &QRadioButton::clicked, this, &BSynergyWidget::serverRadioButtonChecked);
    connect(m_clientRadioButton, &QRadioButton::clicked, this, &BSynergyWidget::clientRadioButtonChecked);
}

BSynergyWidget::~BSynergyWidget()
{
    delete m_server;
    delete m_client;
}

void BSynergyWidget::configureServerClicked()
{
    QPointer<BScreenLayoutConfig> config = new BScreenLayoutConfig(this);
    if (config->exec() == QDialog::Accepted)
    {
        QString errorMsg;
        m_rules = config->rules(&errorMsg);
        qDebug() << errorMsg;
    }
    serverRadioButtonChecked(true);
}

void BSynergyWidget::ipLineEditChanged(const QString& text)
{
    if (text.isEmpty())
    {
        m_startButton->setEnabled(false);
    }
    else
    {
        m_startButton->setEnabled(true);
    }
}

void BSynergyWidget::startClicked()
{
    bool started = false;
    qDebug() << "hello start clicked";
    if (m_serverRadioButton->isChecked() && !m_rules.isEmpty())
    {
        if (m_server == nullptr)
        {
            m_server = new BSynergyServer(m_rules, this);
            started = true;
            qDebug() << "server startet";
        }
    }
    else if (m_clientRadioButton->isChecked() && !m_ipLineEdit->text().isEmpty())
    {
        if (m_client == nullptr)
        {
            m_client = new BSynergyClient(this);
        }
        started = true;
        m_client->openScreen();
        m_client->connectTo(m_ipLineEdit->text());
        qDebug() << "client startet";
    }

    if (started)
    {
        disconnect(m_startButton, nullptr, nullptr, nullptr);
        connect(m_startButton, &QPushButton::clicked, this, &BSynergyWidget::stopClicked);
        m_startButton->setText(QLatin1String("Stop"));
    }
}

void BSynergyWidget::stopClicked()
{
    qDebug() << "BSynergyWidget::stopClicked";
    if (m_server)
    {
        disconnect(m_server, nullptr, nullptr, nullptr);
        m_server->close();
        delete m_server;
        m_server = nullptr;
    }
    else if (m_client)
    {
        disconnect(m_client, nullptr, nullptr, nullptr);
        m_client->close();
        delete m_client;
        m_client = nullptr;
    }
    m_startButton->setText(QLatin1String("Start"));
    disconnect(m_startButton, nullptr, nullptr, nullptr);
    connect(m_startButton, &QPushButton::clicked, this, &BSynergyWidget::startClicked);
}

void BSynergyWidget::serverRadioButtonChecked(bool checked)
{
    if (checked)
    {
        if (m_rules.isEmpty())
        {
            m_startButton->setEnabled(false);
        }
        else
        {
            m_startButton->setEnabled(true);
        }
    }
}

void BSynergyWidget::clientRadioButtonChecked(bool checked)
{
    if (checked)
    {
        if (m_ipLineEdit->text().isEmpty())
        {
            m_startButton->setEnabled(false);
        }
        else
        {
            m_startButton->setEnabled(true);
        }
    }
}

#include "moc_bsynergywidget.cpp"
