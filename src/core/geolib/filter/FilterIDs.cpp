#include <geolib/filter/Advanced.h>
#include <geolib/filter/CacheName.h>
#include <geolib/filter/Combined.h>
#include <geolib/filter/Difficulty.h>
#include <geolib/filter/Distance.h>
#include <geolib/filter/Found.h>
#include <geolib/filter/NotFound.h>
#include <geolib/filter/MyOwn.h>
#include <geolib/filter/MyNotOwn.h>
#include <geolib/filter/GeocacheType.h>
#include <geolib/filter/HasCorrectedOrFinal.h>
#include <geolib/filter/DoesNotHaveCorrectedOrFinal.h>
#include <geolib/filter/MatchAll.h>
#include <geolib/filter/PlacedBy.h>
#include <geolib/filter/Size.h>
#include <geolib/filter/StatusAvailable.h>
#include <geolib/filter/StatusArchived.h>
#include <geolib/filter/StatusDisabled.h>
#include <geolib/filter/Terrain.h>
#include <geolib/filter/WaypointType.h>

using namespace GCM::geolib::filter;

const int GCM_API Combined::ID = 1;
const int GCM_API MatchAll::ID = 2;
const int GCM_API Advanced::ID = 3;

const int GCM_API CacheName::ID = 10;
const int GCM_API PlacedBy::ID = 11;
const int GCM_API Difficulty::ID = 12;
const int GCM_API Terrain::ID = 13;
const int GCM_API Size::ID = 14;
const int GCM_API StatusAvailable::ID = 15;
const int GCM_API StatusArchived::ID = 16;
const int GCM_API StatusDisabled::ID = 17;
const int GCM_API HasCorrectedOrFinal::ID = 18;
const int GCM_API DoesNotHaveCorrectedOrFinal::ID = 19;
const int GCM_API Found::ID = 20;
const int GCM_API NotFound::ID = 21;
const int GCM_API MyOwn::ID = 22;
const int GCM_API MyNotOwn::ID = 23;
const int GCM_API Distance::ID = 24;

const int GCM_API GeocacheType::ID = 30;
const int GCM_API WaypointType::ID = 31;
