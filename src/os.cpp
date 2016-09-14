//
// Copyright (c) 2011 - 2015 ASPECTRON Inc.
// All Rights Reserved.
//
// This file is part of JSX (https://github.com/aspectron/jsx) project.
//
// Distributed under the MIT software license, see the accompanying
// file LICENSE
//
#include "nitrogen/os.hpp"

#include <cassert>

namespace aspect { namespace os {

filesystem::path resolve(filesystem::path const& p);

filesystem::path exe_path()
{
// http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe

	filesystem::path result;
#if defined(_WIN32)
	std::string buf(2048, 0);
	size_t size = ::GetModuleFileNameA(NULL, &buf[0], static_cast<DWORD>(buf.size()));
	if (size > buf.size())
	{
		buf.resize(size + 1);
		size = ::GetModuleFileNameA(NULL, &buf[0], static_cast<DWORD>(buf.size()));
	}
	buf.resize(size);
	result = buf.c_str();
#elif defined(__linux__)
	result = filesystem::read_symlink("/proc/self/exe");
#elif defined(__FreeBSD__)
	int mib[4];
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PATHNAME;
	mib[3] = -1;
	std::string buf(2048, 0);
	size_t size = buf.size();
	if (sysctl(mib, 4, &buf[0], &size, NULL, 0) == -1)
	{
		buf.resize(size + 1);
		sysctl(mib, 4, &buf[0], &size, NULL, 0)
	}
	buf.resize(size);
	result = buf.c_str();
#elif defined(__APPLE__)
	std::string buf(2048, 0);
	uint32_t size = static_cast<uint32_t>(buf.size());
	if ( _NSGetExecutablePath(&buf[0], &size) == -1 )
	{
		buf.resize(size + 1);
		_NSGetExecutablePath(&buf[0], &size);
	}
	buf.resize(size);
	result = buf.c_str();
#else
#error Not implemented yet
#endif
	return resolve(result);
}

filesystem::path resolve(filesystem::path const& p)
{
	filesystem::path result;
	for (auto const& part : p)
	{
		if (part == "..")
		{
			// /a/b/.. is not necessarily /a if b is a symbolic link
			if (filesystem::is_symlink(result) )
				result /= part;
			// /a/b/../.. is not /a/b/.. under most circumstances
			// We can end up with ..s in our result because of symbolic links
			else if (result.filename() == "..")
				result /= part;
			// Otherwise it should be safe to resolve the parent
			else
				result = result.parent_path();
		}
		else if (part == ".")
		{
			// Ignore
		}
		else
		{
			// Just cat other path entries
			result /= part;
		}
	}
	result.make_preferred();
	return result;
}

#if defined(_WIN32)
//
// Usage: _set_thread_name(-1, "MainThread");
//
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // must be 0x1000
	LPCSTR szName; // pointer to name (in user addr space)
	DWORD dwThreadID; // thread ID (-1=caller thread)
	DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;
#pragma pack(pop)

static void _set_thread_name(uint32_t thread_id, const char *thread_name)
{
	// thread name should be max 32 characters
	// and should contain thread name abbreviation
	assert(strlen(thread_name) < 32);

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = thread_name;
	info.dwThreadID = (DWORD)thread_id;
	info.dwFlags = 0;

	__try
	{
		RaiseException( 0x406D1388, 0, /*sizeof(info)/sizeof(DWORD)*/ 4, (const ULONG_PTR*)&info );
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}

void set_thread_name(const char *thread_name)
{
	_set_thread_name(GetCurrentThreadId(), thread_name);
}

#elif defined(__linux__)
void set_thread_name(const char *thread_name)
{
#if __GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 12)
	pthread_setname_np(pthread_self(), thread_name);
#endif
}
#elif defined(__APPLE__)
void set_thread_name(const char *thread_name)
{
	pthread_setname_np(thread_name);
}
#else
void set_thread_name(const char *thread_name) { }
#endif

}} // aspect::os
