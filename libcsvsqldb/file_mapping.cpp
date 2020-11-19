//
//  file_mapping.cpp
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

#include "file_mapping.h"

#include "base/json.h"
#include "base/string_helper.h"

#include <fstream>
#include <regex>

namespace csvsqldb
{
  CSVSQLDB_IMPLEMENT_EXCEPTION(MappingException, csvsqldb::Exception);


  void FileMapping::initialize(const Mappings& mapping)
  {
    static std::regex r(R"((.+)->([a-zA-Z_]+))");

    for (const auto& keyValue : mapping) {
      std::smatch match;
      if (!regex_match(keyValue._mapping, match, r)) {
        CSVSQLDB_THROW(MappingException, "not a valid file to table mapping '" << keyValue._mapping << "'");
      } else {
        Mapping map = {match.str(1), keyValue._delimiter, keyValue._skipFirstLine};
        _fileTableMapping.insert(std::make_pair(csvsqldb::toupper_copy(match.str(2)), map));
      }
    }
  }

  bool FileMapping::addMapping(const std::string& tableName, const Mapping& mapping)
  {
    const auto iter = _fileTableMapping.find(csvsqldb::toupper_copy(tableName));
    if (iter == _fileTableMapping.end()) {
      _fileTableMapping.insert(std::make_pair(csvsqldb::toupper_copy(tableName), mapping));
      return true;
    }
    return false;
  }

  void FileMapping::removeMapping(const std::string& tableName)
  {
    auto iter = _fileTableMapping.find(csvsqldb::toupper_copy(tableName));
    if (iter != _fileTableMapping.end()) {
      _fileTableMapping.erase(iter);
    }
  }

  const Mapping& FileMapping::getMappingForTable(const std::string& tableName) const
  {
    const auto iter = _fileTableMapping.find(csvsqldb::toupper_copy(tableName));
    if (iter == _fileTableMapping.end()) {
      CSVSQLDB_THROW(MappingException, "no mapping found for table '" << tableName << "'");
    }
    return iter->second;
  }

  csvsqldb::StringVector FileMapping::asStringVector() const
  {
    csvsqldb::StringVector mapping;

    for (const auto& keyValue : _fileTableMapping) {
      mapping.push_back(keyValue.second._mapping + "->" + keyValue.first);
    }

    return mapping;
  }

  void FileMapping::mergeMapping(const FileMapping& mappings)
  {
    for (const auto& mapping : mappings._fileTableMapping) {
      addMapping(mapping.first, mapping.second);
    }
  }

  FileMapping FileMapping::fromJson(std::istream& stream)
  {
    try {
      json j;
      stream >> j;

      FileMapping fileMapping;

      auto mapping = j["Mapping"];
      auto tableName = mapping["name"].get<std::string>();
      for (const auto& element : mapping["mappings"]) {
        Mapping map;
        map._mapping = element["pattern"].get<std::string>();
        map._delimiter = element["delimiter"].get<std::string>()[0];
        map._skipFirstLine = element["skipFirstLine"].get<bool>();
        fileMapping.addMapping(tableName, map);
      }

      return fileMapping;
    } catch (const std::exception& ex) {
      CSVSQLDB_THROW(JsonException, "Not a valid file mapping: " << ex.what());
    }
  }

  std::string FileMapping::asJson(const std::string& tableName) const
  {
    auto mapping = getMappingForTable(tableName);

    json j;
    j["Mapping"]["name"] = csvsqldb::toupper_copy(tableName);

    json mapping_array = json::array();
    json entry{{"pattern", mapping._mapping},
               {"delimiter", std::string(1, mapping._delimiter)},
               {"skipFirstLine", mapping._skipFirstLine}};
    mapping_array.push_back(entry);
    j["Mapping"]["mappings"] = mapping_array;

    std::stringstream ss;
    ss << std::setw(2) << j << std::endl;
    return ss.str();
  }

  void FileMapping::readFromPath(FileMapping& fileMapping, const fs::path& path)
  {
    std::vector<fs::path> entries;
    std::copy(fs::directory_iterator(path), fs::directory_iterator(), std::back_inserter(entries));

    for (const auto& entry : entries) {
      std::string mappingEntry = entry.string();
      std::ifstream mappingStream(mappingEntry);

      FileMapping mapping = FileMapping::fromJson(mappingStream);
      fileMapping.mergeMapping(mapping);
    }
  }
}
