#include "mpcparser.h"

#define CHECK_AND_RETURN(b, s, l)						  \
if (!b) {                                                                         \
    l == "" ? qWarning() << "skipping empty line"                                 \
            : qWarning() << "conversation error" << s << "skipping line" << l;    \
    return b;                                                                     \
}

MpcParser::MpcParser(QString filename, OrbType orbType)
    : m_filename(filename), m_orbType(orbType), m_abort(false) { }

MpcParser::~MpcParser() { }

void MpcParser::abort()
{
    m_abort = true;
}

/* https://minorplanetcenter.net/iau/info/MPOrbitFormat.html */
bool MpcParser::parseAsteroid(const QString &line, struct asteroid_t *asteroid)
{
    bool ok;
    struct ln_date date;

    memset(&date, 0, sizeof(date));
    memset(asteroid, 0, sizeof(*asteroid));

    strcpy(asteroid->designation, line.mid(0, 7).toStdString().c_str());

    asteroid->H = QStringView{line}.mid(8, 5).toDouble(&ok);
    if (!ok) // CHECK_AND_SKIP(ok, line.mid(8, 5), line)
        asteroid->H = std::numeric_limits<double>::quiet_NaN();

    asteroid->G = QStringView{line}.mid(14, 5).toDouble(&ok);
    if (!ok) // CHECK_AND_SKIP(ok, line.mid(14, 5), line)
        asteroid->G = std::numeric_limits<double>::quiet_NaN();

    strcpy(asteroid->epoch, line.mid(20, 5).toStdString().c_str());
    ln_get_date_from_mpc(&date, asteroid->epoch);
    sprintf(asteroid->epoch, "%04d-%02d-%02d", date.years, date.months, date.days);

    asteroid->anomaly_mean = QStringView{line}.mid(26, 9).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(26, 9), line);

    asteroid->perihelion = QStringView{line}.mid(37, 9).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(37, 9), line);

    asteroid->ascending_node = QStringView{line}.mid(48, 9).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(48, 9), line);

    asteroid->inclination = QStringView{line}.mid(59, 9).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(59, 9), line);

    asteroid->eccentricity = QStringView{line}.mid(70, 9).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(70, 9), line);

    asteroid->daily_motion = QStringView{line}.mid(80, 11).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(80, 11), line);

    asteroid->semimajor_axis = QStringView{line}.mid(92, 11).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(92, 11), line);

    if (QStringView{line}.mid(117, 5).toString() != "     ") {
	asteroid->observations = QStringView{line}.mid(117, 5).toUInt(&ok);
	CHECK_AND_RETURN(ok, QStringView{line}.mid(117, 5), line);
    } else
	asteroid->observations = 0;

    asteroid->classification_flag = QStringView{line}.mid(161, 4).toUShort(&ok, 16);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(161, 4), line);

    strcpy(asteroid->designation_readable, line.mid(166, 28).trimmed().toStdString().c_str());

    strcpy(asteroid->last_observation, line.mid(194, 8).toStdString().c_str());
    sprintf(asteroid->last_observation, "%c%c%c%c-%c%c-%c%c",
	    asteroid->last_observation[0],asteroid->last_observation[1],
	    asteroid->last_observation[2],asteroid->last_observation[3],
	    asteroid->last_observation[4],asteroid->last_observation[5],
	    asteroid->last_observation[6],asteroid->last_observation[7]);

    return true;
}

/* https://www.minorplanetcenter.net/iau/info/CometOrbitFormat.html
   Columns   F77    Use

    1 -   4  i4     Periodic comet number
    5        a1     Orbit type (generally `C', `P' or `D')
    6 -  12  a7     Provisional designation (in packed form)

   15 -  18  i4     Year of perihelion passage
   20 -  21  i2     Month of perihelion passage
   23 -  29  f7.4   Day of perihelion passage (TT)

   31 -  39  f9.6   Perihelion distance (AU)
   42 -  49  f8.6   Orbital eccentricity

   52 -  59  f8.4   Argument of perihelion, J2000.0 (degrees)
   62 -  69  f8.4   Longitude of the ascending node, J2000.0
                      (degrees)
   72 -  79  f8.4   Inclination in degrees, J2000.0 (degrees)

   82 -  85  i4     Year of epoch for perturbed solutions
   86 -  87  i2     Month of epoch for perturbed solutions
   88 -  89  i2     Day of epoch for perturbed solutions

   92 -  95  f4.1   Absolute magnitude
   97 - 100  f4.0   Slope parameter

  103 - 158  a56    Designation and Name

  160 - 168  a9     Reference
 */
bool MpcParser::parseComet(const QString &line, struct comet_t *comet)
{
    bool ok;

    memset(comet, 0, sizeof(*comet));

    const char orbit_type = QStringView{line}.mid(4, 1).toString().at(0).toLatin1();
    switch (orbit_type) {
    case 'C': {
	strncpy(comet->orbit_type, "long-period", sizeof(comet->orbit_type));
	break;
    }
    case 'P': {
	strncpy(comet->orbit_type, "short-period", sizeof(comet->orbit_type));
	break;
    }
    case 'D': {
	strncpy(comet->orbit_type, "defunct", sizeof(comet->orbit_type));
	break;
    }
    case 'X': {
	strncpy(comet->orbit_type, "uncertain", sizeof(comet->orbit_type));
	break;
    }
    case 'I': {
	strncpy(comet->orbit_type, "interstellar", sizeof(comet->orbit_type));
	break;
    }
    case 'A': {
	strncpy(comet->orbit_type, "minor-planet", sizeof(comet->orbit_type));
	break;
    }
    default:
	comet->orbit_type[0] = '\0';
	break;
    }

    comet->perihelion_year = QStringView{line}.mid(14, 4).toUShort(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(14, 4), line);

    comet->perihelion_month = QStringView{line}.mid(19, 2).toUShort(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(19, 2), line);

    comet->perihelion_day = QStringView{line}.mid(22, 7).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(22, 7), line);

    comet->perihelion_distance = QStringView{line}.mid(30, 9).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(30, 9), line);

    comet->eccentricity = QStringView{line}.mid(41, 8).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(41, 8), line);

    comet->perihelion = QStringView{line}.mid(51, 8).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(51, 8), line);

    comet->ascending_node = QStringView{line}.mid(61, 8).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(61, 8), line);

    comet->inclination = QStringView{line}.mid(71, 8).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(71, 8), line);

#if COMET_PERTURBATION
    comet->epoch_pert_year = QStringView{line}.mid(81, 4).toUShort(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(81, 4), line);

    comet->epoch_pert_month = QStringView{line}.mid(85, 2).toUShort(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(85, 2), line);

    comet->epoch_pert_day = QStringView{line}.mid(87, 2).toUShort(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(87, 2), line);
#endif

    comet->H = QStringView{line}.mid(91, 4).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(91, 4), line);

    comet->G = QStringView{line}.mid(96, 4).toDouble(&ok);
    CHECK_AND_RETURN(ok, QStringView{line}.mid(96, 4), line);

    strcpy(comet->designation_name, line.mid(102, 56).trimmed().toStdString().c_str());

    return true;
}

bool MpcParser::start()
{
    QFile inputFile(m_filename);

    inputFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!inputFile.isOpen())
        return false;

    const auto sizeFile = inputFile.size();

    /* Skip header (size 2349) bytes when MPCORB.DAT. */
    if (m_orbType == OrbType::ASTEROID && sizeFile > MPCORB_HEADER_SIZE)
	inputFile.seek(MPCORB_HEADER_SIZE);

    QTextStream textStream(&inputFile);
    QString line;

    struct asteroid_t asteroid;
    struct comet_t comet;

    bool result;
    quint32 linesParsed = 0;

    while (!m_abort && textStream.readLineInto(&line)) {

	if (m_orbType == OrbType::ASTEROID && !parseAsteroid(line, &asteroid))
            continue;
	if (m_orbType == OrbType::COMET && !parseComet(line, &comet))
            continue;

        if (m_orbType == OrbType::ASTEROID) {
	    emit parsedAsteroid(asteroid);
	}
	if (m_orbType == OrbType::COMET) {
	    emit parsedComet(comet);
	};

        auto remains = inputFile.bytesAvailable();
        auto progress_ = ((sizeFile - remains) * 100.0) / sizeFile;

        emit progress(progress_, ++linesParsed);
    }

    inputFile.close();
    emit finished();

    return true;
}
