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

#include <QtCore/QObject>
#include <QtGui/qwindowdefs.h>

class QJsonValue;
class FFWindowsManager;


class FFWindow
	: public QObject
{
	Q_OBJECT
public:
	enum State {
		Init,  // We currently try to get the firefox window id
		Ready, // We hava a firefox window id
		Broken // We failed to get a firefox window id
	};

	typedef int FFId;

	FFWindow(WId kwsId, FFWindowsManager* manager);
	virtual ~FFWindow() = default;

	/**
	 * @brief Returns the current window title.
	 *
	 * Not to be confused with the tab title.
	 */
	QString getTitle() const;

	WId  getKwsID() const { return kwsID; }
	FFId getFFId()  const;
	void setFFId(FFId id);
	void setFFIdViaTitle();

	bool isReady() const { return state == Ready; };

	/// If the list is empty or contains a null UUID, the window is on all activities.
	QStringList getActivities() const;
	bool isOnActivity(const QString& uuid) const;

	QString toString() const;

	static bool isFFWindow(WId id);
	static bool isValidFFId(FFId id) { return id >= 0;}

public:
	static const FFWindow::FFId InvalidFFId;

private:
	FFWindowsManager* manager;
	State             state;
	/// @brief Window Id (Qt/KWindowSystem)
	WId               kwsID;
	/// @brief Firefox window Id
	FFWindow::FFId    ffID;
};
