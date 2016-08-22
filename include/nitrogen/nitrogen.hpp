#pragma once

#if _MSC_VER
//	#pragma warning ( disable : 4251 )
#if defined(NITROGEN_EXPORTS)
#define NITROGEN_API __declspec(dllexport)
#else
#define NITROGEN_API __declspec(dllimport)
#endif
#elif __GNUC__ >= 4
# define NITROGEN_API __attribute__((visibility("default")))
#else
#define NITROGEN_API // nothing, symbols in a shared library are exported by default
#endif

#include "aligned_allocator.hpp"
#include "geometry.hpp"
#include "nodeutil.hpp"
#include "threads.hpp"
