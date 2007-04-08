/*
 * alerticon.cpp - class for handling animating alert icons
 * Copyright (C) 2003  Michail Pishchagin
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "alerticon.h"
#include "common.h"
#include <qtimer.h>
#include <qapplication.h>
//Added by qt3to4:
#include <QPixmap>

//----------------------------------------------------------------------------
// MetaAlertIcon
//----------------------------------------------------------------------------

class MetaAlertIcon : public QObject
{
	Q_OBJECT
public:
	MetaAlertIcon();
	~MetaAlertIcon();

	Impix blank16() const;
	int framenumber() const;

signals:
	void updateFrame(int frame);
	void update();

public slots:
	void updateAlertStyle();

private slots:
	void animTimeout();

private:
	QTimer *animTimer;
	int frame;
	Impix _blank16;
};

static MetaAlertIcon *metaAlertIcon = 0;

MetaAlertIcon::MetaAlertIcon()
: QObject(qApp)
{
	animTimer = new QTimer(this);
	connect(animTimer, SIGNAL(timeout()), SLOT(animTimeout()));
	animTimer->start(120 * 5);
	frame = 0;

	// blank icon
	QImage blankImg(16, 16, QImage::Format_ARGB32);
	blankImg.fill(0x00000000);
	_blank16.setImage(blankImg);
}

MetaAlertIcon::~MetaAlertIcon()
{
}

Impix MetaAlertIcon::blank16() const
{
	return _blank16;
}

void MetaAlertIcon::animTimeout()
{
	frame = !frame;
	emit updateFrame(frame);
}

void MetaAlertIcon::updateAlertStyle()
{
	emit update();
}

int MetaAlertIcon::framenumber() const
{
	return frame;
}

//----------------------------------------------------------------------------
// AlertIcon::Private
//----------------------------------------------------------------------------

class AlertIcon::Private : public QObject
{
	Q_OBJECT
public:
	Private(AlertIcon *_ai);
	~Private();

	void init();

public slots:
	void update();
	void activated(bool playSound);
	void stop();

	void updateFrame(int frame);
	void pixmapChanged(const QPixmap &);

public:
	AlertIcon *ai;
	Icon *real;
	bool isActivated;
	Impix impix;
};

AlertIcon::Private::Private(AlertIcon *_ai)
{
	if ( !metaAlertIcon )
		metaAlertIcon = new MetaAlertIcon();

	ai = _ai;
	real = 0;
	isActivated = false;
}

AlertIcon::Private::~Private()
{
	if ( isActivated )
		stop();

	if ( real )
		delete real;
}

void AlertIcon::Private::init()
{
	connect(metaAlertIcon, SIGNAL(update()), SLOT(update()));
	connect(real, SIGNAL(iconModified(const QPixmap &)), SLOT(pixmapChanged(const QPixmap &)));

	if ( option.alertStyle == 2 && real->isAnimated() )
		impix = real->frameImpix();
	else
		impix = real->impix();
}

void AlertIcon::Private::update()
{
	stop();
	activated(false);
}

void AlertIcon::Private::activated(bool playSound)
{
	if ( option.alertStyle == 2 && real->isAnimated() ) {
		if ( !isActivated ) {
			connect(real, SIGNAL(pixmapChanged(const QPixmap &)), SLOT(pixmapChanged(const QPixmap &)));
			real->activated(playSound);
			isActivated = true;
		}
	}
	else if ( option.alertStyle == 1 || (option.alertStyle == 2 && !real->isAnimated()) ) {
		connect(metaAlertIcon, SIGNAL(updateFrame(int)), SLOT(updateFrame(int)));
	}
	else {
		impix = real->impix();
		emit ai->pixmapChanged(impix.pixmap());
	}
}

void AlertIcon::Private::stop()
{
	disconnect(metaAlertIcon, SIGNAL(updateFrame(int)), this, SLOT(updateFrame(int)));

	if ( isActivated ) {
		disconnect(real, SIGNAL(pixmapChanged(const QPixmap &)), this, SLOT(pixmapChanged(const QPixmap &)));
		real->stop();
		isActivated = false;
	}
}

void AlertIcon::Private::updateFrame(int frame)
{
	if ( !metaAlertIcon ) // just in case
		metaAlertIcon = new MetaAlertIcon();

	if ( frame )
		impix = real->impix();
	else
		impix = metaAlertIcon->blank16();

	emit ai->pixmapChanged(impix.pixmap());
}

void AlertIcon::Private::pixmapChanged(const QPixmap &p)
{
	impix = real->frameImpix();

	emit ai->pixmapChanged(p);
}

//----------------------------------------------------------------------------
// AlertIcon
//----------------------------------------------------------------------------

AlertIcon::AlertIcon(const Icon *icon)
{
	d = new Private(this);
	if ( icon )
		d->real = new Icon(*icon);
	else
		d->real = new Icon();

	d->init();
}

AlertIcon::~AlertIcon()
{
	delete d;
}

bool AlertIcon::isAnimated() const
{
	return d->real->isAnimated();
}

const QPixmap &AlertIcon::pixmap() const
{
	return d->impix.pixmap();
}

const QImage &AlertIcon::image() const
{
	return d->impix.image();
}

void AlertIcon::activated(bool playSound)
{
	d->activated(playSound);
}

void AlertIcon::stop()
{
	d->stop();
}

const QIcon &AlertIcon::iconSet() const
{
	return d->real->iconSet();
}

const Impix &AlertIcon::impix() const
{
	return d->impix;
}

int AlertIcon::frameNumber() const
{
	if ( option.alertStyle == 2 && d->real->isAnimated() ) {
		return d->real->frameNumber();
	}
	else if ( option.alertStyle == 1 || (option.alertStyle == 2 && !d->real->isAnimated()) ) {
		return metaAlertIcon->framenumber();
	}

	return 0;
}

const QString &AlertIcon::name() const
{
	return d->real->name();
}

Icon *AlertIcon::copy() const
{
	return new AlertIcon(d->real);
}

//----------------------------------------------------------------------------

void alertIconUpdateAlertStyle()
{
	if ( !metaAlertIcon )
		metaAlertIcon = new MetaAlertIcon();

	metaAlertIcon->updateAlertStyle();
}

#include "alerticon.moc"