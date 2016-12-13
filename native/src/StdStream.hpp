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
#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QSocketNotifier>

/**
 * @brief Class to easily read from / write to standard input / output
 *
 */
class StdStream
	: public QObject, public QDataStream
{
	Q_OBJECT
public:
	enum Type { Input, Output };

	StdStream(Type type);
	virtual ~StdStream() = default;

	bool flush() { return file.flush(); };

signals:
	void readyRead();

private:
	QFile           file;
	QSocketNotifier notifier;
};
