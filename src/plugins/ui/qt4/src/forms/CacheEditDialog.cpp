#include "CacheEditDialog.h"
#include "ui_CacheEditDialog.h"
#include <time.h>
#include <QMap>
#include <database/Database.h>
#include <GC.h>
#include <geolib/Geocache.h>
#include <geolib/GeocacheWaypoint.h>
#include <geolib/GeocacheWaypointList.h>
#include <geolib/Attribute.h>
#include <geolib/AttributeSet.h>
#include "FlowLayout.h"
#include "../widgets/AttributeChecked.h"

using namespace GCM::geolib;

QMap<QString, QStringList> CacheEditDialog::countrys;
bool CacheEditDialog::countrysInitialized = false;
void CacheEditDialog::initCountrys() {
        if (countrysInitialized) return;

        // Countrys
        countrys["Afghanistan"] = QStringList();
        countrys["Aland Islands"] = QStringList();
        countrys["Albania"] = QStringList();
        countrys["Algeria"] = QStringList();
        countrys["American Samoa"] = QStringList();
        countrys["Andorra"] = QStringList();
        countrys["Angola"] = QStringList();
        countrys["Anguilla"] = QStringList();
        countrys["Antarctica"] = QStringList();
        countrys["Antigua and Barbuda"] = QStringList();
        countrys["Argentina"] = QStringList();
        countrys["Armenia"] = QStringList();
        countrys["Aruba"] = QStringList();
        countrys["Australia"] = QStringList();
        countrys["Austria"] = QStringList();
        countrys["Azerbaijan"] = QStringList();
        countrys["Bahamas"] = QStringList();
        countrys["Bahrain"] = QStringList();
        countrys["Bangladesh"] = QStringList();
        countrys["Barbados"] = QStringList();
        countrys["Belarus"] = QStringList();
        countrys["Belgium"] = QStringList();
        countrys["Belize"] = QStringList();
        countrys["Benin"] = QStringList();
        countrys["Bermuda"] = QStringList();
        countrys["Bhutan"] = QStringList();
        countrys["Bolivia"] = QStringList();
        countrys["Bonaire"] = QStringList();
        countrys["Bosnia and Herzegovina"] = QStringList();
        countrys["Botswana"] = QStringList();
        countrys["Bouvet Island"] = QStringList();
        countrys["Brazil"] = QStringList();
        countrys["British Indian Ocean Territories"] = QStringList();
        countrys["British Virgin Islands"] = QStringList();
        countrys["Brunei"] = QStringList();
        countrys["Bulgaria"] = QStringList();
        countrys["Burkina Faso"] = QStringList();
        countrys["Burundi"] = QStringList();
        countrys["Cambodia"] = QStringList();
        countrys["Cameroon"] = QStringList();
        countrys["Canada"] = QStringList();
        countrys["Cape Verde"] = QStringList();
        countrys["Cayman Islands"] = QStringList();
        countrys["Central African Republic"] = QStringList();
        countrys["Chad"] = QStringList();
        countrys["Chile"] = QStringList();
        countrys["China"] = QStringList();
        countrys["Christmas Island"] = QStringList();
        countrys["Cocos (Keeling) Islands"] = QStringList();
        countrys["Colombia"] = QStringList();
        countrys["Comoros"] = QStringList();
        countrys["Congo"] = QStringList();
        countrys["Cook Islands"] = QStringList();
        countrys["Costa Rica"] = QStringList();
        countrys["Croatia"] = QStringList();
        countrys["Cuba"] = QStringList();
        countrys["Curacao"] = QStringList();
        countrys["Cyprus"] = QStringList();
        countrys["Czech Republic"] = QStringList();
        countrys["Democratic Republic of the Congo"] = QStringList();
        countrys["Denmark"] = QStringList();
        countrys["Djibouti"] = QStringList();
        countrys["Dominica"] = QStringList();
        countrys["Dominican Republic"] = QStringList();
        countrys["East Timor"] = QStringList();
        countrys["Ecuador"] = QStringList();
        countrys["Egypt"] = QStringList();
        countrys["El Salvador"] = QStringList();
        countrys["Equatorial Guinea"] = QStringList();
        countrys["Eritrea"] = QStringList();
        countrys["Estonia"] = QStringList();
        countrys["Ethiopia"] = QStringList();
        countrys["Falkland Islands"] = QStringList();
        countrys["Faroe Islands"] = QStringList();
        countrys["Fiji"] = QStringList();
        countrys["Finland"] = QStringList();
        countrys["France"] = QStringList();
        countrys["French Guiana"] = QStringList();
        countrys["French Polynesia"] = QStringList();
        countrys["French Southern Territories"] = QStringList();
        countrys["Gabon"] = QStringList();
        countrys["Gambia"] = QStringList();
        countrys["Georgia"] = QStringList();
        countrys["Germany"] = QStringList();
        countrys["Ghana"] = QStringList();
        countrys["Gibraltar"] = QStringList();
        countrys["Greece"] = QStringList();
        countrys["Greenland"] = QStringList();
        countrys["Grenada"] = QStringList();
        countrys["Guadeloupe"] = QStringList();
        countrys["Guam"] = QStringList();
        countrys["Guatemala"] = QStringList();
        countrys["Guernsey"] = QStringList();
        countrys["Guinea"] = QStringList();
        countrys["Guinea-Bissau"] = QStringList();
        countrys["Guyana"] = QStringList();
        countrys["Haiti"] = QStringList();
        countrys["Heard Island And Mcdonald Islands"] = QStringList();
        countrys["Honduras"] = QStringList();
        countrys["Hong Kong"] = QStringList();
        countrys["Hungary"] = QStringList();
        countrys["Iceland"] = QStringList();
        countrys["India"] = QStringList();
        countrys["Indonesia"] = QStringList();
        countrys["Iran"] = QStringList();
        countrys["Iraq"] = QStringList();
        countrys["Ireland"] = QStringList();
        countrys["Isle of Man"] = QStringList();
        countrys["Israel"] = QStringList();
        countrys["Italy"] = QStringList();
        countrys["Ivory Coast"] = QStringList();
        countrys["Jamaica"] = QStringList();
        countrys["Japan"] = QStringList();
        countrys["Jersey"] = QStringList();
        countrys["Jordan"] = QStringList();
        countrys["Kazakhstan"] = QStringList();
        countrys["Kenya"] = QStringList();
        countrys["Kiribati"] = QStringList();
        countrys["Kuwait"] = QStringList();
        countrys["Kyrgyzstan"] = QStringList();
        countrys["Laos"] = QStringList();
        countrys["Latvia"] = QStringList();
        countrys["Lebanon"] = QStringList();
        countrys["Lesotho"] = QStringList();
        countrys["Liberia"] = QStringList();
        countrys["Libya"] = QStringList();
        countrys["Liechtenstein"] = QStringList();
        countrys["Lithuania"] = QStringList();
        countrys["Luxembourg"] = QStringList();
        countrys["Macau"] = QStringList();
        countrys["Macedonia"] = QStringList();
        countrys["Madagascar"] = QStringList();
        countrys["Malawi"] = QStringList();
        countrys["Malaysia"] = QStringList();
        countrys["Maldives"] = QStringList();
        countrys["Mali"] = QStringList();
        countrys["Malta"] = QStringList();
        countrys["Marshall Islands"] = QStringList();
        countrys["Martinique"] = QStringList();
        countrys["Mauritania"] = QStringList();
        countrys["Mauritius"] = QStringList();
        countrys["Mayotte"] = QStringList();
        countrys["Mexico"] = QStringList();
        countrys["Micronesia"] = QStringList();
        countrys["Moldova"] = QStringList();
        countrys["Monaco"] = QStringList();
        countrys["Mongolia"] = QStringList();
        countrys["Montenegro"] = QStringList();
        countrys["Montserrat"] = QStringList();
        countrys["Morocco"] = QStringList();
        countrys["Mozambique"] = QStringList();
        countrys["Myanmar"] = QStringList();
        countrys["Namibia"] = QStringList();
        countrys["Nauru"] = QStringList();
        countrys["Nepal"] = QStringList();
        countrys["Netherlands"] = QStringList();
        countrys["Netherlands Antilles"] = QStringList();
        countrys["Nevis and St Kitts"] = QStringList();
        countrys["New Caledonia"] = QStringList();
        countrys["New Zealand"] = QStringList();
        countrys["Nicaragua"] = QStringList();
        countrys["Niger"] = QStringList();
        countrys["Nigeria"] = QStringList();
        countrys["Niue"] = QStringList();
        countrys["Norfolk Island"] = QStringList();
        countrys["North Korea"] = QStringList();
        countrys["Northern Mariana Islands"] = QStringList();
        countrys["Norway"] = QStringList();
        countrys["Oman"] = QStringList();
        countrys["Pakistan"] = QStringList();
        countrys["Palau"] = QStringList();
        countrys["Palestine"] = QStringList();
        countrys["Panama"] = QStringList();
        countrys["Papua New Guinea"] = QStringList();
        countrys["Paraguay"] = QStringList();
        countrys["Peru"] = QStringList();
        countrys["Philippines"] = QStringList();
        countrys["Pitcairn Islands"] = QStringList();
        countrys["Poland"] = QStringList();
        countrys["Portugal"] = QStringList();
        countrys["Puerto Rico"] = QStringList();
        countrys["Qatar"] = QStringList();
        countrys["Reunion"] = QStringList();
        countrys["Romania"] = QStringList();
        countrys["Russia"] = QStringList();
        countrys["Rwanda"] = QStringList();
        countrys["Saba"] = QStringList();
        countrys["Saint Helena"] = QStringList();
        countrys["Saint Kitts and Nevis"] = QStringList();
        countrys["Saint Lucia"] = QStringList();
        countrys["Samoa"] = QStringList();
        countrys["San Marino"] = QStringList();
        countrys["Sao Tome and Principe"] = QStringList();
        countrys["Saudi Arabia"] = QStringList();
        countrys["Senegal"] = QStringList();
        countrys["Serbia"] = QStringList();
        countrys["Seychelles"] = QStringList();
        countrys["Sierra Leone"] = QStringList();
        countrys["Singapore"] = QStringList();
        countrys["Slovakia"] = QStringList();
        countrys["Slovenia"] = QStringList();
        countrys["Solomon Islands"] = QStringList();
        countrys["Somalia"] = QStringList();
        countrys["South Africa"] = QStringList();
        countrys["South Georgia and Sandwich Islands"] = QStringList();
        countrys["South Korea"] = QStringList();
        countrys["South Sudan"] = QStringList();
        countrys["Spain"] = QStringList();
        countrys["Sri Lanka"] = QStringList();
        countrys["St Barthelemy"] = QStringList();
        countrys["St Eustatius"] = QStringList();
        countrys["St Kitts"] = QStringList();
        countrys["St Pierre Miquelon"] = QStringList();
        countrys["St Vince Grenadines"] = QStringList();
        countrys["St. Martin"] = QStringList();
        countrys["Sudan"] = QStringList();
        countrys["Suriname"] = QStringList();
        countrys["Svalbard and Jan Mayen"] = QStringList();
        countrys["Swaziland"] = QStringList();
        countrys["Sweden"] = QStringList();
        countrys["Switzerland"] = QStringList();
        countrys["Syria"] = QStringList();
        countrys["Taiwan"] = QStringList();
        countrys["Tajikistan"] = QStringList();
        countrys["Tanzania"] = QStringList();
        countrys["Thailand"] = QStringList();
        countrys["Togo"] = QStringList();
        countrys["Tokelau"] = QStringList();
        countrys["Tonga"] = QStringList();
        countrys["Trinidad and Tobago"] = QStringList();
        countrys["Tunisia"] = QStringList();
        countrys["Turkey"] = QStringList();
        countrys["Turkmenistan"] = QStringList();
        countrys["Turks and Caicos Islands"] = QStringList();
        countrys["Tuvalu"] = QStringList();
        countrys["Uganda"] = QStringList();
        countrys["Ukraine"] = QStringList();
        countrys["United Arab Emirates"] = QStringList();
        countrys["United Kingdom"] = QStringList();
        countrys["United States"] = QStringList();
        countrys["Unknown"] = QStringList();
        countrys["Uruguay"] = QStringList();
        countrys["US Minor Outlying Islands"] = QStringList();
        countrys["US Virgin Islands"] = QStringList();
        countrys["Uzbekistan"] = QStringList();
        countrys["Vanuatu"] = QStringList();
        countrys["Vatican City State"] = QStringList();
        countrys["Venezuela"] = QStringList();
        countrys["Vietnam"] = QStringList();
        countrys["Wallis And Futuna Islands"] = QStringList();
        countrys["Western Sahara"] = QStringList();
        countrys["Yemen"] = QStringList();
        countrys["Zambia"] = QStringList();
        countrys["Zimbabwe"] = QStringList();

        // States for USA
        QStringList *states = &countrys["United States"];
        states->append("Alabama (AL)");
        states->append("Alaska (AK)");
        states->append("Arizona (AZ)");
        states->append("Arkansas (AR)");
        states->append("California (CA)");
        states->append("Colorado (CO)");
        states->append("Connecticut (CT)");
        states->append("Delaware (DE)");
        states->append("District of Columbia (DC)");
        states->append("Florida (FL)");
        states->append("Georgia (GA)");
        states->append("Hawaii (HI)");
        states->append("Idaho (ID)");
        states->append("Illinois (IL)");
        states->append("Indiana (IN)");
        states->append("Iowa (IA)");
        states->append("Kansas (KS)");
        states->append("Kentucky (KY)");
        states->append("Louisiana (LA)");
        states->append("Maine (ME)");
        states->append("Maryland (MD)");
        states->append("Massachusetts (MA)");
        states->append("Michigan (MI)");
        states->append("Minnesota (MN)");
        states->append("Mississippi (MS)");
        states->append("Missouri (MO)");
        states->append("Montana (MT)");
        states->append("Nebraska (NE)");
        states->append("Nevada (NV)");
        states->append("New Hampshire (NH)");
        states->append("New Jersey (NJ)");
        states->append("New Mexico (NM)");
        states->append("New York (NY)");
        states->append("North Carolina (NC)");
        states->append("North Dakota (ND)");
        states->append("Ohio (OH)");
        states->append("Oklahoma (OK)");
        states->append("Oregon (OR)");
        states->append("Pennsylvania (PA)");
        states->append("Rhode Island (RI)");
        states->append("South Carolina (SC)");
        states->append("South Dakota (SD)");
        states->append("Tennessee (TN)");
        states->append("Texas (TX)");
        states->append("Utah (UT)");
        states->append("Vermont (VT)");
        states->append("Virginia (VA)");
        states->append("Washington (WA)");
        states->append("West Virginia (WV)");
        states->append("Wisconsin (WI)");
        states->append("Wyoming (WY)");

        states = &countrys["Australia"];
        states->append("Australian Capital Territory");
        states->append("New South Wales");
        states->append("Northern Territory");
        states->append("Queensland");
        states->append("South Australia");
        states->append("Tasmania");
        states->append("Victoria");
        states->append("Western Australia");

        states = &countrys["Austria"];
        states->append("Burgenland");
        states->append("Kärnten");
        states->append("Niederösterreich");
        states->append("Oberösterreich");
        states->append("Salzburg");
        states->append("Steiermark");
        states->append("Tirol");
        states->append("Vorarlberg");
        states->append("Wien");

        states = &countrys["Belgium"];
        states->append("Antwerpen");
        states->append("Brabant wallon");
        states->append("Brussels");
        states->append("Hainaut");
        states->append("Liège");
        states->append("Limburg");
        states->append("Luxembourg");
        states->append("Namur");
        states->append("Oost-Vlaanderen");
        states->append("Vlaams-Brabant");
        states->append("West-Vlaanderen");

        states = &countrys["Brazil"];
        states->append("Acre");
        states->append("Alagoas");
        states->append("Amapá");
        states->append("Amazonas");
        states->append("Bahia");
        states->append("Ceará");
        states->append("Distrito Federal");
        states->append("Espírito Santo");
        states->append("Goiás");
        states->append("Maranhão");
        states->append("Mato Grosso");
        states->append("Mato Grosso do Sul");
        states->append("Minas Gerais");
        states->append("Pará");
        states->append("Paraíba");
        states->append("Paraná");
        states->append("Pernambuco");
        states->append("Piauí");
        states->append("Rio de Janeiro");
        states->append("Rio Grande do Norte");
        states->append("Rio Grande do Sul");
        states->append("Rondônia");
        states->append("Roraima");
        states->append("Santa Catarina");
        states->append("São Paulo");
        states->append("Sergipe");
        states->append("Tocantins");

        states = &countrys["Canada"];
        states->append("Alberta");
        states->append("British Columbia");
        states->append("Manitoba");
        states->append("New Brunswick");
        states->append("Newfoundland and Labrador");
        states->append("Northwest Territories");
        states->append("Nova Scotia");
        states->append("Nunavut");
        states->append("Ontario");
        states->append("Prince Edward Island");
        states->append("Quebec");
        states->append("Saskatchewan");
        states->append("Yukon Territory");

        states = &countrys["Czech Republic"];
        states->append("Hlavni mesto Praha");
        states->append("Jihocesky kraj");
        states->append("Jihomoravsky kraj");
        states->append("Karlovarsky kraj");
        states->append("Kraj Vysocina");
        states->append("Kralovehradecky kraj");
        states->append("Liberecky kraj");
        states->append("Moravskoslezsky kraj");
        states->append("Olomoucky kraj");
        states->append("Pardubicky kraj");
        states->append("Plzensky kraj");
        states->append("Stredocesky kraj");
        states->append("Ustecky kraj");
        states->append("Zlinsky kraj");

        states = &countrys["France"];
        states->append("Alsace");
        states->append("Aquitaine");
        states->append("Auvergne");
        states->append("Basse-Normandie");
        states->append("Bourgogne");
        states->append("Bretagne");
        states->append("Centre");
        states->append("Champagne-Ardenne");
        states->append("Corse");
        states->append("Franche-Comté");
        states->append("Haute-Normandie");
        states->append("Île-de-France");
        states->append("Languedoc-Roussillon");
        states->append("Limousin");
        states->append("Lorraine");
        states->append("Midi-Pyrénées");
        states->append("Nord-Pas-de-Calais");
        states->append("Pays de la Loire");
        states->append("Picardie");
        states->append("Poitou-Charentes");
        states->append("Provence-Alpes-Côte d'Azur");
        states->append("Rhône-Alpes");

        states = &countrys["Germany"];
        states->append("Baden-Württemberg");
        states->append("Bayern");
        states->append("Berlin");
        states->append("Brandenburg");
        states->append("Bremen");
        states->append("Hamburg");
        states->append("Hessen");
        states->append("Mecklenburg-Vorpommern");
        states->append("Niedersachsen");
        states->append("Nordrhein-Westfalen");
        states->append("Rheinland-Pfalz");
        states->append("Saarland");
        states->append("Sachsen");
        states->append("Sachsen-Anhalt");
        states->append("Schleswig-Holstein");
        states->append("Thüringen");

        states = &countrys["Hungary"];
        states->append("Bács-Kiskun");
        states->append("Baranya");
        states->append("Békés");
        states->append("Borsod-Abaúj-Zemplén");
        states->append("Budapest");
        states->append("Csongrád");
        states->append("Fejér");
        states->append("Gyor-Moson-Sopron");
        states->append("Hajdú-Bihar");
        states->append("Heves");
        states->append("Jász-Nagykun-Szolnok");
        states->append("Komárom-Esztergom");
        states->append("Nógrád");
        states->append("Pest");
        states->append("Somogy");
        states->append("Szabolcs-Szatmár-Bereg");
        states->append("Tolna");
        states->append("Vas");
        states->append("Veszprém");
        states->append("Zala");

        states = &countrys["Ireland"];
        states->append("Connacht");
        states->append("Dublin");
        states->append("Leinster");
        states->append("Munster");
        states->append("Ulster");

        states = &countrys["Italy"];
        states->append("Abruzzo");
        states->append("Basilicata");
        states->append("Calabria");
        states->append("Campania");
        states->append("Emilia-Romagna");
        states->append("Friuli-Venezia Giulia");
        states->append("Lazio");
        states->append("Liguria");
        states->append("Lombardia");
        states->append("Marche");
        states->append("Molise");
        states->append("Piemonte");
        states->append("Puglia");
        states->append("Sardegna");
        states->append("Sicilia");
        states->append("Toscana");
        states->append("Trentino-Alto Adige");
        states->append("Umbria");
        states->append("Valle d'Aosta");
        states->append("Veneto");

        states = &countrys["Japan"];
        states->append("Aichi");
        states->append("Akita");
        states->append("Aomori");
        states->append("Chiba");
        states->append("Ehime");
        states->append("Fukui");
        states->append("Fukuoka");
        states->append("Fukushima");
        states->append("Gifu");
        states->append("Gunma");
        states->append("Hiroshima");
        states->append("Hokkaido");
        states->append("Hyogo");
        states->append("Ibaraki");
        states->append("Ishikawa");
        states->append("Iwate");
        states->append("Kagawa");
        states->append("Kagoshima");
        states->append("Kanagawa");
        states->append("Kochi");
        states->append("Kumamoto");
        states->append("Kyoto");
        states->append("Mie");
        states->append("Miyagi");
        states->append("Miyazaki");
        states->append("Nagano");
        states->append("Nagasaki");
        states->append("Nara");
        states->append("Niigata");
        states->append("Oita");
        states->append("Okayama");
        states->append("Okinawa");
        states->append("Osaka");
        states->append("Saga");
        states->append("Saitama");
        states->append("Shiga");
        states->append("Shimane");
        states->append("Shizuoka");
        states->append("Tochigi");
        states->append("Tokushima");
        states->append("Tokyo");
        states->append("Tottori");
        states->append("Toyama");
        states->append("Wakayama");
        states->append("Yamagata");
        states->append("Yamaguchi");
        states->append("Yamanashi");

        states = &countrys["Mexico"];
        states->append("Aguascalientes");
        states->append("Baja California");
        states->append("Baja California Sur");
        states->append("Campeche");
        states->append("Chiapas");
        states->append("Chihuahua");
        states->append("Coahuila");
        states->append("Colima");
        states->append("Distrito Federal");
        states->append("Durango");
        states->append("Guanajuato");
        states->append("Guerrero");
        states->append("Hidalgo");
        states->append("Jalisco");
        states->append("México");
        states->append("Michoacán");
        states->append("Morelos");
        states->append("Nayarit");
        states->append("Nuevo León");
        states->append("Oaxaca");
        states->append("Puebla");
        states->append("Querétaro");
        states->append("Quintana Roo");
        states->append("San Luis Potosí");
        states->append("Sinaloa");
        states->append("Sonora");
        states->append("Tabasco");
        states->append("Tamaulipas");
        states->append("Tlaxcala");
        states->append("Veracruz");
        states->append("Yucatán");
        states->append("Zacatecas");

        states = &countrys["Netherlands"];
        states->append("Drenthe");
        states->append("Flevoland");
        states->append("Friesland");
        states->append("Gelderland");
        states->append("Groningen");
        states->append("Limburg");
        states->append("Noord-Brabant");
        states->append("Noord-Holland");
        states->append("Overijssel");
        states->append("Utrecht");
        states->append("Zeeland");
        states->append("Zuid-Holland");

        states = &countrys["New Zeland"];
        states->append("Chatham Islands");
        states->append("North Island");
        states->append("South Island");

        states = &countrys["Norway"];
        states->append("Akershus");
        states->append("Aust-Agder");
        states->append("Buskerud");
        states->append("Finnmark");
        states->append("Hedmark");
        states->append("Hordaland");
        states->append("Møre og Romsdal");
        states->append("Nordland");
        states->append("Nord-Trøndelag");
        states->append("Oppland");
        states->append("Oslo");
        states->append("Østfold");
        states->append("Rogaland");
        states->append("Sogn og Fjordane");
        states->append("Sør-Trøndelag");
        states->append("Telemark");
        states->append("Troms");
        states->append("Vest-Agder");
        states->append("Vestfold");

        states = &countrys["Poland"];
        states->append("Dolnośląskie");
        states->append("Kujawsko-Pomorskie");
        states->append("Łódzkie");
        states->append("Lubelskie");
        states->append("Lubuskie");
        states->append("Małopolskie");
        states->append("Mazowieckie");
        states->append("Opolskie");
        states->append("Podkarpackie");
        states->append("Podlaskie");
        states->append("Pomorskie");
        states->append("Śląskie");
        states->append("Świętokrzyskie");
        states->append("Warmińsko-Mazurskie");
        states->append("Wielkopolskie");
        states->append("Zachodniopomorskie");

        states = &countrys["Portugal"];
        states->append("Arquipélago da Madeira");
        states->append("Arquipélago dos Açores");
        states->append("Aveiro");
        states->append("Beja");
        states->append("Braga");
        states->append("Bragança");
        states->append("Castelo Branco");
        states->append("Coimbra");
        states->append("Évora");
        states->append("Faro");
        states->append("Guarda");
        states->append("Leiria");
        states->append("Lisboa");
        states->append("Portalegre");
        states->append("Porto");
        states->append("Santarém");
        states->append("Setúbal");
        states->append("Viana do Castelo");
        states->append("Vila Real");
        states->append("Viseu");

        states = &countrys["Slovakia"];
        states->append("Banskobystrický kraj");
        states->append("Bratislavský kraj");
        states->append("Košický kraj");
        states->append("Nitriansky kraj");
        states->append("Prešovský kraj");
        states->append("Trenčiansky kraj");
        states->append("Trnavský kraj");
        states->append("Žilinský kraj");

        states = &countrys["Spain"];
        states->append("Andalucía");
        states->append("Aragón");
        states->append("Cantabria");
        states->append("Castilla y León");
        states->append("Castilla-La Mancha");
        states->append("Cataluña");
        states->append("Ceuta");
        states->append("Comunidad de Madrid");
        states->append("Comunidad Foral de Navarra");
        states->append("Comunidad Valenciana");
        states->append("Extremadura");
        states->append("Galicia");
        states->append("Islas Baleares");
        states->append("Islas Canarias");
        states->append("La Rioja");
        states->append("Melilla");
        states->append("País Vasco");
        states->append("Principado de Asturias");
        states->append("Región de Murcia");

        states = &countrys["Sweden"];
        states->append("Blekinge");
        states->append("Dalarna");
        states->append("Gävleborg");
        states->append("Gotland");
        states->append("Halland");
        states->append("Jämtland");
        states->append("Jönköping");
        states->append("Kalmar");
        states->append("Kronoberg");
        states->append("Norrbotten");
        states->append("Örebro");
        states->append("Östergötland");
        states->append("Skåne");
        states->append("Södermanland");
        states->append("Stockholm");
        states->append("Uppsala");
        states->append("Värmland");
        states->append("Västerbotten");
        states->append("Västernorrland");
        states->append("Västmanland");
        states->append("Västra Götaland");

        states = &countrys["Switzerland"];
        states->append("Espace Mittelland (BE/SO)");
        states->append("Graubuenden (GR)");
        states->append("Jura (JU/NE)");
        states->append("Nordwestschweiz (AG/BL/BS)");
        states->append("Ostschweiz (SG/SH/TG/AI/AR/GL)");
        states->append("Region Zuerich (ZH)");
        states->append("Suisse romande (GE/VD/FR)");
        states->append("Tessin (TI)");
        states->append("Wallis (VS)");
        states->append("Zentralschweiz (ZG/SZ/LU/UR/OW/NW)");

        states = &countrys["United Kingdom"];
        states->append("East Midlands");
        states->append("Eastern England");
        states->append("London");
        states->append("North Wales");
        states->append("Northeast England");
        states->append("Northern Scotland");
        states->append("Northwest England");
        states->append("South East England");
        states->append("South Wales");
        states->append("South West England");
        states->append("Southern England");
        states->append("Southern Scotland");
        states->append("West Midlands");
        states->append("Yorkshire");

        countrysInitialized = true;
}

CacheEditDialog::CacheEditDialog(GCM::GC<GCM::geolib::Geocache> cache, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CacheEditDialog),
        cache(cache)
{
        ui->setupUi(this);
        this->setupUi();

        if (!cache->isLongLoaded()) {
                wereLongLoaded = false;
                if (cache->getDbProvider()) {
                        cache->getDbProvider()->loadLongData(cache);
                }
        } else {
                wereLongLoaded = true;
        }

        if (!cache->isAttributesLoaded()) {
                wereAttributesLoaded = true;
                if (cache->getDbProvider()) {
                        cache->getDbProvider()->loadAttributes(cache);
                }
        } else {
                wereAttributesLoaded = false;
        }

        this->ui->labelGCID->setText(QString::fromUtf8(cache->getId()->c_str()));
        this->ui->cacheName->setText(QString::fromUtf8(cache->getName()->c_str()));

        QString title;
        title.sprintf("Modify cache %s", cache->getName()->c_str());
        this->setWindowTitle(title);

        if (cache->isAvailable()) {
                this->ui->comboStatus->setCurrentIndex(0);
        } else if (cache->isArchived()) {
                this->ui->comboStatus->setCurrentIndex(2);
        } else {
                this->ui->comboStatus->setCurrentIndex(1);
        }

        this->ui->placedBy->setText(QString::fromUtf8(cache->getPlacedBy()->c_str()));
        this->ui->ownerName->setText(QString::fromUtf8(cache->getOwner()->c_str()));

        QString oid;
        oid.sprintf("%d", cache->getOwnerId().get());
        this->ui->ownerId->setText(oid);

        switch ((Geocache::Type)cache->getCacheType()) {
        case Geocache::T_CITO: this->ui->type->setCurrentIndex(9); break;
        case Geocache::T_EARTH: this->ui->type->setCurrentIndex(4); break;
        case Geocache::T_EVENT: this->ui->type->setCurrentIndex(7); break;
        case Geocache::T_LETTERBOX: this->ui->type->setCurrentIndex(3); break;
        case Geocache::T_MEGAEVENT: this->ui->type->setCurrentIndex(8); break;
        case Geocache::T_MULTI: this->ui->type->setCurrentIndex(1); break;
        case Geocache::T_TRADITIONAL: this->ui->type->setCurrentIndex(0); break;
        case Geocache::T_UNKNOWN: this->ui->type->setCurrentIndex(2); break;
        case Geocache::T_VIRTUAL: this->ui->type->setCurrentIndex(6); break;
        case Geocache::T_WEBCAM: this->ui->type->setCurrentIndex(10); break;
        case Geocache::T_WHERIGO: this->ui->type->setCurrentIndex(5); break;
        default: this->ui->type->setCurrentIndex(0); break; // Fallback to traditional
        }

        switch ((Geocache::Size)cache->getSize()) {
        case Geocache::S_MICRO: this->ui->size->setCurrentIndex(0); break;
        case Geocache::S_SMALL: this->ui->size->setCurrentIndex(1); break;
        case Geocache::S_REGULAR: this->ui->size->setCurrentIndex(2); break;
        case Geocache::S_LARGE: this->ui->size->setCurrentIndex(3); break;
        case Geocache::S_OTHER: this->ui->size->setCurrentIndex(4); break;
        case Geocache::S_NOT_LISTED: this->ui->size->setCurrentIndex(5); break;
        case Geocache::S_VIRTUAL: this->ui->size->setCurrentIndex(6); break;
        }

        this->ui->sliderDifficulty->setValue(cache->getDifficulty() / 5 - 1);
        this->ui->sliderTerrain->setValue(cache->getTerrain() / 5 - 1);

        time_t dtCreated = cache->getDtCreated();
        struct tm *dtParsed = gmtime(&dtCreated);

        this->ui->dateCreated->setDate(QDate(
                dtParsed->tm_year + 1900,
                dtParsed->tm_mon + 1,
                dtParsed->tm_mday
        ));

        QString country = QString::fromUtf8(cache->getCountry()->c_str());
        bool found = false;

        for (int i = 0; i < this->ui->comboCountry->count(); i++) {
                if (this->ui->comboCountry->itemText(i) == country) {
                        this->ui->comboCountry->setCurrentIndex(i);
                        found = true;
                        break;
                }
        }

        if (!found) {
                this->ui->comboCountry->setEditText(country);
        }

        QString state = QString::fromUtf8(cache->getState()->c_str());
        found = false;
        for (int i = 0; i < this->ui->comboState->count(); i++) {
                if (this->ui->comboState->itemText(i) == state) {
                        this->ui->comboState->setCurrentIndex(i);
                        found = true;
                        break;
                }
        }

        if (!found) {
                this->ui->comboState->setEditText(state);
        }

        this->ui->hint->document()->setPlainText(QString::fromUtf8(cache->getHint()->c_str()));

        if (cache->isFound()) {
                time_t dtFound = cache->getDtFound();
                struct tm *dtFParsed = gmtime(&dtFound);

                this->ui->cbFound->setChecked(true);
                this->ui->foundDate->setDate(QDate(dtFParsed->tm_year + 1900, dtFParsed->tm_mon + 1, dtFParsed->tm_mday));
                this->ui->foundTime->setTime(QTime(dtFParsed->tm_hour, dtFParsed->tm_min, 0, 0));
        } else {
                this->ui->cbFound->setChecked(false);
        }

        if (cache->isOwn()) {
            this->ui->cbMyOwn->setChecked(true);
        } else {
            this->ui->cbMyOwn->setChecked(false);
        }

        // Coordinates...
        if (cache->hasCorrectedCoords()) {
                Coords c;
                c.dlat = cache->getLatitude();
                c.dlon = cache->getLongitude();
                c.mlat = (c.dlat - (int)c.dlat) * 60;
                c.mlon = (c.dlon - (int)c.dlon) * 60;
                c.slat = (c.mlat - (int)c.mlat) * 60;
                c.slon = (c.mlon - (int)c.mlon) * 60;

                this->ui->correctedCoordsGroup->setChecked(true);
                this->ui->correctedCoords->setCoords(c);
        }

        Coords c;
        c.dlat = cache->getInitialLatitude();
        c.dlon = cache->getInitialLongitude();
        c.mlat = (c.dlat - (int)c.dlat) * 60;
        c.mlon = (c.dlon - (int)c.dlon) * 60;
        c.slat = (c.mlat - (int)c.mlat) * 60;
        c.slon = (c.mlon - (int)c.mlon) * 60;

        this->ui->initialCoords->setCoords(c);

        // Locking
        this->ui->lockAll->setChecked((cache->getLockFlags() & Geocache::LOCK_ALL) == Geocache::LOCK_ALL);
        this->ui->lockBasic->setChecked((cache->getLockFlags() & Geocache::LOCK_INFO) == Geocache::LOCK_INFO);
        this->ui->lockCoords->setChecked((cache->getLockFlags() & Geocache::LOCK_COORDS) == Geocache::LOCK_COORDS);
        this->ui->lockListing->setChecked((cache->getLockFlags() & Geocache::LOCK_LISTING) == Geocache::LOCK_LISTING);
        this->ui->lockStats->setChecked((cache->getLockFlags() & Geocache::LOCK_BASIC) == Geocache::LOCK_BASIC);

        // User note
        this->ui->userNote->document()->setPlainText(QString::fromUtf8(cache->getComment()->c_str()));

        // Listing
        this->ui->shortDesc->document()->setPlainText(QString::fromUtf8(cache->getShortDescription()->c_str()));
        this->ui->longDesc->document()->setPlainText(QString::fromUtf8(cache->getLongDescription()->c_str()));

        // Scroll listing areas to top
        this->ui->shortDesc->moveCursor(QTextCursor::Start);
        this->ui->shortDesc->ensureCursorVisible();

        this->ui->longDesc->moveCursor(QTextCursor::Start);
        this->ui->longDesc->ensureCursorVisible();

        this->ui->shortDescFlag->setChecked(cache->getSDIsHTML());
        this->ui->longDescFlag->setChecked(cache->getLDIsHTML());

        // Attributes
        GCM::GC<AttributeSet> attributes = cache->getAttributes();
        AttributeSet::iterator ai = attributes->begin();
        while (ai != attributes->end()) {
                this->attributes[ai->first]->setCheckState((ai->second->isPositive())?Qt::Checked:Qt::Unchecked);
                ai++;
        }
}

void CacheEditDialog::setupUi() {
        initCountrys();

        QMap<QString, QStringList>::iterator i = countrys.begin();
        while (i != countrys.end()) {
                this->ui->comboCountry->addItem(i.key());
                i++;
        }
        connect(this->ui->comboCountry, SIGNAL(currentIndexChanged(QString)), this, SLOT(onCountryChanged(QString)));

        connect(this->ui->sliderDifficulty, SIGNAL(valueChanged(int)), this, SLOT(onDiffChanged(int)));
        connect(this->ui->sliderTerrain, SIGNAL(valueChanged(int)), this, SLOT(onTerrChanged(int)));
        connect(this->ui->cbFound, SIGNAL(stateChanged(int)), this, SLOT(onCbFoundClick(int)));
        connect(this->ui->lockAll, SIGNAL(stateChanged(int)), this, SLOT(onLockAllClick(int)));

        connect(this, SIGNAL(accepted()), this, SLOT(onAccepted()));

        // List waypoints for the cache and provide them to the selection.
        this->waypoints = cache->getDbProvider()->getCacheWaypoints(cache);
        GeocacheWaypointList::iterator wi = this->waypoints->begin();

        this->ui->comboWaypoints->addItem("");
        int index = 1;

        while (wi != this->waypoints->end()) {
                QIcon icon;

                switch ((*wi)->getWptType()) {
                case GeocacheWaypoint::T_FINAL: icon = QIcon(":/icons/ic_wpt_final.png"); break;
                case GeocacheWaypoint::T_PARKING: icon = QIcon(":/icons/ic_wpt_parking.png"); break;
                case GeocacheWaypoint::T_QTA: icon = QIcon(":/icons/ic_wpt_qta.png"); break;
                case GeocacheWaypoint::T_REFERENCE: icon = QIcon(":/icons/ic_wpt_reference.png"); break;
                case GeocacheWaypoint::T_SOM: icon = QIcon(":/icons/ic_wpt_stage.png"); break;
                case GeocacheWaypoint::T_TRAILHEAD: icon = QIcon(":/icons/ic_wpt_trailhead.png"); break;
                }

                this->ui->comboWaypoints->addItem(icon, QString::fromUtf8((*wi)->getName()->c_str()));

                if ((*wi)->useAsCorrected()) {
                        this->ui->comboWaypoints->setCurrentIndex(index);
                }

                wi++;
                index++;
        }

        connect(this->ui->comboWaypoints, SIGNAL(currentIndexChanged(int)), this, SLOT(onWptChanged(int)));

        // Create attributes...
        FlowLayout *layout = new FlowLayout(0, 0, 0);
        layout->setContentsMargins(0, 0, 30, 0);

        const Attribute::AttributeList &attributes = Attribute::getKnownAttributes();
        Attribute::AttributeList::const_iterator ai = attributes.begin();
        while (ai != attributes.end()) {
                AttributeChecked *attr = new AttributeChecked(ai->first);
                layout->addWidget(attr);
                this->attributes[ai->first] = attr;
                ai++;
        }

        this->ui->attributes->setLayout(layout);
}

CacheEditDialog::~CacheEditDialog()
{
        QMap<int, AttributeChecked *>::iterator i = this->attributes.begin();
        while (i != this->attributes.end()) {
                delete *i;
                i++;
        }
        delete ui;
}

void CacheEditDialog::changeEvent(QEvent *e)
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

void CacheEditDialog::onDiffChanged(int value) {
        QString txt;
        value = value + 1;
        if (value % 2 == 1) {
                txt.sprintf("%1.1f", value / 2.0);
        } else {
                txt.sprintf("%d", value / 2);
        }

        this->ui->labelDifficulty->setText(txt);
}

void CacheEditDialog::onTerrChanged(int value) {
        QString txt;
        value = value + 1;
        if (value % 2 == 1) {
                txt.sprintf("%1.1f", value / 2.0);
        } else {
                txt.sprintf("%d", value / 2);
        }

        this->ui->labelTerrain->setText(txt);
}

void CacheEditDialog::onCbFoundClick(int state) {
        if (state == Qt::Checked) {
                this->ui->labelFoundDate->setEnabled(true);
                this->ui->labelFoundTime->setEnabled(true);
                this->ui->foundDate->setEnabled(true);
                this->ui->foundTime->setEnabled(true);
        } else {
                this->ui->labelFoundDate->setEnabled(false);
                this->ui->labelFoundTime->setEnabled(false);
                this->ui->foundDate->setEnabled(false);
                this->ui->foundTime->setEnabled(false);
        }
}

void CacheEditDialog::onLockAllClick(int state) {
        if (state == Qt::Checked) {
                this->ui->lockBasic->setChecked(true);
                this->ui->lockCoords->setChecked(true);
                this->ui->lockListing->setChecked(true);
                this->ui->lockStats->setChecked(true);
                this->ui->lockBasic->setEnabled(false);
                this->ui->lockCoords->setEnabled(false);
                this->ui->lockListing->setEnabled(false);
                this->ui->lockStats->setEnabled(false);
        } else {
                this->ui->lockBasic->setEnabled(true);
                this->ui->lockCoords->setEnabled(true);
                this->ui->lockListing->setEnabled(true);
                this->ui->lockStats->setEnabled(true);
        }
}

void CacheEditDialog::onCountryChanged(QString country) {
        QStringList &states = countrys[country];
        this->ui->comboState->clear();
        if (states.length() > 0) {
                QStringList::iterator i = states.begin();
                while (i != states.end()) {
                        this->ui->comboState->addItem(*i);
                        i++;
                }
        }
}

void CacheEditDialog::onAccepted() {
        cache->setName(this->ui->cacheName->text().toUtf8().data());

        switch (this->ui->comboStatus->currentIndex()) {
        case 1: cache->setCacheStatus(Geocache::ST_DISABLED); break;
        case 2: cache->setCacheStatus(Geocache::ST_ARCHIVED); break;
        default: cache->setCacheStatus(Geocache::ST_AVAILABLE); break;
        }

        cache->setPlacedBy(this->ui->placedBy->text().toUtf8().data());
        cache->setOwner(this->ui->ownerName->text().toUtf8().data());
        cache->setOwnerId(atoi(this->ui->ownerId->text().toAscii().data()));

        switch (this->ui->type->currentIndex()) {
        case 1: this->cache->setCacheType(Geocache::T_MULTI); break;
        case 2: this->cache->setCacheType(Geocache::T_UNKNOWN); break;
        case 3: this->cache->setCacheType(Geocache::T_LETTERBOX); break;
        case 4: this->cache->setCacheType(Geocache::T_EARTH); break;
        case 5: this->cache->setCacheType(Geocache::T_WHERIGO); break;
        case 6: this->cache->setCacheType(Geocache::T_VIRTUAL); break;
        case 7: this->cache->setCacheType(Geocache::T_EVENT); break;
        case 8: this->cache->setCacheType(Geocache::T_MEGAEVENT); break;
        case 9: this->cache->setCacheType(Geocache::T_CITO); break;
        case 10: this->cache->setCacheType(Geocache::T_WEBCAM); break;
        default: this->cache->setCacheType(Geocache::T_TRADITIONAL); break;
        }

        switch (this->ui->size->currentIndex()) {
        case 0: this->cache->setSize(Geocache::S_MICRO); break;
        case 1: this->cache->setSize(Geocache::S_SMALL); break;
        case 2: this->cache->setSize(Geocache::S_REGULAR); break;
        case 3: this->cache->setSize(Geocache::S_LARGE); break;
        case 4: this->cache->setSize(Geocache::S_OTHER); break;
        case 6: this->cache->setSize(Geocache::S_VIRTUAL); break;
        default: this->cache->setSize(Geocache::S_NOT_LISTED); break;
        }

        this->cache->setDifficulty(this->ui->sliderDifficulty->value() * 5 + 5);
        this->cache->setTerrain(this->ui->sliderTerrain->value() * 5 + 5);

        const QDate qdtCreated = this->ui->dateCreated->date();
        struct tm dtCreated;
        dtCreated.tm_sec = 0;
        dtCreated.tm_min = 0;
        dtCreated.tm_hour = 0;
        dtCreated.tm_mday = qdtCreated.day();
        dtCreated.tm_mon = qdtCreated.month() - 1;
        dtCreated.tm_year = qdtCreated.year() - 1900;
        this->cache->setDtCreated(mktime(&dtCreated));

        if (this->ui->comboCountry->currentIndex() >= 0) {
                this->cache->setCountry(this->ui->comboCountry->itemText(this->ui->comboCountry->currentIndex()).toUtf8().data());
        } else {
                this->cache->setCountry(this->ui->comboCountry->currentText().toUtf8().data());
        }

        if (this->ui->comboState->currentIndex() >= 0) {
                this->cache->setState(this->ui->comboState->itemText(this->ui->comboState->currentIndex()).toUtf8().data());
        } else {
                this->cache->setState(this->ui->comboState->currentText().toUtf8().data());
        }

        this->cache->setHint(this->ui->hint->document()->toPlainText().toUtf8().data());

        if (this->ui->cbFound->isChecked()) {
                QDate qdtFound = this->ui->foundDate->date();
                QTime qtmFound = this->ui->foundTime->time();

                struct tm dtFound;
                dtFound.tm_sec = 0;
                dtFound.tm_min = qtmFound.minute();
                dtFound.tm_hour = qtmFound.hour();
                dtFound.tm_mday = qdtFound.day();
                dtFound.tm_mon = qdtFound.month() - 1;
                dtFound.tm_year = qdtFound.year() - 1900;

                cache->setDtFound(mktime(&dtFound));
        }

        if (this->ui->cbMyOwn->isChecked()) {
            cache->setOwn(true);
        } else {
            cache->setOwn(false);
        }

        // Coordinates

        // Update initial coordinates
        Coords initial = this->ui->initialCoords->getCoords();
        cache->setInitialLatitudeE6(initial.dlat * 1E6);
        cache->setInitialLongitudeE6(initial.dlon * 1E6);

        // Update corrected coordinates, if checked.
        if (this->ui->correctedCoordsGroup->isChecked()) {
                cache->setHasCorrectedCoords(true);
                Coords c = this->ui->correctedCoords->getCoords();
                cache->setLatitudeE6(c.dlat * 1E6);
                cache->setLongitudeE6(c.dlon * 1E6);
        } else if (cache->hasCorrectedCoords()) {
                // Removing of corrected coords
                cache->setLatitudeE6(0);
                cache->setLongitudeE6(0);
        }

        // Locking
        if (this->ui->lockAll->isChecked()) {
                cache->setLockAll(true);
        } else {
                cache->setInfoLocked(this->ui->lockBasic->isChecked());
                cache->setCoordsLocked(this->ui->lockCoords->isChecked());
                cache->setListingLocked(this->ui->lockListing->isChecked());
                cache->setBasicLocked(this->ui->lockStats->isChecked());
        }

        // User note
        cache->setComment(this->ui->userNote->document()->toPlainText().toUtf8().data());

        // Listing
        cache->setShortDescription(this->ui->shortDesc->document()->toPlainText().toUtf8().data());
        cache->setSDIsHTML(this->ui->shortDescFlag->isChecked());
        cache->setLongDescription(this->ui->longDesc->document()->toPlainText().toUtf8().data());
        cache->setLDIsHTML(this->ui->longDescFlag->isChecked());

        // Attributes
        QMap<int, AttributeChecked *>::iterator ai = this->attributes.begin();
        GCM::GC<AttributeSet> as = cache->getAttributes();
        as->clear();

        while (ai != this->attributes.end()) {
                switch (ai.value()->checkState()) {
                case Qt::Checked:
                        as->append(new Attribute(ai.key(), true));
                        break;
                case Qt::Unchecked:
                        as->append(new Attribute(ai.key(), false));
                        break;
                default: break;
                }

                ai++;
        }

        // TODO: Tags

        // Save to database
        if (cache->getDbProvider()) {
                cache->getDbProvider()->saveCache(cache);
        }

        if (!this->ui->correctedCoordsGroup->isChecked()) {
                cache->setHasCorrectedCoords(false);
        }

        // Forgot long data...
        if (!wereLongLoaded) {
                cache->setLongLoaded(false);
        }

        if (!wereAttributesLoaded) {
                cache->setAttributesLoaded(false);
        }
}

void CacheEditDialog::onWptChanged(int index) {
        if (index > 0) {
                GCM::GC<GeocacheWaypoint> waypoint = this->waypoints->at(index - 1);
                Coords c;
                c.dlat = waypoint->getLatitude();
                c.dlon = waypoint->getLongitude();
                c.mlat = (c.dlat - (int)c.dlat) * 60;
                c.mlon = (c.dlon - (int)c.dlon) * 60;
                c.slat = (c.mlat - (int)c.mlat) * 60;
                c.slon = (c.mlon - (int)c.mlon) * 60;
                this->ui->correctedCoords->setCoords(c);
        }
}
