#include "tabledialog.h"

TableDialog::TableDialog(QWidget *parent, SolarSystem *solarSystem)
    : QDialog(parent), m_TableView(nullptr), m_SqlQueryModel(nullptr),
      m_SolarSystem(*solarSystem), m_totalObjects(0), m_OrbType(OrbType::UNKNOWN)
{
    m_VBoxLayout = new QVBoxLayout();
    m_TableView = new QTableView();
    m_HBoxLayout = new QHBoxLayout();

    m_PushButtonDisplay = new QPushButton(tr("Display"), this);
    m_PushButtonClear = new QPushButton(tr("Clear"), this);
    m_PushButtonExecQuery = new QPushButton(tr("Execute query"), this);
    m_LineEdit = new QLineEdit(this);

    m_HBoxLayout->addWidget(m_PushButtonDisplay);
    m_HBoxLayout->addWidget(m_PushButtonClear);
    m_HBoxLayout->addWidget(m_PushButtonExecQuery);

    m_VBoxLayout->addWidget(m_TableView);
    m_VBoxLayout->addLayout(m_HBoxLayout);
    m_VBoxLayout->addWidget(m_LineEdit);

    setLayout(m_VBoxLayout);

    m_TableView->setSizeAdjustPolicy(QTableView::AdjustToContents);
    m_TableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_TableView->installEventFilter(this);
    m_LineEdit->installEventFilter(this);

    m_TableView->setSortingEnabled(false);

    createConnections();

    updateWindowTitle();
}

TableDialog::~TableDialog()
{
    if (m_SqlQueryModel) {
	delete m_SqlQueryModel;
	m_SqlQueryModel = nullptr;
    }
}

void TableDialog::createConnections()
{
    connect(m_PushButtonDisplay, &QPushButton::clicked, this, &TableDialog::clickedButtonDisplay);
    connect(m_PushButtonClear, &QPushButton::clicked, [&]() {
	m_TableView->clearSelection();
	updateWindowTitle();
	renderSelection();
    });

    connect(m_PushButtonExecQuery, &QPushButton::clicked, this, &TableDialog::clickedButtonExecQuery);
}

void TableDialog::showQueryInTable()
{
    Sqlite connection(Settings::FilenameDb());
    QSqlDatabase db = connection.getDb();

    if (!db.isOpen()) {
	QMessageBox::critical(this, tr("Database error"),
			      tr("Cannot open sqlite database"));
        return;
    }

    if (m_SqlQueryModel == nullptr)
	m_SqlQueryModel = new QSqlQueryModel(m_TableView);

    QSqlQuery queryNumRows(m_SqlCmdNumRow, db);
    queryNumRows.exec();
    if (queryNumRows.first())
	m_totalObjects = queryNumRows.value(0).toUInt();

    m_SqlQueryModel->setQuery(QSqlQuery(m_SqlCmdQuery, db));
    /* Force fetching of the entire result set for
       databases that don't report back the size of a query. */
    while (m_SqlQueryModel->canFetchMore())
	m_SqlQueryModel->fetchMore();
    m_TableView->setModel(m_SqlQueryModel);

    updateWindowTitle();

    /* Note: we need a non nullptr m_TableView->selectionModel(), thus connect
       signal and slot here and not in createConnections(). */
    connect(m_TableView->selectionModel(), &QItemSelectionModel::selectionChanged, [&]()
    {
	QItemSelectionModel *selectionModel = m_TableView->selectionModel();
	if (selectionModel->hasSelection())
	    updateWindowTitle(selectionModel->selectedRows().size());
    });

    m_TableView->show();
    show();
}

void TableDialog::reject()
{
}

void TableDialog::closeEvent(QCloseEvent *event)
{
    m_TableView->clearSelection();
    updateWindowTitle();
    renderSelection();
}

bool TableDialog::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_LineEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
	if (keyEvent->key() == Qt::Key_Return) {
	    clickedButtonExecQuery();
	    updateWindowTitle();
	}
    }

    return QObject::eventFilter(object, event);
}

void TableDialog::clickedButtonDisplay()
{
    renderSelection();
}

void TableDialog::clickedButtonExecQuery()
{
    m_SqlCmdQuery = m_LineEdit->text();
    showQueryInTable();
}

void TableDialog::updateWindowTitle(quint32 numSelectedObjects)
{
    const QString celestialType = m_OrbType == OrbType::ASTEROID ?
	tr("asteroids") : m_OrbType == OrbType::COMET ? tr("comets") : "";

    setWindowTitle(tr("Number of selected")
		   + " "
		   + celestialType
		   + " "
		   + QString::number(numSelectedObjects)
		   + " / "
		   + QString::number(m_totalObjects));
}
