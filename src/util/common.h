#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QDateTime>
#include <libnova/ln_types.h>
#include <libnova/julian_day.h>

#define ONE_AU_IN_METER 149597870700.0
#define ONE_METER_IN_AU (1.0 / 149597870700.0)

#define DEFAULT_DOWNLOAD_MPCORB_DAT_GZ_URL "https://www.minorplanetcenter.net/iau/MPCORB/MPCORB.DAT.gz"
#define DEFAULT_DOWNLOAD_COMET_ELS_URL "https://www.minorplanetcenter.net/iau/MPCORB/CometEls.txt"

static QString JdToQStringDate(double jd)
{
    struct ln_date date;
    ln_get_date(jd, &date);

    return QString::number(date.years) + "-"
	+ QString::number(date.months).rightJustified(2, '0') + "-"
	+ QString::number(date.days).rightJustified(2, '0') + " "
	+ QString::number(date.hours).rightJustified(2, '0') + ":"
	+ QString::number(date.minutes).rightJustified(2, '0');
}

static double DateTimeToJD(const QDateTime &DateTime)
{
     struct ln_date date {
	.years = DateTime.date().year(),
	.months = DateTime.date().month(),
	.days = DateTime.date().day(),
	.hours = DateTime.time().hour(),
	.minutes = DateTime.time().minute(),
	.seconds = (double)DateTime.time().second()
    };

    return ln_get_julian_day(&date);
}

static void JDToDateTime(QDateTime *DateTime, double JD)
{
    struct ln_date date;

    ln_get_date(JD, &date);
    DateTime->setDate(QDate(date.years, date.months, date.days));
    DateTime->setTime(QTime(date.hours, date.minutes, date.seconds));
}

#endif	/* COMMON_H */
