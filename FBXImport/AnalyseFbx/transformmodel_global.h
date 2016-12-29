#ifndef _TRANSFORM_MODEL_GOLBAL_H
#define _TRANSFORM_MODEL_GOLBAL_H

#include <QtCore/qglobal.h>

#ifndef QT_STATIC
# if defined(TRANSFORMMODEL_LIB)
#  define TRANSFORMMODEL_EXPORT Q_DECL_EXPORT
# else
#  define TRANSFORMMODEL_EXPORT Q_DECL_IMPORT
# endif
#else
# define TRANSFORMMODEL_EXPORT
#endif


#endif