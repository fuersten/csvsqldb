//
//  file_mapping.cpp
//  csvsqldb
//
//  BSD 3-Clause License
//  Copyright (c) 2015, Lars-Christian Fürstenberg
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

#include "base/json_object.h"
#include "base/string_helper.h"
#include <boost/regex.hpp>

#include <fstream>

namespace csvsqldb
{
  CSVSQLDB_IMPLEMENT_EXCEPTION(MappingException, csvsqldb::Exception);


  FileMapping::FileMapping()
  {
  }

  void FileMapping::initialize(const Mappings& mapping)
  {
    boost::regex r(R"((.+)->([a-zA-Z_]+))");
    for (const auto& keyValue : mapping) {
      boost::smatch match;
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
    FileTableMapping::const_iterator iter = _fileTableMapping.find(csvsqldb::toupper_copy(tableName));
    if (iter == _fileTableMapping.end()) {
      _fileTableMapping.insert(std::make_pair(csvsqldb::toupper_copy(tableName), mapping));
      return true;
    }
    return false;
  }

  void FileMapping::removeMapping(const std::string& tableName)
  {
    FileTableMapping::iterator iter = _fileTableMapping.find(csvsqldb::toupper_copy(tableName));
    if (iter != _fileTableMapping.end()) {
      _fileTableMapping.erase(iter);
    }
  }

  const Mapping& FileMapping::getMappingForTable(const std::string& tableName) const
  {
    FileTableMapping::const_iterator iter = _fileTableMapping.find(csvsqldb::toupper_copy(tableName));
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
    std::shared_ptr<csvsqldb::json::JsonObjectCallback> callback = std::make_shared<csvsqldb::json::JsonObjectCallback>();
    csvsqldb::json::Parser parser(stream, callback);
    parser.parse();
    const csvsqldb::json::JsonObject& obj = callback->getObject();
    csvsqldb::json::JsonObject table = obj["Mapping"];
    std::string tableName = table["name"].getAsString();

    FileMapping fileMapping;

    const csvsqldb::json::JsonObject::ObjectArray& mappings = table["mappings"].getArray();
    for (const auto& mapping : mappings) {
      Mapping map;
      map._mapping = mapping["pattern"].getAsString();
      map._delimiter = mapping["delimiter"].getAsString()[0];
      map._skipFirstLine = mapping["skipFirstLine"].getAsBool();
      fileMapping.addMapping(tableName, map);
    }

    return fileMapping;
  }

  std::string FileMapping::asJson(const std::string& tableName, const Mappings& mappings)
  {
    boost::regex r(R"((.+)->([a-zA-Z_]+))");
    std::stringstream mapping;

    mapping << "{ \"Mapping\" :\n  { \"name\" : \"" << csvsqldb::toupper_copy(tableName) << "\",\n    \"mappings\" : [\n";
    int n = 0;
    for (const auto& map : mappings) {
      boost::smatch match;
      if (!regex_match(map._mapping, match, r)) {
        CSVSQLDB_THROW(MappingException, "not a mapping expression '" << map._mapping << "'");
      }
      std::string mappingTableName = csvsqldb::toupper_copy(match.str(2));
      std::string mappingPattern = match.str(1);

      if (mappingTableName == csvsqldb::toupper_copy(tableName)) {
        if (n > 0) {
          mapping << ",\n";
        }
        mapping << "{ \"pattern\" : ";
        std::stringstream entry;
        for (const auto& c : mappingPattern) {
          if (c == '\\') {
            entry << '\\';
          }
          entry << c;
        }
        mapping << "    "
                << "\"" << entry.str() << "\"";
        mapping << ", \"delimiter\" : \"" << map._delimiter << "\"";
        mapping << ", \"skipFirstLine\" :" << (map._skipFirstLine ? "true" : "false");
        mapping << "}\n";
        ++n;
      }
    }
    mapping << "\n    ]";
    mapping << "\n  }\n}";

    return mapping.str();
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
