//
//  symboltable.h
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

#include <csvsqldb/inc.h>

#include <csvsqldb/block_producer.h>
#include <csvsqldb/database.h>


namespace csvsqldb
{
  class DataProvider
  {
  public:
    virtual ~DataProvider() = default;

    virtual bool produce(BlockProducer& producer) = 0;
  };


  class SystemDualDataProvider : public DataProvider
  {
  public:
    SystemDualDataProvider() = default;

    bool produce(BlockProducer& producer) override;
  };


  class SystemTablesDataProvider : public DataProvider
  {
  public:
    SystemTablesDataProvider(Database& database);

    bool produce(BlockProducer& producer) override;

  private:
    Database& _database;
  };


  class SystemColumnsDataProvider : public DataProvider
  {
  public:
    SystemColumnsDataProvider(Database& database);

    bool produce(BlockProducer& producer) override;

  private:
    Database& _database;
  };


  class SystemFunctionsDataProvider : public DataProvider
  {
  public:
    SystemFunctionsDataProvider(Database& database);

    bool produce(BlockProducer& producer) override;
  };


  class SystemParametersDataProvider : public DataProvider
  {
  public:
    SystemParametersDataProvider(Database& database);

    bool produce(BlockProducer& producer) override;
  };


  class SystemMappingsDataProvider : public DataProvider
  {
  public:
    SystemMappingsDataProvider(Database& database);

    bool produce(BlockProducer& producer) override;

  private:
    Database& _database;
  };
}
