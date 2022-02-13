#include "FilterDialog.h"
#include "ui_FilterDialog.h"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>

#include <stdexcept>

#include "MainWindow.h"

#include <geolib/filter/Advanced.h>
#include <geolib/filter/Combined.h>
#include <geolib/filter/MatchAll.h>
#include <geolib/filter/CacheName.h>
#include <geolib/filter/Difficulty.h>
#include <geolib/filter/Distance.h>
#include <geolib/filter/Found.h>
#include <geolib/filter/GeocacheType.h>
#include <geolib/filter/HasCorrectedOrFinal.h>
#include <geolib/filter/DoesNotHaveCorrectedOrFinal.h>
#include <geolib/filter/MatchAll.h>
#include <geolib/filter/MyOwn.h>
#include <geolib/filter/MyNotOwn.h>
#include <geolib/filter/NotFound.h>
#include <geolib/filter/PlacedBy.h>
#include <geolib/filter/Size.h>
#include <geolib/filter/StatusArchived.h>
#include <geolib/filter/StatusAvailable.h>
#include <geolib/filter/StatusDisabled.h>
#include <geolib/filter/Terrain.h>
#include <geolib/filter/WaypointType.h>

#include <geolib/ReferencePoint.h>

#include <config/AppConfig.h>

#include <util/File.h>

#include "../forms/AdvancedFilterModifyDialog.h"

using namespace GCM;
using namespace GCM::config;
using namespace GCM::geolib;
using namespace GCM::geolib::filter;

class FilterListItem: public QListWidgetItem {
public:
	FilterListItem(const QString &text, GCM::GC<Filter> f, AdvancedFilterItem::Mode mode, QListWidget *parent = 0, int type = Type):
			QListWidgetItem(text, parent, type), mFilter(f) {

		this->setMode(mode);
	}

	GCM::GC<Filter> filter() {
		return this->mFilter;
	}

	AdvancedFilterItem::Mode mode() {
		return this->mMode;
	}

	void setFilter(GCM::GC<Filter> filter) {
		this->mFilter = filter;
	}

	void setMode(AdvancedFilterItem::Mode mode) {
		this->mMode = mode;

		switch (mode) {
		case AdvancedFilterItem::Initial:
			this->setIcon(QIcon(":/ic_af_load.png"));
			break;

		case AdvancedFilterItem::Add:
			this->setIcon(QIcon(":/ic_af_add.png"));
			break;

		case AdvancedFilterItem::Subtract:
			this->setIcon(QIcon(":/ic_af_sub.png"));
			break;

		case AdvancedFilterItem::Intersect:
			this->setIcon(QIcon(":/ic_af_intersect.png"));
			break;
		}
	}

private:
	GCM::GC<Filter> mFilter;
	AdvancedFilterItem::Mode mMode;
};

FilterDialog::FilterDialog(QWidget *parent, GCM::GC<Filter> f) :
	QDialog(parent),
	ui(new Ui::FilterDialog),
	filterSaved(true),
	filterFile(NULL),
	appendMode(MainWindow::Replace)
{
	ui->setupUi(this);

	// Hide advanced section
	this->ui->containerAdvanced->setVisible(false);
	this->ui->verticalLine->setVisible(false);

	this->ui->cbHasCorrected->setTristate(true);

	connect(this->ui->btnAdvanced, SIGNAL(clicked()), this, SLOT(onBtnAdvancedClick()));
	connect(this->ui->btnCancel, SIGNAL(clicked()), this, SLOT(onBtnCancelClick()));
	connect(this->ui->btnApply, SIGNAL(clicked()), this, SLOT(onBtnApplyClick()));
	connect(this, SIGNAL(accepted()), this, SLOT(onAccepted()));

	connect(this->ui->difficultyFrom, SIGNAL(valueChanged(int)), this, SLOT(onDiffFromChanged(int)));
	connect(this->ui->difficultyTo, SIGNAL(valueChanged(int)), this, SLOT(onDiffToChanged(int)));
	connect(this->ui->terrainFrom, SIGNAL(valueChanged(int)), this, SLOT(onTerrFromChanged(int)));
	connect(this->ui->terrainTo, SIGNAL(valueChanged(int)), this, SLOT(onTerrToChanged(int)));

	connect(this->ui->btnCacheAll, SIGNAL(clicked()), this, SLOT(onBtnCacheAllClicked()));
	connect(this->ui->btnCacheNone, SIGNAL(clicked()), this, SLOT(onBtnCacheNoneClicked()));
	connect(this->ui->btnCacheInvert, SIGNAL(clicked()), this, SLOT(onBtnCacheInvertClicked()));

	connect(this->ui->btnWaypointAll, SIGNAL(clicked()), this, SLOT(onBtnWaypointAllClicked()));
	connect(this->ui->btnWaypointNone, SIGNAL(clicked()), this, SLOT(onBtnWaypointNoneClicked()));
	connect(this->ui->btnWaypointInvert, SIGNAL(clicked()), this, SLOT(onBtnWaypointInvertClicked()));

	connect(this->ui->btnSave, SIGNAL(clicked()), this, SLOT(onBtnSaveClick()));
	connect(this->ui->btnLoad, SIGNAL(clicked()), this, SLOT(onBtnLoadClick()));

	connect(this->ui->advancedList, SIGNAL(currentRowChanged(int)), this, SLOT(onAdvancedRowChanged(int)));
	connect(this->ui->btnAdd, SIGNAL(clicked()), this, SLOT(onBtnAddClick()));
	connect(this->ui->btnRemove, SIGNAL(clicked()), this, SLOT(onBtnRemoveClick()));

	// Advanced filter popup menu actions
	QAction *action;

	action = new QAction("Modify...", NULL);
	this->popupActions.append(action);
	connect(action, SIGNAL(triggered()), this, SLOT(onAfModifyClick()));

	action = new QAction(NULL);
	action->setSeparator(true);
	this->popupActions.append(action);

	action = actionAfAdd = new QAction(QIcon(":/ic_af_add.png"), "Add", NULL);
	this->popupActions.append(action);
	connect(action, SIGNAL(triggered()), this, SLOT(onAfAddClick()));

	action = actionAfSubtract = new QAction(QIcon(":/ic_af_sub.png"), "Subtract", NULL);
	this->popupActions.append(action);
	connect(action, SIGNAL(triggered()), this, SLOT(onAfSubtractClick()));

	action = actionAfIntersect = new QAction(QIcon(":/ic_af_intersect.png"), "Intersect", NULL);
	this->popupActions.append(action);
	connect(action, SIGNAL(triggered()), this, SLOT(onAfIntersectClick()));

	connect(this->ui->advancedList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showAfContextMenu(QPoint)));

	this->adjustSize();

	this->ui->advancedList->clear();

	this->ui->containerAdvanced->setVisible(false);
	this->ui->verticalLine->setVisible(false);
	this->ui->btnAdvanced->setText("Advanced...");

	ArrayList<ReferencePoint>::iterator i = AppConfig::getInstance()->getReferencePoints()->begin();
	while (i != AppConfig::getInstance()->getReferencePoints()->end()) {
		this->ui->refPointSelect->addItem(QString::fromUtf8((*i)->getName()->c_str()));
		i++;
	}

	this->loadIdentity();

	if (f) {
		if (f->getId() != Advanced::ID) {
			FilterListItem *item = new FilterListItem("Initial filter", this->buildFilter(), AdvancedFilterItem::Initial, this->ui->advancedList);
			this->ui->advancedList->addItem(item);
			this->lastAdvancedRow = 0;
			this->ui->advancedList->setCurrentRow(0);
		}

		this->loadFilter(f);
	} else {
		FilterListItem *item = new FilterListItem("Initial filter", filter, AdvancedFilterItem::Initial, this->ui->advancedList);
		this->ui->advancedList->addItem(item);
		this->lastAdvancedRow = 0;
		this->ui->advancedList->setCurrentRow(0);
	}

	this->applyMenu = new QMenu(this->ui->btnApplyAdv);

	this->actionMenuAdd = new QAction(QIcon(":/ic_af_add.png"), "Add", NULL);
	connect(this->actionMenuAdd, SIGNAL(triggered()), this, SLOT(onApplyAdd()));

	this->actionMenuSub = new QAction(QIcon(":/ic_af_sub.png"), "Subtract", NULL);
	connect(this->actionMenuSub, SIGNAL(triggered()), this, SLOT(onApplySub()));

	this->actionMenuIntersect = new QAction(QIcon(":/ic_af_intersect.png"), "Intersect", NULL);
	connect(this->actionMenuIntersect, SIGNAL(triggered()), this, SLOT(onApplyIntersect()));

	this->applyMenu->addAction(this->actionMenuAdd);
	this->applyMenu->addAction(this->actionMenuSub);
	this->applyMenu->addAction(this->actionMenuIntersect);

	this->ui->btnApplyAdv->setMenu(this->applyMenu);

	// Connect modified signals to reset saved state.
	connect(this->ui->cacheCito, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cacheEarth, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cacheEvent, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cacheLetterbox, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cacheMegaEvent, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cacheMulti, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cacheMystery, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cacheName, SIGNAL(textEdited(QString)), this, SLOT(anythingChanged()));
	connect(this->ui->cacheTraditional, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cacheVirtual, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cacheWebcam, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cacheWherigo, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cbArchived, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cbAvailable, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cbDisabled, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cbFound, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cbHasCorrected, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cbNotFound, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cbNotOwn, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->cbOwn, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->comboCacheNameMatch, SIGNAL(currentIndexChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->difficultyFrom, SIGNAL(valueChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->difficultyTo, SIGNAL(valueChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->placedBy, SIGNAL(textEdited(QString)), this, SLOT(anythingChanged()));
	connect(this->ui->sizeLarge, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->sizeMicro, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->sizeNotChosen, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->sizeOther, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->sizeRegular, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->sizeSmall, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->sizeVirtual, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->terrainFrom, SIGNAL(valueChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->terrainTo, SIGNAL(valueChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->waypointFinal, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->waypointParking, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->waypointQta, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->waypointReference, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->waypointSom, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->waypointTrailhead, SIGNAL(stateChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->referencePoint, SIGNAL(toggled(bool)), this, SLOT(anythingChanged()));
	connect(this->ui->refPointDistance, SIGNAL(valueChanged(int)), this, SLOT(anythingChanged()));
	connect(this->ui->refPointSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(anythingChanged()));
}

FilterDialog::~FilterDialog()
{
	delete ui;

	// Delete the actions
	QList<QAction *>::iterator i = this->popupActions.begin();
	while (i != this->popupActions.end()) {
		delete (*i);
		i++;
	}

	delete this->applyMenu;
	delete this->actionMenuAdd;
	delete this->actionMenuIntersect;
	delete this->actionMenuSub;
}

void FilterDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void FilterDialog::loadIdentity() {
	// Load identity filter...
	this->ui->comboCacheNameMatch->setCurrentIndex(0);
	this->ui->cacheName->setText("");

	this->ui->placedBy->setText("");

	this->ui->difficultyFrom->setValue(1);
	this->ui->difficultyTo->setValue(9);

	this->ui->terrainFrom->setValue(1);
	this->ui->terrainTo->setValue(9);

	this->ui->cbAvailable->setChecked(true);
	this->ui->cbDisabled->setChecked(true);
	this->ui->cbArchived->setChecked(true);

	this->ui->cbFound->setChecked(true);
	this->ui->cbNotFound->setChecked(true);
	this->ui->cbOwn->setChecked(true);
	this->ui->cbNotOwn->setChecked(true);

	this->ui->cbHasCorrected->setCheckState(Qt::PartiallyChecked);

	// Type page
	this->ui->cacheTraditional->setChecked(true);
	this->ui->cacheMulti->setChecked(true);
	this->ui->cacheMystery->setChecked(true);
	this->ui->cacheLetterbox->setChecked(true);
	this->ui->cacheEvent->setChecked(true);
	this->ui->cacheMegaEvent->setChecked(true);
	this->ui->cacheCito->setChecked(true);
	this->ui->cacheWherigo->setChecked(true);
	this->ui->cacheEarth->setChecked(true);
	this->ui->cacheVirtual->setChecked(true);
	this->ui->cacheWebcam->setChecked(true);

	this->ui->waypointFinal->setChecked(true);
	this->ui->waypointQta->setChecked(true);
	this->ui->waypointSom->setChecked(true);
	this->ui->waypointReference->setChecked(true);
	this->ui->waypointParking->setChecked(true);
	this->ui->waypointTrailhead->setChecked(true);

	// Advanced attributes
	this->ui->sizeMicro->setChecked(true);
	this->ui->sizeSmall->setChecked(true);
	this->ui->sizeRegular->setChecked(true);
	this->ui->sizeLarge->setChecked(true);
	this->ui->sizeNotChosen->setChecked(true);
	this->ui->sizeOther->setChecked(true);
	this->ui->sizeVirtual->setChecked(true);

	// Distance tab
	this->ui->referencePoint->setChecked(false);
	this->ui->refPointSelect->setCurrentIndex(0);
	this->ui->refPointDistance->setValue(0);
}

void FilterDialog::onBtnAdvancedClick() {
	if (this->ui->containerAdvanced->isVisible()) {
		// Discard all filters...
		QMessageBox msg;
		msg.setText("Do you really want to discard advanced filter settings and proceed with simple configuration?");
		msg.setIcon(QMessageBox::Question);
		msg.setWindowTitle("Discard?");
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

		if (msg.exec() == QMessageBox::Yes) {
			this->ui->advancedList->clear();
			FilterListItem *item = new FilterListItem("Initial filter", this->buildFilter(), AdvancedFilterItem::Initial, this->ui->advancedList);
			this->ui->advancedList->addItem(item);
			this->lastAdvancedRow = 0;

			this->ui->btnAdvanced->setText("Advanced...");
			this->ui->containerAdvanced->setVisible(false);
			this->ui->verticalLine->setVisible(false);
			this->adjustSize();
		}
	} else {
		this->ui->btnAdvanced->setText("Simple");
		this->ui->containerAdvanced->setVisible(true);
		this->ui->verticalLine->setVisible(true);
		this->adjustSize();
	}
}

void FilterDialog::onBtnCancelClick() {
	emit reject();
}

void FilterDialog::onBtnApplyClick() {
	FilterListItem *item = (FilterListItem *)this->ui->advancedList->item(this->lastAdvancedRow);
	item->setFilter(this->buildFilter());

	//if (!this->filterSaved || this->filterFile) {
		emit accept();
	//} else {
		// Nothing changed, so do not force system to reload caches...
	//	emit reject();
	//}
}

void FilterDialog::onApplyAdd() {
	this->appendMode = MainWindow::Add;
	this->onBtnApplyClick();
}

void FilterDialog::onApplySub() {
	this->appendMode = MainWindow::Subtract;
	this->onBtnApplyClick();
}

void FilterDialog::onApplyIntersect() {
	this->appendMode = MainWindow::Intersect;
	this->onBtnApplyClick();
}

MainWindow::AppendMode FilterDialog::getAppendMode() {
	return this->appendMode;
}

GCM::GC<Filter> FilterDialog::constructFilter() {
	// Classic filter
	if (this->ui->advancedList->count() == 1) {
		GCM::GC<Filter> out = this->buildFilter();
		return out;

	// Advanced filter
	} else {
		Advanced *fAdvanced = new Advanced();
		GCM::GC<Filter> out = fAdvanced;
		for (int i = 0; i < this->ui->advancedList->count(); i++) {
			FilterListItem *item = (FilterListItem *)this->ui->advancedList->item(i);
			this->loadIdentity();
			this->loadFilter(item->filter());
			fAdvanced->push_back(AdvancedFilterItem(item->text().toUtf8().data(), item->mode(), this->buildFilter()));
	   }

		return out;
	}
}

void FilterDialog::onAccepted() {
	this->filter = this->constructFilter();
}

// Build filter from settings
GCM::GC<Filter> FilterDialog::buildFilter() {
	Combined *out = new Combined();

	// Cache name
	if (this->ui->cacheName->text() != "") {
		CacheName *fCacheName = new CacheName();
		switch (this->ui->comboCacheNameMatch->currentIndex()) {
		case 0: // Contains
			fCacheName->setMatchMode(CacheName::MM_CONTAINS);
			break;

		case 1: // Does not contains
			fCacheName->setMatchMode(CacheName::MM_DN_CONTAINS);
			break;

		case 2: // Begins with
			fCacheName->setMatchMode(CacheName::MM_BEGINS);
			break;

		case 3: // Ends with
			fCacheName->setMatchMode(CacheName::MM_ENDS);
			break;

		case 4: // Equals
			fCacheName->setMatchMode(CacheName::MM_EQUALS);
			break;

		default:
			fCacheName->setMatchMode(CacheName::MM_IGNORED);
			break;
		}

		if (fCacheName->getMatchMode() == CacheName::MM_IGNORED) {
			delete fCacheName;
		} else {
			fCacheName->setCacheName(this->ui->cacheName->text().toLatin1().data());
			out->append(fCacheName);
		}
	}

	// Placed by
	if (this->ui->placedBy->text() != "") {
		PlacedBy *fPlacedBy = new PlacedBy();
		fPlacedBy->setPlacedBy(this->ui->placedBy->text().toLatin1().data());
		out->append(fPlacedBy);
	}

	// Difficulty
	if (this->ui->difficultyFrom->value() > 1 || this->ui->difficultyTo->value() < 9) {
		Difficulty *fDifficulty = new Difficulty();
		for (int v = this->ui->difficultyFrom->value() * 5 + 5; v <= this->ui->difficultyTo->value() * 5 + 5; v += 5) {
			fDifficulty->include(v);
		}
		out->append(fDifficulty);
	}

	// Terrrain
	if (this->ui->terrainFrom->value() > 1 || this->ui->terrainTo->value() < 9) {
		Terrain *fTerrain = new Terrain();
		for (int v = this->ui->terrainFrom->value() * 5 + 5; v <= this->ui->terrainTo->value() * 5 + 5; v += 5) {
			fTerrain->include(v);
		}
		out->append(fTerrain);
	}

	// Status
	bool showAvailable = this->ui->cbAvailable->checkState() == Qt::Checked;
	bool showDisabled = this->ui->cbDisabled->checkState() == Qt::Checked;
	bool showArchived = this->ui->cbArchived->checkState() == Qt::Checked;

	// If at least one is checked
	if (showAvailable || showDisabled || showArchived) {
		if (!showAvailable) {
			out->append(new StatusAvailable());
		}

		if (!showDisabled) {
			out->append(new StatusDisabled());
		}

		if (!showArchived) {
			out->append(new StatusArchived());
		}
	}

	// Found, not found, own
	bool showFound = this->ui->cbFound->checkState() == Qt::Checked;
	bool showNotFound = this->ui->cbNotFound->checkState() == Qt::Checked;
	bool showOwn = this->ui->cbOwn->checkState() == Qt::Checked;
	bool showNotOwn = this->ui->cbNotOwn->checkState() == Qt::Checked;

	if (showFound && !showNotFound) {
		out->append(new Found());
	} else if (!showFound && showNotFound) {
		out->append(new NotFound());
	}

	if (showOwn && !showNotOwn) {
		out->append(new MyOwn());
	} else if (!showOwn && showNotOwn) {
		out->append(new MyNotOwn());
	}

	// Corrected coords
	if (this->ui->cbHasCorrected->checkState() == Qt::Unchecked) {
		out->append(new HasCorrectedOrFinal());
	} else if (this->ui->cbHasCorrected->checkState() == Qt::Checked) {
		out->append(new DoesNotHaveCorrectedOrFinal());
	}

	// Cache types
	bool cacheTraditional = this->ui->cacheTraditional->checkState() == Qt::Checked;
	bool cacheMulti = this->ui->cacheMulti->checkState() == Qt::Checked;
	bool cacheMystery = this->ui->cacheMystery->checkState() == Qt::Checked;
	bool cacheLetterbox = this->ui->cacheLetterbox->checkState() == Qt::Checked;
	bool cacheEvent = this->ui->cacheEvent->checkState() == Qt::Checked;
	bool cacheMegaEvent = this->ui->cacheMegaEvent->checkState() == Qt::Checked;
	bool cacheCito = this->ui->cacheCito->checkState() == Qt::Checked;
	bool cacheWherigo = this->ui->cacheWherigo->checkState() == Qt::Checked;
	bool cacheEarth = this->ui->cacheEarth->checkState() == Qt::Checked;
	bool cacheVirtual = this->ui->cacheVirtual->checkState() == Qt::Checked;
	bool cacheWebcam = this->ui->cacheWebcam->checkState() == Qt::Checked;

	bool allChecked = cacheTraditional && cacheMulti && cacheMystery && cacheLetterbox &&
		cacheEvent && cacheMegaEvent && cacheCito && cacheWherigo && cacheEarth && cacheVirtual && cacheWebcam;

	// If at least one is checked and not all of them are checked.
	if (!allChecked && (cacheTraditional || cacheMulti || cacheMystery || cacheLetterbox || cacheEvent ||
		cacheMegaEvent || cacheCito || cacheWherigo || cacheEarth || cacheVirtual || cacheWebcam)) {

		GeocacheType *fGcType = new GeocacheType();
		if (cacheTraditional) fGcType->include(Geocache::T_TRADITIONAL);
		if (cacheMulti) fGcType->include(Geocache::T_MULTI);
		if (cacheMystery) fGcType->include(Geocache::T_UNKNOWN);
		if (cacheLetterbox) fGcType->include(Geocache::T_LETTERBOX);
		if (cacheEvent) fGcType->include(Geocache::T_EVENT);
		if (cacheMegaEvent) fGcType->include(Geocache::T_MEGAEVENT);
		if (cacheCito) fGcType->include(Geocache::T_CITO);
		if (cacheWherigo) fGcType->include(Geocache::T_WHERIGO);
		if (cacheEarth) fGcType->include(Geocache::T_EARTH);
		if (cacheVirtual) fGcType->include(Geocache::T_VIRTUAL);
		if (cacheWebcam) fGcType->include(Geocache::T_WEBCAM);

		out->append(fGcType);
	}

	// Waypoint types
	bool waypointFinal = this->ui->waypointFinal->checkState() == Qt::Checked;
	bool waypointQta = this->ui->waypointQta->checkState() == Qt::Checked;
	bool waypointSom = this->ui->waypointSom->checkState() == Qt::Checked;
	bool waypointReference = this->ui->waypointReference->checkState() == Qt::Checked;
	bool waypointParking = this->ui->waypointParking->checkState() == Qt::Checked;
	bool waypointTrailhead = this->ui->waypointTrailhead->checkState() == Qt::Checked;

	allChecked = waypointFinal && waypointQta && waypointSom && waypointReference && waypointParking && waypointTrailhead;
	if (!allChecked && (waypointFinal || waypointQta || waypointSom || waypointReference || waypointParking || waypointTrailhead)) {
		WaypointType *fWptType = new WaypointType();
		if (waypointFinal) fWptType->include(GeocacheWaypoint::T_FINAL);
		if (waypointQta) fWptType->include(GeocacheWaypoint::T_QTA);
		if (waypointSom) fWptType->include(GeocacheWaypoint::T_SOM);
		if (waypointReference) fWptType->include(GeocacheWaypoint::T_REFERENCE);
		if (waypointParking) fWptType->include(GeocacheWaypoint::T_PARKING);
		if (waypointTrailhead) fWptType->include(GeocacheWaypoint::T_TRAILHEAD);

		out->append(fWptType);
	}

	// Cache size
	bool sizeMicro = this->ui->sizeMicro->checkState() == Qt::Checked;
	bool sizeSmall = this->ui->sizeSmall->checkState() == Qt::Checked;
	bool sizeRegular = this->ui->sizeRegular->checkState() == Qt::Checked;
	bool sizeLarge = this->ui->sizeLarge->checkState() == Qt::Checked;
	bool sizeOther = this->ui->sizeOther->checkState() == Qt::Checked;
	bool sizeNotChosen = this->ui->sizeNotChosen->checkState() == Qt::Checked;
	bool sizeVirtual = this->ui->sizeVirtual->checkState() == Qt::Checked;

	allChecked = sizeMicro && sizeSmall && sizeRegular && sizeLarge && sizeOther && sizeNotChosen && sizeVirtual;
	if (!allChecked && (sizeMicro || sizeSmall || sizeRegular || sizeLarge || sizeOther || sizeNotChosen || sizeVirtual)) {
		Size *fSize = new Size();
		if (sizeMicro) fSize->include(Geocache::S_MICRO);
		if (sizeSmall) fSize->include(Geocache::S_SMALL);
		if (sizeRegular) fSize->include(Geocache::S_REGULAR);
		if (sizeLarge) fSize->include(Geocache::S_LARGE);
		if (sizeOther) fSize->include(Geocache::S_OTHER);
		if (sizeNotChosen) fSize->include(Geocache::S_NOT_LISTED);
		if (sizeVirtual) fSize->include(Geocache::S_VIRTUAL);

		out->append(fSize);
	}

	// Distance
	if (this->ui->referencePoint->isChecked() && this->ui->refPointDistance->value() > 0) {
		Distance *fDistance = new Distance();
		if (this->ui->refPointSelect->currentIndex() == 0) {
			fDistance->setUseCacheDistance(true);
		} else {
			fDistance->setUseCacheDistance(false);
			try {
				GCM::GC<ReferencePoint> refPt = AppConfig::getInstance()->getReferencePoints()->at(this->ui->refPointSelect->currentIndex() - 1);
				fDistance->setOrigin(refPt->getLatitudeE6(), refPt->getLongitudeE6());
			} catch (std::out_of_range) {
			}

		}
		fDistance->setDistance(this->ui->refPointDistance->value() * 1000);

		out->append(fDistance);
	}

	// Return built filter.
	return out;
}

void FilterDialog::onDiffFromChanged(int value) {
	if (this->ui->difficultyTo->value() < value) {
		this->ui->difficultyTo->setValue(value);
	}

	QString str;
	str.sprintf("(%1.*lf)", value % 2 == 0, value * 0.5 + 0.5);
	this->ui->labelDiffFrom->setText(str);
}

void FilterDialog::onDiffToChanged(int value) {
	if (this->ui->difficultyFrom->value() > value) {
		this->ui->difficultyFrom->setValue(value);
	}

	QString str;
	str.sprintf("(%1.*lf)", value % 2 == 0, value * 0.5 + 0.5);
	this->ui->labelDiffTo->setText(str);
}

void FilterDialog::onTerrFromChanged(int value) {
	if (this->ui->terrainTo->value() < value) {
		this->ui->terrainTo->setValue(value);
	}

	QString str;
	str.sprintf("(%1.*lf)", value % 2 == 0, value * 0.5 + 0.5);
	this->ui->labelTerrFrom->setText(str);
}

void FilterDialog::onTerrToChanged(int value) {
	if (this->ui->terrainFrom->value() > value) {
		this->ui->terrainFrom->setValue(value);
	}

	QString str;
	str.sprintf("(%1.*lf)", value % 2 == 0, value * 0.5 + 0.5);
	this->ui->labelTerrTo->setText(str);
}

void FilterDialog::onBtnCacheAllClicked() {
	this->ui->cacheTraditional->setChecked(true);
	this->ui->cacheMulti->setChecked(true);
	this->ui->cacheMystery->setChecked(true);
	this->ui->cacheLetterbox->setChecked(true);
	this->ui->cacheEvent->setChecked(true);
	this->ui->cacheMegaEvent->setChecked(true);
	this->ui->cacheCito->setChecked(true);
	this->ui->cacheWherigo->setChecked(true);
	this->ui->cacheEarth->setChecked(true);
	this->ui->cacheVirtual->setChecked(true);
	this->ui->cacheWebcam->setChecked(true);
}

void FilterDialog::onBtnCacheInvertClicked() {
	this->ui->cacheTraditional->setChecked(this->ui->cacheTraditional->checkState() == Qt::Unchecked);
	this->ui->cacheMulti->setChecked(this->ui->cacheMulti->checkState() == Qt::Unchecked);
	this->ui->cacheMystery->setChecked(this->ui->cacheMystery->checkState() == Qt::Unchecked);
	this->ui->cacheLetterbox->setChecked(this->ui->cacheLetterbox->checkState() == Qt::Unchecked);
	this->ui->cacheEvent->setChecked(this->ui->cacheEvent->checkState() == Qt::Unchecked);
	this->ui->cacheMegaEvent->setChecked(this->ui->cacheMegaEvent->checkState() == Qt::Unchecked);
	this->ui->cacheCito->setChecked(this->ui->cacheCito->checkState() == Qt::Unchecked);
	this->ui->cacheWherigo->setChecked(this->ui->cacheWherigo->checkState() == Qt::Unchecked);
	this->ui->cacheEarth->setChecked(this->ui->cacheEarth->checkState() == Qt::Unchecked);
	this->ui->cacheVirtual->setChecked(this->ui->cacheVirtual->checkState() == Qt::Unchecked);
	this->ui->cacheWebcam->setChecked(this->ui->cacheWebcam->checkState() == Qt::Unchecked);
}

void FilterDialog::onBtnCacheNoneClicked() {
	this->ui->cacheTraditional->setChecked(false);
	this->ui->cacheMulti->setChecked(false);
	this->ui->cacheMystery->setChecked(false);
	this->ui->cacheLetterbox->setChecked(false);
	this->ui->cacheEvent->setChecked(false);
	this->ui->cacheMegaEvent->setChecked(false);
	this->ui->cacheCito->setChecked(false);
	this->ui->cacheWherigo->setChecked(false);
	this->ui->cacheEarth->setChecked(false);
	this->ui->cacheVirtual->setChecked(false);
	this->ui->cacheWebcam->setChecked(false);
}

void FilterDialog::onBtnWaypointAllClicked() {
	this->ui->waypointFinal->setChecked(true);
	this->ui->waypointQta->setChecked(true);
	this->ui->waypointSom->setChecked(true);
	this->ui->waypointReference->setChecked(true);
	this->ui->waypointParking->setChecked(true);
	this->ui->waypointTrailhead->setChecked(true);
}

void FilterDialog::onBtnWaypointInvertClicked() {
	this->ui->waypointFinal->setChecked(this->ui->waypointFinal->checkState() == Qt::Unchecked);
	this->ui->waypointQta->setChecked(this->ui->waypointQta->checkState() == Qt::Unchecked);
	this->ui->waypointSom->setChecked(this->ui->waypointSom->checkState() == Qt::Unchecked);
	this->ui->waypointReference->setChecked(this->ui->waypointReference->checkState() == Qt::Unchecked);
	this->ui->waypointParking->setChecked(this->ui->waypointParking->checkState() == Qt::Unchecked);
	this->ui->waypointTrailhead->setChecked(this->ui->waypointTrailhead->checkState() == Qt::Unchecked);
}

void FilterDialog::onBtnWaypointNoneClicked() {
	this->ui->waypointFinal->setChecked(false);
	this->ui->waypointQta->setChecked(false);
	this->ui->waypointSom->setChecked(false);
	this->ui->waypointReference->setChecked(false);
	this->ui->waypointParking->setChecked(false);
	this->ui->waypointTrailhead->setChecked(false);
}

GCM::GC<GCM::geolib::Filter> FilterDialog::getFilter() {
	return this->filter;
}

void FilterDialog::loadFilter(GCM::GC<GCM::geolib::Filter> f) {
	if (f->getId() == Advanced::ID) {
		this->ui->advancedList->clear();

		Advanced *fAdvanced = (Advanced *)f.getObj();
		Advanced::iterator i = fAdvanced->begin();
		while (i != fAdvanced->end()) {
			FilterListItem *item = new FilterListItem(
				QString::fromUtf8(i->getName()->c_str()),
				i->getFilter(),
				i->getMode(),
				this->ui->advancedList
			);
			this->ui->advancedList->addItem(item);
			i++;
		}

		// If there is at least one filter in the advanced set,
		// select the first one from the set as currently edited.
		if (this->ui->advancedList->count() > 0) {
			this->lastAdvancedRow = 0;
			this->ui->advancedList->setCurrentRow(this->lastAdvancedRow);

			FilterListItem *item = (FilterListItem *)this->ui->advancedList->item(this->lastAdvancedRow);
			this->loadFilter(item->filter());

			if (!this->ui->advancedList->isVisible()) {
				emit this->ui->btnAdvanced->click();
			}

		// If there is no filter in the advanced set, create the initial one.
		} else {
			this->loadIdentity();
			FilterListItem *item = new FilterListItem(QString("Initial filter"), this->buildFilter(), AdvancedFilterItem::Initial, this->ui->advancedList);
			this->ui->advancedList->addItem(item);
		}

	} else if (f->getId() == Combined::ID) {
		// Load combined filter...
		Combined *fCombined = (Combined *)f.getObj();
		Combined::iterator i = fCombined->begin();
		while (i != fCombined->end()) {
			this->loadFilter(*i);
			i++;
		}
	} else if (f->getId() == CacheName::ID) {
		CacheName *fCacheName = (CacheName *)f.getObj();
		this->ui->cacheName->setText(QString::fromUtf8(fCacheName->getCacheName()->c_str()));

		switch (fCacheName->getMatchMode()) {
		case CacheName::MM_CONTAINS: this->ui->comboCacheNameMatch->setCurrentIndex(0); break;
		case CacheName::MM_DN_CONTAINS: this->ui->comboCacheNameMatch->setCurrentIndex(1); break;
		case CacheName::MM_BEGINS: this->ui->comboCacheNameMatch->setCurrentIndex(2); break;
		case CacheName::MM_ENDS: this->ui->comboCacheNameMatch->setCurrentIndex(3); break;
		case CacheName::MM_EQUALS: this->ui->comboCacheNameMatch->setCurrentIndex(4); break;
		default: this->ui->cacheName->setText(""); break;
		}
	} else if (f->getId() == Difficulty::ID) {
		Difficulty *fDifficulty = (Difficulty *)f.getObj();

		// Get lowest difficulty
		int minDiff = 10;
		for (; minDiff <= 50; minDiff += 5) {
			if (fDifficulty->contains(minDiff)) break;
		}


		// Get highest difficulty
		int maxDiff = 50;
		for (; maxDiff >= 10; maxDiff -= 5) {
			if (fDifficulty->contains(maxDiff)) break;
		}

		this->ui->difficultyFrom->setValue(minDiff / 5 - 1);
		this->ui->difficultyTo->setValue(maxDiff / 5 - 1);
	} else if (f->getId() == Terrain::ID) {
		Terrain *fTerrain = (Terrain *)f.getObj();

		// Get lowest terrain
		int minTerr = 10;
		for (; minTerr <= 50; minTerr += 5) {
			if (fTerrain->contains(minTerr)) break;
		}


		// Get highest terrain
		int maxTerr = 50;
		for (; maxTerr >= 10; maxTerr -= 5) {
			if (fTerrain->contains(maxTerr)) break;
		}

		this->ui->terrainFrom->setValue(minTerr / 5 - 1);
		this->ui->terrainTo->setValue(maxTerr / 5 - 1);
	} else if (f->getId() == DoesNotHaveCorrectedOrFinal::ID) {
		this->ui->cbHasCorrected->setChecked(false);
	} else if (f->getId() == HasCorrectedOrFinal::ID) {
		this->ui->cbHasCorrected->setChecked(true);
	} else if (f->getId() == Found::ID) {
		this->ui->cbFound->setChecked(true);
		this->ui->cbNotFound->setChecked(false);
	} else if (f->getId() == NotFound::ID) {
		this->ui->cbFound->setChecked(false);
		this->ui->cbNotFound->setChecked(true);
	} else if (f->getId() == GeocacheType::ID) {
		GeocacheType *fGeocacheType = (GeocacheType *)f.getObj();

		if (!fGeocacheType->contains(Geocache::T_CITO)) this->ui->cacheCito->setChecked(false);
		if (!fGeocacheType->contains(Geocache::T_EARTH)) this->ui->cacheEarth->setChecked(false);
		if (!fGeocacheType->contains(Geocache::T_EVENT)) this->ui->cacheEvent->setChecked(false);
		if (!fGeocacheType->contains(Geocache::T_LETTERBOX)) this->ui->cacheLetterbox->setChecked(false);
		if (!fGeocacheType->contains(Geocache::T_MEGAEVENT)) this->ui->cacheMegaEvent->setChecked(false);
		if (!fGeocacheType->contains(Geocache::T_MULTI)) this->ui->cacheMulti->setChecked(false);
		if (!fGeocacheType->contains(Geocache::T_TRADITIONAL)) this->ui->cacheTraditional->setChecked(false);
		if (!fGeocacheType->contains(Geocache::T_UNKNOWN)) this->ui->cacheMystery->setChecked(false);
		if (!fGeocacheType->contains(Geocache::T_VIRTUAL)) this->ui->cacheVirtual->setChecked(false);
		if (!fGeocacheType->contains(Geocache::T_WEBCAM)) this->ui->cacheWebcam->setChecked(false);
		if (!fGeocacheType->contains(Geocache::T_WHERIGO)) this->ui->cacheWherigo->setChecked(false);
	} else if (f->getId() == WaypointType::ID) {
		WaypointType *fWaypointType = (WaypointType *)f.getObj();

		if (!fWaypointType->contains(GeocacheWaypoint::T_FINAL)) this->ui->waypointFinal->setChecked(false);
		if (!fWaypointType->contains(GeocacheWaypoint::T_PARKING)) this->ui->waypointParking->setChecked(false);
		if (!fWaypointType->contains(GeocacheWaypoint::T_QTA)) this->ui->waypointQta->setChecked(false);
		if (!fWaypointType->contains(GeocacheWaypoint::T_REFERENCE)) this->ui->waypointReference->setChecked(false);
		if (!fWaypointType->contains(GeocacheWaypoint::T_SOM)) this->ui->waypointSom->setChecked(false);
		if (!fWaypointType->contains(GeocacheWaypoint::T_TRAILHEAD)) this->ui->waypointTrailhead->setChecked(false);
	} else if (f->getId() == MyOwn::ID) {
		this->ui->cbOwn->setChecked(true);
		this->ui->cbNotOwn->setChecked(false);
	} else if (f->getId() == MyNotOwn::ID) {
		this->ui->cbOwn->setChecked(false);
		this->ui->cbNotOwn->setChecked(true);
	} else if (f->getId() == Size::ID) {
		Size *fSize = (Size *)f.getObj();
		if (!fSize->contains(Geocache::S_MICRO)) this->ui->sizeMicro->setChecked(false);
		if (!fSize->contains(Geocache::S_SMALL)) this->ui->sizeSmall->setChecked(false);
		if (!fSize->contains(Geocache::S_REGULAR)) this->ui->sizeRegular->setChecked(false);
		if (!fSize->contains(Geocache::S_LARGE)) this->ui->sizeLarge->setChecked(false);
		if (!fSize->contains(Geocache::S_OTHER)) this->ui->sizeOther->setChecked(false);
		if (!fSize->contains(Geocache::S_NOT_LISTED)) this->ui->sizeNotChosen->setChecked(false);
		if (!fSize->contains(Geocache::S_VIRTUAL)) this->ui->sizeVirtual->setChecked(false);
	} else if (f->getId() == PlacedBy::ID) {
		PlacedBy *fPlacedBy = (PlacedBy *)f.getObj();
		this->ui->placedBy->setText(QString::fromUtf8(fPlacedBy->getPlacedBy()->c_str()));
	} else if (f->getId() == StatusArchived::ID) {
		this->ui->cbArchived->setChecked(false);
	} else if (f->getId() == StatusAvailable::ID) {
		this->ui->cbAvailable->setChecked(false);
	} else if (f->getId() == StatusDisabled::ID) {
		this->ui->cbDisabled->setChecked(false);
	} else if (f->getId() == Distance::ID) {
		Distance *fDistance = (Distance *)f.getObj();
		this->ui->referencePoint->setChecked(true);
		this->ui->refPointDistance->setValue(fDistance->getDistance() / 1000);

		// Try to locate reference point...
		ArrayList<ReferencePoint>::iterator i = AppConfig::getInstance()->getReferencePoints()->begin();
		int index = 1;
		while (i != AppConfig::getInstance()->getReferencePoints()->end()) {
			if ((*i)->getLatitudeE6() == fDistance->getLatitudeE6() && (*i)->getLongitudeE6() == fDistance->getLongitudeE6()) {
				this->ui->refPointSelect->setCurrentIndex(index);
				break;
			}
			index++;
			i++;

		}

		if (i == AppConfig::getInstance()->getReferencePoints()->end()) {
			this->ui->refPointSelect->setCurrentIndex(0);
		}
	} else {
		// Unknown filter, ignore...
	}
}

void FilterDialog::onBtnLoadClick() {
	QString file = QFileDialog::getOpenFileName(this, "Load filter", QString(), "XML Filters (*.xml)", 0);
	if (file != "") {
		GCM::GC<Filter> filter = Filter::loadFromFile(file.toUtf8().data());
		if (filter) {
			this->loadIdentity();

			if (filter->getId() != Advanced::ID) {
				this->ui->advancedList->clear();

				this->ui->containerAdvanced->setVisible(false);
				this->ui->verticalLine->setVisible(false);
				this->adjustSize();

				this->ui->btnAdvanced->setText("Advanced...");

				FilterListItem *item = new FilterListItem("Initial filter", filter, AdvancedFilterItem::Initial, this->ui->advancedList);
				this->ui->advancedList->addItem(item);
				this->lastAdvancedRow = 0;
				this->ui->advancedList->setCurrentRow(0);

				this->loadFilter(filter);
			} else {
				this->loadFilter(filter);
			}

			this->filterSaved = true;
			this->filterFile = GCM::io::File(file.toUtf8().data());
		}
	}
}

void FilterDialog::onBtnSaveClick() {
	FilterListItem *item = (FilterListItem *)this->ui->advancedList->item(this->lastAdvancedRow);
	item->setFilter(this->buildFilter());

	QString file = QFileDialog::getSaveFileName(this, "Save filter", QString(), "XML Filters (*.xml)", 0);
	if (file != "") {
		if (!file.endsWith(".xml")) file.append(".xml");
		GCM::GC<Filter> filter = this->constructFilter();
		Filter::saveToFile(filter, file.toUtf8().data());

		this->filterSaved = true;
		this->filterFile = GCM::io::File(file.toUtf8().data());
	}
}

void FilterDialog::onAdvancedRowChanged(int row) {
	if (row != this->lastAdvancedRow) {
		if (row < 0) {
			// Nothing is selected, select back the old one
			this->ui->advancedList->setCurrentRow(this->lastAdvancedRow);
		} else {
			// Change in selection. Switch filters.

			// Save old filter
			FilterListItem *item = (FilterListItem *)this->ui->advancedList->item(this->lastAdvancedRow);
			item->setFilter(this->buildFilter());

			// Load new filter
			item = (FilterListItem *)this->ui->advancedList->item(row);
			this->loadIdentity();
			this->loadFilter(item->filter());

			this->lastAdvancedRow = row;
		}
	}
}

void FilterDialog::onBtnAddClick() {
	AdvancedFilterModifyDialog dlg(this);
	if (dlg.exec() == QDialog::Accepted) {
		// Save current filter to currently selected item.
		FilterListItem *item = (FilterListItem *)this->ui->advancedList->item(this->lastAdvancedRow);
		if (item) {
			item->setFilter(this->buildFilter());
		}

		this->loadIdentity();
		item = new FilterListItem(dlg.getFilterName(), this->buildFilter(), dlg.getFilterMode(), this->ui->advancedList);
		this->ui->advancedList->addItem(item);

		// Switch to the newly added row.
		this->lastAdvancedRow = this->ui->advancedList->count() - 1;
		this->ui->advancedList->setCurrentRow(this->lastAdvancedRow);
		this->anythingChanged();
	}
}

void FilterDialog::onBtnRemoveClick() {
	// Discard current filter.
	delete this->ui->advancedList->takeItem(this->lastAdvancedRow);

	if (this->ui->advancedList->count() == 0) {
		// Create new initial value
		this->loadIdentity();
		FilterListItem *initial = new FilterListItem("Initial filter", this->buildFilter(), AdvancedFilterItem::Initial, this->ui->advancedList);
		this->ui->advancedList->addItem(initial);
	} else {
		// Reset first filter type to initial just in case first item was deleted.
		((FilterListItem *)this->ui->advancedList->item(0))->setMode(AdvancedFilterItem::Initial);
	}

	if (this->lastAdvancedRow >= this->ui->advancedList->count()) {
		this->lastAdvancedRow = this->ui->advancedList->count() - 1;
	}

	// Select another filter.
	this->ui->advancedList->setCurrentRow(this->lastAdvancedRow);
	this->anythingChanged();
}

void FilterDialog::onAfModifyClick() {
	// Get current item
	FilterListItem *item = (FilterListItem *)this->ui->advancedList->item(this->lastAdvancedRow);

	AdvancedFilterModifyDialog dlg(this);
	dlg.setFilterName(item->text());
	dlg.setFilterMode(item->mode());

	if (dlg.exec() == QDialog::Accepted) {
		if (item->mode() != AdvancedFilterItem::Initial) {
			item->setMode(dlg.getFilterMode());
		}

		item->setText(dlg.getFilterName());
		this->anythingChanged();
	}
}

void FilterDialog::onAfAddClick() {
	FilterListItem *item = (FilterListItem *)this->ui->advancedList->item(this->lastAdvancedRow);
	if (item->mode() != AdvancedFilterItem::Initial) {
		item->setMode(AdvancedFilterItem::Add);
	}
	this->anythingChanged();
}

void FilterDialog::onAfSubtractClick() {
	FilterListItem *item = (FilterListItem *)this->ui->advancedList->item(this->lastAdvancedRow);
	if (item->mode() != AdvancedFilterItem::Initial) {
		item->setMode(AdvancedFilterItem::Subtract);
	}
	this->anythingChanged();
}

void FilterDialog::onAfIntersectClick() {
	FilterListItem *item = (FilterListItem *)this->ui->advancedList->item(this->lastAdvancedRow);
	if (item->mode() != AdvancedFilterItem::Initial) {
		item->setMode(AdvancedFilterItem::Intersect);
	}
	this->anythingChanged();
}

void FilterDialog::showAfContextMenu(QPoint position) {
	FilterListItem *item = (FilterListItem *)this->ui->advancedList->item(this->lastAdvancedRow);
	if (item->mode() == AdvancedFilterItem::Initial) {
		actionAfAdd->setEnabled(false);
		actionAfSubtract->setEnabled(false);
		actionAfIntersect->setEnabled(false);
	} else {
		actionAfAdd->setEnabled(true);
		actionAfSubtract->setEnabled(true);
		actionAfIntersect->setEnabled(true);
	}
	QMenu::exec(this->popupActions, this->ui->advancedList->mapToGlobal(position));
	this->anythingChanged();
}

bool FilterDialog::isFilterSaved() {
	return this->filterSaved;
}

GCM::io::File FilterDialog::getFilterFile() {
	if (!this->isFilterSaved()) {
		return NULL;
	} else {
		return this->filterFile;
	}
}

void FilterDialog::anythingChanged() {
	this->filterSaved = false;
}
