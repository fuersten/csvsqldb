//
//  csvsqldb test
//
//  BSD 3-Clause License
//  Copyright (c) 2015-2020 Lars-Christian Fürstenberg
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted
//  provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//  conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of
//  conditions and the following disclaimer in the documentation and/or other materials provided
//  with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors may be used to
//  endorse or promote products derived from this software without specific prior written
//  permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//

#include "temporary_directory.h"

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#else
  #include <limits.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <unistd.h>
#endif

#include <array>
#include <cstring>


const std::filesystem::path& TemporaryDirectoryGuard::temporaryDirectoryPath() const
{
  return _path;
}

TemporaryDirectoryGuard::~TemporaryDirectoryGuard()
{
  std::error_code ec;
  if (std::filesystem::equivalent(std::filesystem::current_path(), _path, ec)) {
    std::filesystem::current_path("..");
  }

  if (!ec) {
    std::filesystem::remove_all(_path, ec);
  }
}


std::filesystem::path TemporaryDirectoryGuard::uniqueTempDirectoryPath()
{
  std::error_code ec;
  auto path = uniqueTempDirectoryPath(ec);
  if (ec) {
    throw std::runtime_error{"Cannot create temp path: " + ec.message()};
  }
  return path;
}

std::filesystem::path TemporaryDirectoryGuard::uniqueTempDirectoryPath(std::error_code& ec)
{
  std::filesystem::path p{std::filesystem::temp_directory_path(ec)};

  if (ec) {
    return std::filesystem::path{};
  }

#ifdef WIN32
  WCHAR tmp_dir[MAX_PATH];
  if (GetTempFileNameW(p.c_str(), L"csvsqldb", 0, tmp_dir) == 0) {
    ec.assign(GetLastError(), std::system_category());
  }
  std::filesystem::remove(tmp_dir);
  std::filesystem::create_directories(tmp_dir);
#else
  std::array<char, PATH_MAX> tmpl = {};
  ::strncpy(tmpl.data(), (p / "csvsqldb_XXXXXX").c_str(), PATH_MAX);

  const char* tmp_dir = ::mkdtemp(tmpl.data());
  if (tmp_dir == nullptr) {
    ec.assign(errno, std::system_category());
  }
#endif

  return std::filesystem::path{tmp_dir};
}
