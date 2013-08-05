#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MLITE_LIBRARY)
#  define MLITESHARED_EXPORT Q_DECL_EXPORT
#else
#  define MLITESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MLITE_GLOBAL_H
