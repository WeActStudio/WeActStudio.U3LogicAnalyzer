#pragma once

#include <QtGlobal>

/* FLUENTUI3STYLE_STATIC: added locally, see UPSTREAM.md. Upstream knows
 * the shared library and the plugin case only, but this copy is built as
 * a static library and linked into the application, where an import or
 * export declaration would leave the symbols unresolved. */
#if defined(FLUENTUI3STYLE_STATIC)
#  define FLUENTUI3STYLE_EXPORT
#elif defined(FLUENTUI3STYLE_LIBRARY)
#  define FLUENTUI3STYLE_EXPORT Q_DECL_EXPORT
#else
#  define FLUENTUI3STYLE_EXPORT Q_DECL_IMPORT
#endif
