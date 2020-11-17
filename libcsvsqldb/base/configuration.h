//
//  configuration.h
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

#ifndef csvsqldb_configuration_h
#define csvsqldb_configuration_h

#include "libcsvsqldb/inc.h"

#include "types.h"

#include <memory>


namespace csvsqldb
{
  /**
   * Base class for configurations.
   */
  class CSVSQLDB_EXPORT Configuration
  {
  public:
    typedef std::shared_ptr<const Configuration> Ptr;

    virtual ~Configuration();

    Configuration(const Configuration&) = delete;
    Configuration(Configuration&&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    Configuration& operator=(Configuration&&) = delete;

    /**
     * Return all sub-properties of the given path.
     * @param path Dot separated path to the properties
     * @param properties Will be filled with the found properties
     * @return Returns the number of found properties.
     */
    size_t getProperties(const std::string& path, StringVector& properties) const;

    /**
     * Check if a configuration has a specific property.
     * @param path Dot separated path to the property
     * @return true if the configuration has the property, false otherwise.
     */
    bool hasProperty(const std::string& path) const;

    /**
     * Retrieve a configuration property of type bool with a possible default value.
     * @param path Dot separated path to the property
     * @param defaultValue Default value that gets returned, if the property was not found in the configuration
     * @return Returns the bool value of the configuration property or the defaultValue if not found.
     */
    bool get(const std::string& path, bool defaultValue) const;

    /**
     * Retrieve a configuration property of type int with a possible default value.
     * @param path Dot separated path to the property
     * @param defaultValue Default value that gets returned, if the property was not found in the configuration
     * @return Returns the int value of the configuration property or the defaultValue if not found.
     */
    int32_t get(const std::string& path, int32_t defaultValue) const;

    /**
     * Retrieve a configuration property of type long with a possible default value.
     * @param path Dot separated path to the property
     * @param defaultValue Default value that gets returned, if the property was not found in the configuration
     * @return Returns the long value of the configuration property or the defaultValue if not found.
     */
    int64_t get(const std::string& path, int64_t defaultValue) const;

    /**
     * Retrieve a configuration property of type float with a possible default value.
     * @param path Dot separated path to the property
     * @param defaultValue Default value that gets returned, if the property was not found in the configuration
     * @return Returns the float value of the configuration property or the defaultValue if not found.
     */
    float get(const std::string& path, float defaultValue) const;

    /**
     * Retrieve a configuration property of type double with a possible default value.
     * @param path Dot separated path to the property
     * @param defaultValue Default value that gets returned, if the property was not found in the configuration
     * @return Returns the double value of the configuration property or the defaultValue if not found.
     */
    double get(const std::string& path, double defaultValue) const;

    /**
     * Retrieve a configuration property of type string with a possible default value.
     * @param path Dot separated path to the property
     * @param defaultValue Default value that gets returned, if the property was not found in the configuration
     * @return Returns the string value of the configuration property or the defaultValue if not found.
     */
    std::string get(const std::string& path, const char* defaultValue) const;

    /**
     * Retrieve a configuration property of type string with a possible default value.
     * @param path Dot separated path to the property
     * @param defaultValue Default value that gets returned, if the property was not found in the configuration
     * @return Returns the string value of the configuration property or the defaultValue if not found.
     */
    std::string get(const std::string& path, const std::string& defaultValue) const;

    /**
     * Retrieve a configuration property according to its given template type.
     * Will throw a ConfigurationException if an error occurs, i.e. the property was not found or has the wrong type.
     * @param path Dot separated path to the value
     * @return Returns the value of the configuration property.
     */
    template<typename T>
    T get(const std::string& path) const
    {
      Typer<T> typer;
      return get(path, typer);
    }

  protected:
    Configuration();

  private:
    /**
     * Template method to return all sub-properties of the given path.
     * @param path Dot separated path to the properties
     * @param properties Will be filled with the found properties
     * @return Returns the number of found properties.
     */
    virtual size_t doGetProperties(const std::string& path, StringVector& properties) const = 0;

    /**
     * Template method for the inspection of properties.
     * @param path Dot separated path to the property
     * @return true if the configuration has the property, false otherwise.
     */
    virtual bool doHasProperty(const std::string& path) const = 0;

    /**
     * Template method for the generic fetching of a configuration property.
     * Has to be implemented by the inheriting class. Has to throw a CofigurationException upon any error. The path can be a
     * dot separated string, that stand for subentries of the property.
     * @param path Dot separated path to the value
     * @param typer The correct type object for the returning type
     * @return Returns the corresponding value.
     */
    virtual bool get(const std::string& path, Typer<bool> typer) const = 0;

    /**
     * Template method for the generic fetching of a configuration property.
     * Has to be implemented by the inheriting class. Has to throw a CofigurationException upon any error. The path can be a
     * dot separated string, that stand for subentries of the property.
     * @param path Dot separated path to the value
     * @param typer The correct type object for the returning type
     * @return Returns the corresponding value.
     */
    virtual int32_t get(const std::string& path, Typer<int32_t> typer) const = 0;

    /**
     * Template method for the generic fetching of a configuration property.
     * Has to be implemented by the inheriting class. Has to throw a CofigurationException upon any error. The path can be a
     * dot separated string, that stand for subentries of the property.
     * @param path Dot separated path to the value
     * @param typer The correct type object for the returning type
     * @return Returns the corresponding value.
     */
    virtual int64_t get(const std::string& path, Typer<int64_t> typer) const = 0;

    /**
     * Template method for the generic fetching of a configuration property.
     * Has to be implemented by the inheriting class. Has to throw a CofigurationException upon any error. The path can be a
     * dot separated string, that stand for subentries of the property.
     * @param path Dot separated path to the value
     * @param typer The correct type object for the returning type
     * @return Returns the corresponding value.
     */
    virtual float get(const std::string& path, Typer<float> typer) const = 0;

    /**
     * Template method for the generic fetching of a configuration property.
     * Has to be implemented by the inheriting class. Has to throw a CofigurationException upon any error. The path can be a
     * dot separated string, that stand for subentries of the property.
     * @param path Dot separated path to the value
     * @param typer The correct type object for the returning type
     * @return Returns the corresponding value.
     */
    virtual double get(const std::string& path, Typer<double> typer) const = 0;

    /**
     * Template method for the generic fetching of a configuration property.
     * Has to be implemented by the inheriting class. Has to throw a CofigurationException upon any error. The path can be a
     * dot separated string, that stand for subentries of the property.
     * @param path Dot separated path to the value
     * @param typer The correct type object for the returning type
     * @return Returns the corresponding value.
     */
    virtual std::string get(const std::string& path, Typer<std::string> typer) const = 0;
  };
}

#endif
