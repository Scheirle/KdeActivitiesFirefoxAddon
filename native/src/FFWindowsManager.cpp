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

#include "FFWindowsManager.hpp"

#include <KWindowSystem/kwindowsystem.h>
#include <KWindowSystem/kwindowinfo.h>

#include <QtCore/QTimer>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QThread>
#include <QtCore/QCoreApplication>
#include <QtGui/QWindow>

FFWindowsManager::FFWindowsManager(QObject* parent)
	: QObject(parent)
	, con(this)
	, updatingWindows(false)
{
	KWindowSystem* kws = KWindowSystem::self();
	connect(kws, SIGNAL(windowAdded(WId)),   this, SLOT(onKWSWindowAdded(WId)));
	connect(kws, SIGNAL(windowRemoved(WId)), this, SLOT(onKWSWindowRemoved(WId)));

	connect(&con, &Connection::newMessage, this, &FFWindowsManager::onMessage);
	connect(&con, &Connection::closed,     [](){ qDebug() << "ByeBye."; QCoreApplication::quit(); });
}

void FFWindowsManager::onKWSWindowAdded(WId id)
{
	static bool firstTimeDelay = true;
	if (!FFWindow::isFFWindow(id))
		return;

	if (!getWindow(id))
	{
		auto w = std::make_shared<FFWindow>(id, this);
		windows.push_back(w);
		if (firstTimeDelay)
		{
			// The first time we receive this signal is at startup
			// Therefor it is likely that firefox restores multiple windows
			// so we wait a bit to do a bulk update
			firstTimeDelay = false;
			QTimer::singleShot(1000, this, &FFWindowsManager::updateWindows);
		}
		else
		{
			QTimer::singleShot(0, this, &FFWindowsManager::updateWindows);
		}
	}
	//dump();
}

void FFWindowsManager::onKWSWindowRemoved(WId id)
{
	auto w = getWindow(id);
	if (w)
	{
		windows.removeOne(w);
		dump();
	}
}


void FFWindowsManager::updateWindows()
{
	if (updatingWindows)
	{
		QTimer::singleShot(100, this, &FFWindowsManager::updateWindows);
		return;
	}
	updatingWindows = true;

	int windowsToUpdate = 0;
	QJsonArray array;
	for (auto w : windows)
	{
		FFWindow::FFId id = w->getFFId();
		if (FFWindow::isValidFFId(id))
		{
			array.append(QJsonValue(static_cast<int>(id)));
		}
		else
		{
			updatingWindowsSingleUpdate = w;
			windowsToUpdate++;
		}
	}
	if (windowsToUpdate == 0)
	{
		updatingWindows = false;
		return;
	}
	else if (windowsToUpdate == 1)
	{
		QJsonObject obj;
		obj.insert("exclude", array);
		con.sendMessage("ffwindowsmanager", "singleupdate", obj);
		return;
	}
	else
	{
		QJsonObject obj;
		obj.insert("exclude", array);
		con.sendMessage("ffwindowsmanager", "title_settoid", obj);
		return;
	}
}

void FFWindowsManager::onMessage(QString kwsId, QString type, const QJsonValue& data)
{
	if (kwsId != "ffwindowsmanager")
		return;

	qDebug() << "FFWindowsManager::onMessage:";

	if (type == "title_tabscreated")
	{
		for (auto w : windows)
		{
			w->setFFIdViaTitle();
		}

		con.sendMessage("ffwindowsmanager", "title_restore", QJsonValue(""));
		dump();
		updatingWindows = false;
	}
	else if (type == "singleupdate_unknownwindows")
	{
		QJsonArray array = data.toArray();
		if (array.size() == 1)
		{
			FFWindow::FFId id = array.first().toInt(-2);
			Q_ASSERT(FFWindow::isValidFFId(id));
			updatingWindowsSingleUpdate->setFFId(id);
			qDebug() << "singleupdate_unknownwindows: " << id;
			dump();
		}
		else
		{
			// Seems like we missed a lot of windows -> do a bulk update
			QTimer::singleShot(0, this, &FFWindowsManager::updateWindows);
		}
		updatingWindows = false;
	}
	else if (type == "newtab")
	{
		QJsonObject obj = data.toObject();
		int         wid = obj["wid"].toInt(-2);
		int         tid = obj["tid"].toInt(-2);
		onFFTabAdded(wid, tid);
	}
	else
	{
		qDebug() << "FFWindowsManager::onMessage: Unknown Type";
		qDebug() << "__Type: " << type;
		qDebug() << "__Data: " << data;
	}
}


FFWindowsManager::FFWindow_ptr FFWindowsManager::getWindow(FFWindow::FFId id)
{
	for (auto w : windows)
	{
		if (w->getFFId() == id)
			return w;
	}
	return nullptr;
}

FFWindowsManager::FFWindow_ptr FFWindowsManager::getWindow(WId id)
{
	for (auto w : windows)
	{
		if (w->getKwsID() == id)
			return w;
	}
	return nullptr;
}

void FFWindowsManager::dump() const
{
	qDebug() << "FFWindowsManager::dump:";
	qDebug() << "_windows:";
	for (auto w : windows)
	{
		qDebug() << "__" << w->toString();
	}
}

void FFWindowsManager::onFFTabAdded(FFWindow::FFId ffWindowID, int ffTabID)
{
	auto window = getWindow(ffWindowID);
	if (!window)
	{
		qDebug() << "onFFTabAdded: No Matching Window found.";
		return;
	}

	const QString currentActivity = getCurrentActivity();
	if (window->isOnActivity(currentActivity))
	{
		qDebug() << "onFFTabAdded: nothing to do, already on correct activity.";
		return;
	}

	// find a window on the current Activity
	FFWindow_ptr newOwner = nullptr;
	for (auto w : windows)
	{
		if (w->isReady() && w->isOnActivity(currentActivity))
		{
			newOwner = w;
			break;
		}
	}

	if (!newOwner)
	{
		// Open new Window on current activity
		qDebug() << "onFFTabAdded: Move to new Window";
		QJsonObject obj;
		obj.insert("tid", QJsonValue(ffTabID));
		obj.insert("wid", QJsonValue(ffWindowID));

		con.sendMessage("ffwindowsmanager", "movetabtonewwindow", obj);
	}
	else
	{
		// Move Tab to this window
		qDebug() << "onFFTabAdded: Move to other Window";

		QJsonObject obj;
		obj.insert("tid", QJsonValue(ffTabID));
		obj.insert("wid", QJsonValue(newOwner->getFFId()));
		con.sendMessage("ffwindowsmanager", "movetabtowindow", obj);

		KWindowSystem::forceActiveWindow(newOwner->getKwsID());
	}

	// Do not highlight the orignal window
	QThread::msleep(1000);
	KWindowSystem::demandAttention(window->getKwsID(), false);
}
