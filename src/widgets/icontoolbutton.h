/*
 * iconwidget.h - misc. Iconset- and Icon-aware widgets
 * Copyright (C) 2003  Michail Pishchagin
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef ICONTOOLBUTTON_H
#define ICONTOOLBUTTON_H

#include <QToolButton>
#include <QPixmap>

class Icon;
class Iconset;

class IconToolButton : public QToolButton
{
	Q_OBJECT
	Q_PROPERTY( QString iconName READ iconName WRITE setIcon )

	Q_OVERRIDE( QPixmap pixmap DESIGNABLE false SCRIPTABLE false )
	Q_OVERRIDE( QIconSet iconSet DESIGNABLE false SCRIPTABLE false )

public:
	IconToolButton(QWidget *parent = 0);
	~IconToolButton();

	void setIcon(const QIcon &);

public slots:
	void setIcon(const Icon *, bool activate = true);
	void setIcon(const QString &);
	QString iconName() const;

public:
	class Private;
private:
	Private *d;
};

#endif