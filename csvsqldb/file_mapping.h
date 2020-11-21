//
//  file_mapping.h
//  csvsqldb
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

#pragma once

#include "csvsqldb/inc.h"

#include "base/exception.h"
#include "base/types.h"

#include <filesystem>
#include <map>


namespace csvsqldb
{
  namespace fs = std::filesystem;

  CSVSQLDB_DECLARE_EXCEPTION(MappingException, csvsqldb::Exception);


  struct CSVSQLDB_EXPORT Mapping {
    std::string _mapping;
    char _delimiter;
    bool _skipFirstLine;
  };


  class CSVSQLDB_EXPORT FileMapping
  {
  public:
    using Mappings = std::vector<Mapping>;

    FileMapping() = default;

    void initialize(const Mappings& mapping);

    const Mapping& getMappingForTable(const std::string& tableName) const;

    void mergeMapping(const FileMapping& mappings);

    csvsqldb::StringVector asStringVector() const;

    void removeMapping(const std::string& tableName);

    static FileMapping fromJson(std::istream& stream);

    std::string asJson(const std::string& tableName) const;

    static void readFromPath(FileMapping& fileMapping, const fs::path& path);

  private:
    bool addMapping(const std::string& tableName, const Mapping& mapping);

    std::map<std::string, Mapping> _fileTableMapping;
  };
}
