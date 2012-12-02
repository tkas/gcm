# -------------------------------------------------
# Project created by QtCreator 2012-03-21T23:04:35
# -------------------------------------------------
TEMPLATE = lib
TARGET_EXT = 
CONFIG += plugin
INCLUDEPATH += ../../../../../include
QT += webkit
FORMS += forms/MainWindow.ui \
    forms/StartupDialog.ui \
    forms/ProgressDialog.ui \
    forms/WaypointDialog.ui \
    forms/FilterDialog.ui \
    forms/AdvancedFilterModifyDialog.ui \
    forms/CacheEditDialog.ui \
    widgets/CoordinatesWidget.ui \
    widgets/AttributeChecked.ui \
    forms/CacheDetailsDialog.ui \
    forms/ExportDialog.ui \
    forms/AboutDialog.ui \
    forms/CreateDatabaseDialog.ui \
    forms/DatabaseSelectDialog.ui \
    forms/DatabaseRenameDialog.ui \
    forms/SetFoundDialog.ui \
    forms/SettingsDialog.ui \
    forms/AddRefPointDialog.ui \
    forms/AddOtherNickDialog.ui \
    forms/SearchDialog.ui
SOURCES += main.cpp \
    forms/MainWindow.cpp \
    forms/StartupDialog.cpp \
    Qt4Ui.cpp \
    Qt4DialogBuilder.cpp \
    Qt4StartupDialog.cpp \
    forms/ProgressDialog.cpp \
    Qt4ProgressDialog.cpp \
    widgets/CacheTreeView.cpp \
    forms/WaypointDialog.cpp \
    widgets/NumberLineEdit.cpp \
    forms/FilterDialog.cpp \
    forms/AdvancedFilterModifyDialog.cpp \
    widgets/QSelectAllLineEdit.cpp \
    models/CacheTreeItem.cpp \
    models/CachesViewModel.cpp \
    forms/CacheEditDialog.cpp \
    widgets/CoordinatesWidget.cpp \
    widgets/AttributeChecked.cpp \
    FlowLayout.cpp \
    forms/CacheDetailsDialog.cpp \
    QtGeolib.cpp \
    widgets/GCMWebView.cpp \
    forms/ExportDialog.cpp \
    forms/AboutDialog.cpp \
    tasks/LoaderTask.cpp \
    tasks/InitTask.cpp \
    ListFilter.cpp \
    CacheTreeDelegate.cpp \
    models/FilterItemModel.cpp \
    tasks/ImportTask.cpp \
    tasks/ExportTask.cpp \
    forms/CreateDatabaseDialog.cpp \
    forms/DatabaseSelectDialog.cpp \
    forms/DatabaseRenameDialog.cpp \
    tasks/SwitchDatabaseTask.cpp \
    forms/SetFoundDialog.cpp \
    forms/SettingsDialog.cpp \
    forms/AddRefPointDialog.cpp \
    forms/AddOtherNickDialog.cpp \
    models/DatabaseSelectModel.cpp \
    models/ReferencePointModel.cpp \
    forms/SearchDialog.cpp
HEADERS += forms/MainWindow.h \
    forms/StartupDialog.h \
    main.h \
    Qt4Ui.h \
    Qt4DialogBuilder.h \
    Qt4StartupDialog.h \
    models/CachesViewModel.h \
    models/CacheTreeItem.h \
    forms/ProgressDialog.h \
    Qt4ProgressDialog.h \
    widgets/CacheTreeView.h \
    forms/WaypointDialog.h \
    widgets/NumberLineEdit.h \
    forms/FilterDialog.h \
    forms/AdvancedFilterModifyDialog.h \
    widgets/QSelectAllLineEdit.h \
    forms/CacheEditDialog.h \
    widgets/CoordinatesWidget.h \
    widgets/AttributeChecked.h \
    FlowLayout.h \
    forms/CacheDetailsDialog.h \
    QtGeolib.h \
    widgets/GCMWebView.h \
    forms/ExportDialog.h \
    forms/AboutDialog.h \
    tasks/LoaderTask.h \
    tasks/InitTask.h \
    ListFilter.h \
    CacheTreeDelegate.h \
    models/FilterItemModel.h \
    tasks/ImportTask.h \
    tasks/ExportTask.h \
    forms/CreateDatabaseDialog.h \
    forms/DatabaseSelectDialog.h \
    forms/DatabaseRenameDialog.h \
    tasks/SwitchDatabaseTask.h \
    forms/SetFoundDialog.h \
    forms/SettingsDialog.h \
    forms/AddRefPointDialog.h \
    forms/AddOtherNickDialog.h \
    models/DatabaseSelectModel.h \
    models/ReferencePointModel.h \
    forms/SearchDialog.h \
    CommandAdapter.h \
    CommandMenuAdapter.h
RESOURCES += res/icons.qrc \
    res/attributes.qrc \
    res/html.qrc \
    res/stars.qrc \
    res/graphic.qrc
