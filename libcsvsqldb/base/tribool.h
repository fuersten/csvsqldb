//
//  tribool.h
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

#include "libcsvsqldb/inc.h"


namespace csvsqldb
{
  /**
   * This class implements a 3-state boolean. It has no concrete status before calling the set method.
   */
  class CSVSQLDB_EXPORT Tribool
  {
  public:
    /**
     * Constructs a tribool instance. The state is indeterminate.
     */
    Tribool()
    : _state(false)
    , _isIndeterminate(true)
    {
    }

    Tribool(const Tribool&) = default;
    Tribool& operator=(const Tribool&) = default;
    Tribool(Tribool&&) = default;
    Tribool& operator=(Tribool&&) = default;

    /**
     * Compares two tribool objects.
     * @return false, if only one is indeterminate or if both are indeterminate. false, if both are not indeterminate and the
     * states
     * are not equal (e.g. one is true and the other false). true, if both are not indeterminate and the states are equal
     * (e.g. both
     * true or both false).
     */
    bool operator==(const Tribool& rhs) const
    {
      if (_isIndeterminate != rhs._isIndeterminate || _isIndeterminate) {
        return false;
      }

      return _state == rhs._state;
    }

    /**
     * Retrieves, if the tribool has a determined status and if it is true.
     * @return true, if the tribool is not indeterminate and set to true, otherwise false.
     */
    bool isTrue() const
    {
      return _isIndeterminate ? false : _state;
    }

    /**
     * Retrieves, if the tribool has a determined status and if it is false.
     * @return true, if the tribool is not indeterminate and set to false, otherwise false.
     */
    bool isFalse() const
    {
      return _isIndeterminate ? false : !_state;
    }

    /**
     * Retrieves, if the tribool is in an indeterminate status.
     * @return true, if the tribool is in an indeterminate status, otherwise false.
     */
    bool isIndeterminate() const
    {
      return _isIndeterminate;
    }

    /**
     * Sets the concrete status of the tribool. After setting the state, isIndeterminate will return false.
     * @param state The concrete status, that the tribool should have
     */
    void set(bool state)
    {
      _isIndeterminate = false;
      _state = state;
    }

    /**
     * Tests if the status of the tribool is true.
     * @return true, if the tribool is not indeterminate and set to true, otherwise false.
     */
    explicit operator bool() const
    {
      return isTrue();
    }

    /**
     * Tests if the status of the tribool is false.
     * @return true, if the tribool is not indeterminate and set to false, otherwise false.
     */
    bool operator!() const
    {
      return isFalse();
    }

  private:
    bool _state;
    bool _isIndeterminate;
  };
}