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

#include "FFWindow.hpp"
#include "Connection.hpp"
#include "FFWindowsManager.hpp"

#include <KWindowSystem/kwindowsystem.h>
#include <KWindowSystem/kwindowinfo.h>

#include <QTimer>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QUuid>
#include <QDebug>


const FFWindow::FFId FFWindow::InvalidFFId = -2;


FFWindow::FFWindow(WId kwsId, FFWindowsManager* manager)
	: manager(manager)
	, state(Init)
	, kwsID(kwsId)
	, ffID(InvalidFFId)
{
}

bool FFWindow::isFFWindow(WId id)
{
	KWindowInfo info(id, {}, {NET::WM2WindowClass});
	return info.valid() && info.windowClassName() == "Navigator" && info.windowClassClass() == "Firefox";
}

QStringList FFWindow::getActivities() const
{
	KWindowInfo info(kwsID, {}, {NET::WM2Activities});
	if (!info.valid())
	{
		return QStringList();
	}
	return info.activities();
}

bool FFWindow::isOnActivity(const QString& uuid) const
{
	QStringList activities = getActivities();
	if (activities.isEmpty())
		return true;
	if (activities.contains(uuid))
		return true;
	if (activities.contains(QUuid().toString()))
		return true;
	return false;
}

FFWindow::FFId FFWindow::getFFId() const
{
	if (state == FFWindow::Ready)
		return ffID;
	return InvalidFFId;
}

void FFWindow::setFFId(FFId id)
{
	ffID  = id;
	state = isValidFFId(ffID) ? Ready : Broken;
}

void FFWindow::setFFIdViaTitle()
{
	if (state != Init)
		return;

	QString title = getTitle();
	title.replace(" - Mozilla Firefox", "");
	bool ok = false;
	ffID = title.toInt(&ok);
	if (ok)
		state = Ready;
	qDebug() << "FFWindow::setFFIDviaTitle: ID: " << ffID << " from Title: " << title;
}

QString FFWindow::getTitle() const
{
	KWindowInfo info(kwsID, {NET::WMName}, {});
	if (!info.valid())
	{
		return QString("**Invalid KWindowInfo**");
	}
	return info.name();
}


QString FFWindow::toString() const
{
	static const QString SInit   = "Init";
	static const QString SReady  = "Ready";
	static const QString SBroken = "Broken";

	QString result;
	result += "FFWindow(" + QString::number(kwsID) + ", " + QString::number(ffID) + ", ";
	switch (state)
	{
	case Init:   result += SInit;   break;
	case Ready:  result += SReady;  break;
	case Broken: result += SBroken; break;
	default:
		Q_ASSERT(false);
	}
	result += ", " + getTitle() + ")";
	return result;
}

