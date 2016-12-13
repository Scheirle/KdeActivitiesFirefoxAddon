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

#include "FFWindow.hpp"
#include "Connection.hpp"

#include <KActivities/kactivities/consumer.h>

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtGui/qwindowdefs.h>

#include <memory>

class Connection;
class QJsonValue;

class FFWindowsManager
	: public QObject
{
	Q_OBJECT
public:
	typedef std::shared_ptr<FFWindow> FFWindow_ptr;

	explicit FFWindowsManager(QObject* parent = 0);
	virtual ~FFWindowsManager() = default;

	FFWindow_ptr getWindow(WId  id);
	FFWindow_ptr getWindow(FFWindow::FFId id);

	//const QList<FFWindow_ptr>& getWindows() const { return windows; };
	QString getCurrentActivity() const { return activities.currentActivity(); };

	void dump() const;

private slots:
	void onKWSWindowAdded(WId id);
	void onKWSWindowRemoved(WId id);

	void onFFTabAdded(FFWindow::FFId ffWindowID, int ffTabID);

	void updateWindows();
	void onMessage(QString kwsId, QString type, const QJsonValue& data);

private:
	Connection            con;
	KActivities::Consumer activities;
	QList<FFWindow_ptr>   windows;

	/// @brief Forces the next window which gets added on the current activity
	bool                  forceNextWindowOnCurrentActivity;
	/// @brief Set to true if we are currently updating the windows
	bool                  updatingWindows;
	/// @brief Set to the window to update if we are only updating a single window.
	/// Only valid as long as updatingWindows is true
	FFWindow_ptr          updatingWindowsSingleUpdate;
};
