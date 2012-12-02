#include "CacheDetailsDialog.h"
#include "ui_CacheDetailsDialog.h"
#include <GC.h>
#include <templater/Templater.h>
#include <geolib/Geocache.h>
#include <geolib/Attribute.h>
#include <geolib/AttributeSet.h>
#include <QFile>
#include <database/Database.h>
#include <QDesktopServices>
#include <QWebFrame>
#include "QtGeolib.h"
#include <QShortcut>
#include <QKeySequence>
#include <QToolBar>

using namespace GCM;
using namespace GCM::geolib;
using namespace GCM::templater;

CacheDetailsDialog::CacheDetailsDialog(GCM::GC<GCM::geolib::Geocache> cache, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CacheDetailsDialog)
{
	ui->setupUi(this);
	this->cache = cache;

	this->setWindowTitle(QString::fromUtf8(cache->getName()->c_str()));
	this->setWindowIcon(QtGeolib::cacheIconSmall(cache));

	if (!cache->isLongLoaded() && cache->getDbProvider()) {
		cache->getDbProvider()->loadLongData(cache);
	}

	if (!cache->isAttributesLoaded() && cache->getDbProvider()) {
		cache->getDbProvider()->loadAttributes(cache);
	}

	if (!cache->isLogsLoaded() && cache->getDbProvider()) {
		cache->getDbProvider()->loadLogs(cache);
	}

	this->ui->listingView->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
	this->ui->listingView->setContextMenuPolicy(Qt::CustomContextMenu);
	this->ui->listingView->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
	connect(this->ui->listingView->page(), SIGNAL(linkClicked(QUrl)), this, SLOT(onLinkClicked(QUrl)));

	/*this->ui->action_Copy->setShortcut(QKeySequence::Copy);
	connect(this->ui->action_Copy, SIGNAL(triggered()), this, SLOT(onCopy()));*/

	QShortcut *sh = new QShortcut(QKeySequence::Copy, this);
	connect(sh, SIGNAL(activated()), this, SLOT(onCopy()));

	Templater tplDetails;

	// Load basic cache information
	GCM::GC<Variable> vcache = new Variable();
	vcache
		->assign("Name", cache->getName())
		->assign("GCID", cache->getId())
		->assign("PlacedBy", cache->getPlacedBy())
		->assign("DtHidden", (int64_t)cache->getDtCreated())
		->assign("Latitude", cache->getLatitude())
		->assign("Longitude", cache->getLongitude())
		->assign("State", cache->getState())
		->assign("Country", cache->getCountry())
		->assign("Difficulty", cache->getDifficulty() / 10.0)
		->assign("Size", (int32_t)cache->getSize())
		->assign("Terrain", cache->getTerrain() / 10.0)
		->assign("ShortDescription", cache->getShortDescription())
		->assign("LongDescription", cache->getLongDescription())
		->assign("Hint", cache->getHint());

	// Load attributes
	GCM::GC<Variable> vattributes = vcache->access("Attributes");
	GCM::GC<AttributeSet> attributes = cache->getAttributes();
	AttributeSet::iterator i = attributes->begin();
	Attribute::AttributeList &list = Attribute::getKnownAttributes();

	while (i != attributes->end()) {
		GCM::GC<Variable> vattribute = new Variable();
		vattribute
			->assign("ID", i->second->getId())
			->assign("Yes", i->second->isPositive())
			->assign("Name", list[i->second->getId()]);

		vattributes->assign(String::format("%d", i->second->getId()), vattribute);

		++i;
	}
	vcache->assign("Attributes", vattributes);

	// Load waypoints
	GCM::GC<GeocacheWaypointList> waypoints = cache->getWaypoints();
	GeocacheWaypointList::iterator wi = waypoints->begin();
	GCM::GC<Variable> vwaypoints = new Variable();
	while (wi != waypoints->end()) {
		GCM::GC<Variable> vwaypoint = new Variable();
		vwaypoint
			->assign("Type", (*wi)->getWptType())
			->assign("Prefix", String((*wi)->getId(), 0, 2))
			->assign("Name", (*wi)->getName())
			->assign("Comment", (*wi)->getComment())
			->assign("Latitude", (*wi)->getLatitude())
			->assign("Longitude", (*wi)->getLongitude());

		vwaypoints->assign((*wi)->getId(), vwaypoint);
		wi++;
	}
	vcache->assign("Waypoints", vwaypoints);

	// Load logs
	GCM::GC<LogList> logs = cache->getLogs();
	LogList::iterator li = logs->begin();
	GCM::GC<Variable> vlogs = new Variable();
	while (li != logs->end()) {
		GCM::GC<Variable> log = new Variable();
		log->assign("ID", (*li)->getId());
		log->assign("Date", (int64_t)((*li)->getTime()));
		log->assign("Finder", (*li)->getFinder());
		log->assign("FinderID", (*li)->getFinderId());
		log->assign("Type", (*li)->getType());
		log->assign("Text", (*li)->getText());
		log->assign("IsEncoded", (*li)->isTextEncoded());

		vlogs->assign(String::format("%d", (*li)->getId()), log);

		li++;
	}
	vcache->assign("Logs", vlogs);

	tplDetails.assign("Cache", vcache);

	//tplDetails.debugVar(vcache);

	QFile data(":/html/listing.html");
	data.open(QFile::ReadOnly);
	String out = tplDetails.eval(data.readAll().data());
	data.close();

	this->ui->listingView->setHtml(QString::fromUtf8(out->c_str()), QUrl("http://www.geocaching.com/seek/cache_details.aspx?wp=" + QString::fromUtf8(cache->getId()->c_str())));
}

CacheDetailsDialog::~CacheDetailsDialog()
{
	delete ui;
}

void CacheDetailsDialog::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void CacheDetailsDialog::onLinkClicked(QUrl url) {
	// Load anchors inside the document.
	if (url.hasFragment() && url.toString(QUrl::RemoveFragment) == this->ui->listingView->page()->currentFrame()->baseUrl().toString()) {
#if QT_VERSION > 0x040700
		this->ui->listingView->page()->currentFrame()->scrollToAnchor(url.fragment());
		//this->ui->listingView->page()->currentFrame()->evaluateJavaScript("window.location.hash = \"" + url.fragment() + "\";");
#else
		this->ui->listingView->page()->currentFrame()->evaluateJavaScript("window.location.hash = \"" + url.fragment() + "\";");
#endif

	// Load external URLs outside.
	} else {
		QDesktopServices::openUrl(url);
	}
}

void CacheDetailsDialog::onCopy() {
	this->ui->listingView->triggerPageAction(QWebPage::Copy);
}
