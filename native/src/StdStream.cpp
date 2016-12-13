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

#include "StdStream.hpp"

#include <QDebug>

#include <stdio.h>  //Provides stdin and stdout
#include <unistd.h> //Provides STDIN_FILENO

StdStream::StdStream(StdStream::Type type)
	: QDataStream()
	, notifier(STDIN_FILENO, QSocketNotifier::Read)
{

	bool ok = false;
	if (type == Input)
	{
		ok = file.open(stdin, QIODevice::ReadOnly | QIODevice::Text);
		connect(&notifier, &QSocketNotifier::activated, this, &StdStream::readyRead);
	}
	else if (type == Output)
	{
		ok = file.open(stdout, QIODevice::WriteOnly);
	}

	Q_ASSERT(ok);

	this->setDevice(&file);
	// TODO: Set to native byte order. Following works (only) for me.
	// See https://developer.mozilla.org/en-US/Add-ons/WebExtensions/Native_messaging#App_side
	this->setByteOrder(QDataStream::LittleEndian);

}
