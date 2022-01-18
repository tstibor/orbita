#include "comettabledialog.h"

CometTableDialog::CometTableDialog(QWidget *parent, SolarSystem *solarSystem) :
    TableDialog(parent, solarSystem)
{
    setSqlCmdQuery(QUERY_COMET);
    setSqlCmdNumRow(QUERY_COMET_NUM_ROWS);
    setOrbType(OrbType::COMET);
    setLineEditText(QUERY_COMET);
}

void CometTableDialog::renderSelection()
{
    m_SolarSystem.removeComets();

    QItemSelectionModel *selectionModel = m_TableView->selectionModel();
    if (selectionModel->hasSelection()) {

	struct comet_t comet;

	foreach(const QModelIndex &index, selectionModel->selectedRows()) {

	    memset(&comet, 0, sizeof(comet));

	    strncpy(comet.designation_name, index.siblingAtColumn(0).data(Qt::DisplayRole)
		    .toString().toStdString().c_str(), sizeof(comet.designation_name));
	    comet.eccentricity        = index.siblingAtColumn(1).data(Qt::DisplayRole).toDouble();
	    comet.inclination         = index.siblingAtColumn(2).data(Qt::DisplayRole).toDouble();
	    comet.ascending_node      = index.siblingAtColumn(3).data(Qt::DisplayRole).toDouble();
	    comet.perihelion          = index.siblingAtColumn(4).data(Qt::DisplayRole).toDouble();
	    comet.perihelion_year     = index.siblingAtColumn(5).data(Qt::DisplayRole).toUInt();
	    comet.perihelion_month    = index.siblingAtColumn(6).data(Qt::DisplayRole).toUInt();
	    comet.perihelion_day      = index.siblingAtColumn(7).data(Qt::DisplayRole).toDouble();
	    comet.perihelion_distance = index.siblingAtColumn(8).data(Qt::DisplayRole).toDouble();
	    strncpy(comet.orbit_type, index.siblingAtColumn(9).data(Qt::DisplayRole)
		    .toString().toStdString().c_str(), sizeof(comet.orbit_type));
	    comet.H = index.siblingAtColumn(10).data(Qt::DisplayRole).toDouble();
	    comet.G = index.siblingAtColumn(11).data(Qt::DisplayRole).toDouble();

	    emit selectedComet(comet);
	}
	emit displaySelectedComets();
    }
}
