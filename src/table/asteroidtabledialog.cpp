#include "asteroidtabledialog.h"

AsteroidTableDialog::AsteroidTableDialog(QWidget *parent, SolarSystem *solarSystem) :
    TableDialog(parent, solarSystem)
{
    setSqlCmdQuery(QUERY_ASTEROID);
    setSqlCmdNumRow(QUERY_ASTEROID_NUM_ROWS);
    setOrbType(OrbType::ASTEROID);
    setLineEditText(QUERY_ASTEROID);
}

void AsteroidTableDialog::renderSelection()
{
    m_SolarSystem.removeAsteroids();

    QItemSelectionModel *selectionModel = m_TableView->selectionModel();
    if (selectionModel->hasSelection()) {

	struct asteroid_t asteroid;

	foreach(const QModelIndex &index, selectionModel->selectedRows()) {

	    memset(&asteroid, 0, sizeof(asteroid));

	    strncpy(asteroid.designation_readable, index.siblingAtColumn(0).data(Qt::DisplayRole)
		    .toString().toStdString().c_str(), sizeof(asteroid.designation_readable));
	    asteroid.eccentricity = index.siblingAtColumn(1).data(Qt::DisplayRole).toDouble();
	    asteroid.semimajor_axis = index.siblingAtColumn(2).data(Qt::DisplayRole).toDouble();
	    asteroid.inclination = index.siblingAtColumn(3).data(Qt::DisplayRole).toDouble();
	    asteroid.ascending_node = index.siblingAtColumn(4).data(Qt::DisplayRole).toDouble();
	    asteroid.perihelion = index.siblingAtColumn(5).data(Qt::DisplayRole).toDouble();
	    asteroid.anomaly_mean = index.siblingAtColumn(6).data(Qt::DisplayRole).toDouble();
	    asteroid.daily_motion = index.siblingAtColumn(7).data(Qt::DisplayRole).toDouble();
	    strncpy(asteroid.epoch, index.siblingAtColumn(8).data(Qt::DisplayRole)
		    .toString().toStdString().c_str(), sizeof(asteroid.epoch));
	    asteroid.classification_flag = index.siblingAtColumn(9).data(Qt::DisplayRole).toUInt();
	    strncpy(asteroid.designation, index.siblingAtColumn(10).data(Qt::DisplayRole)
		    .toString().toStdString().c_str(), sizeof(asteroid.designation));
	    asteroid.observations = index.siblingAtColumn(11).data(Qt::DisplayRole).toUInt();
 	    strncpy(asteroid.last_observation, index.siblingAtColumn(12).data(Qt::DisplayRole)
		    .toString().toStdString().c_str(), sizeof(asteroid.last_observation));
	    asteroid.H = index.siblingAtColumn(13).data(Qt::DisplayRole).toDouble();
	    asteroid.G = index.siblingAtColumn(14).data(Qt::DisplayRole).toDouble();

	    emit selectedAsteroid(asteroid);
	}
	emit displaySelectedAsteroids();
    }
}
