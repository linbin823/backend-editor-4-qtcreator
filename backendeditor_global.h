#ifndef BACKENDEDITOR_GLOBAL_H
#define BACKENDEDITOR_GLOBAL_H

#if defined(BACKENDEDITOR_LIBRARY)
#  define BACKENDEDITOR_EXPORT Q_DECL_EXPORT
#else
#  define BACKENDEDITOR_EXPORT Q_DECL_IMPORT
#endif

#endif // BACKENDEDITOR_GLOBAL_H
