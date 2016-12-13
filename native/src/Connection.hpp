/*
 * This file is part of KdeActivitiesFirefoxAddon
 *
 * KdeActivitiesFirefoxAddon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * KdeActivitiesFirefoxAddon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Copyright (C) 2016  Bernhard Scheirle <bernhard@scheirle.de>
 *
 */

#pragma once

#include "StdStream.hpp"

#include <QtCore/QObject>
#include <QtCore/QTextStream>
#include <QtCore/QVector>

class QJsonDocument;
class QJsonValue;

struct sFFWindow
{
	int ffWindowID;
	QString title;
};


class Connection
    : public QObject
{
	Q_OBJECT
public:
	explicit Connection(QObject* parent);
	virtual ~Connection() = default;

private:
	void onNewMessage(const QJsonDocument& json);
	void onNewMessage_WindowList(const QJsonValue& json);
	void onNewMessage_NewTab(const QJsonValue& json);
	void onNewMessage_NewWindow(const QJsonValue& json);
	void onNewMessage_WindowRemoved(const QJsonValue& json);
	void writeMessage(const QString& type, const QJsonValue& json);

signals:
	void closed();
	void newMessage(QString kwsID, QString type, const QJsonValue& data);

public slots:
	void sendMessage(QString kwsID, QString type, const QJsonValue& data);

private slots:
	void onReadyRead();

private:
	/// from the plugin to the native app
	StdStream in;
	/// from the native app to the plugin
	StdStream out;

};

