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

#include "Connection.hpp"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>


Connection::Connection(QObject* parent)
	: QObject(parent)
	, in(StdStream::Input)
	, out(StdStream::Output)
	//, out(stdout, QIODevice::WriteOnly)
{
	//qDebug() << "Connection::Connection";
	connect(&in, &StdStream::readyRead, this, &Connection::onReadyRead);
}

void Connection::onReadyRead()
{
	//qDebug() << "onReadyRead";

	if (in.atEnd())
	{
		qDebug() << "Connection::onReadyRead: Input stream closed.";
		emit closed();
		return;
	}

	in.startTransaction();
	qint32 length;
	in >> length;
	if (length > 0)
	{
		in.rollbackTransaction();
	}
	else
	{
		in.commitTransaction();
		return;
	}

	QByteArray data;
	in >> data;

	QJsonDocument json = QJsonDocument::fromJson(data);
	onNewMessage(json);
}


void Connection::onNewMessage(const QJsonDocument& json)
{
	qDebug() << "newMessage: " << json;
	QJsonObject obj   = json.object();
	QString     type  = obj["type"].toString().toLower();
	QString     kwsID = obj["kwsid"].toString();
	QJsonValue  data  = obj["data"];

	emit newMessage(kwsID, type, data);
}


void Connection::sendMessage(QString kwsID, QString type, const QJsonValue& data)
{
	QJsonObject obj;
	obj.insert("kwsid", kwsID);
	obj.insert("type", type);
	obj.insert("data", data);

	QJsonDocument doc(obj);
	out << doc.toJson(QJsonDocument::Compact);
	out.flush();
	qDebug() << "sendMessage: " << doc;
}



