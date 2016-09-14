//
// Copyright (c) 2011 - 2015 ASPECTRON Inc.
// All Rights Reserved.
//
// This file is part of JSX (https://github.com/aspectron/jsx) project.
//
// Distributed under the MIT software license, see the accompanying
// file LICENSE
//
#ifndef JSX_OS_HPP_INCLUDED
#define JSX_OS_HPP_INCLUDED

#include <experimental/filesystem>

#include "nitrogen/nitrogen.hpp"

namespace aspect {

namespace filesystem = std::experimental::filesystem;

namespace os {

NITROGEN_API void set_thread_name(char const* name);

/// Get executable path
NITROGEN_API filesystem::path exe_path();

}} // aspect::os

#endif //JSX_OS_HPP_INCLUDED
