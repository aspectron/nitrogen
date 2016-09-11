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

// timestamp in millseconds
inline double get_ts()
{
	using namespace std::chrono;
	typedef high_resolution_clock clock;

	clock::duration const since_epoch = clock::now().time_since_epoch();
	return duration_cast<duration<double>>(since_epoch).count() * 1000.0;
}
