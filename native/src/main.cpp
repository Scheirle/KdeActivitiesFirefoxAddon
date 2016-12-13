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

#include <QGuiApplication>
#include <QDebug>

#include <iostream>


int main(int argc, char** argv)
{
	// QGuiApplication is requiered for KWindowSystem
	QGuiApplication app(argc, argv);
	if (app.isSessionRestored())
	{
		// Prevent this app from launching by restoring a previous session (e.g. at startup)
		// Firefox always launches this app manually
		return 0;
	}

	FFWindowsManager manager;
	return app.exec();
}
