#ifndef PLUGIN-UI-QT4_GLOBAL_H
#define PLUGIN-UI-QT4_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef PLUGIN-UI-QT4_LIB
# define PLUGIN-UI-QT4_EXPORT Q_DECL_EXPORT
#else
# define PLUGIN-UI-QT4_EXPORT Q_DECL_IMPORT
#endif

#endif // PLUGIN-UI-QT4_GLOBAL_H
