//
//  glob.cpp
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

#include "base/glob.h"

#include "base/exception.h"

#include <wordexp.h>


namespace csvsqldb
{
  static std::string getError(int no)
  {
    switch (no) {
      case WRDE_BADCHAR:
        return "Illegal char in pattern";
      case WRDE_BADVAL:
        return "Undefined variable reference";
      case WRDE_CMDSUB:
        return "Command substitution occurred";
      case WRDE_NOSPACE:
        return "Out of memory";
      case WRDE_SYNTAX:
        return "Shell syntax error";
    }
    return "Undefined error";
  }

  size_t expand(const std::string& pattern, StringVector& files)
  {
    wordexp_t p;
    auto res = wordexp(pattern.c_str(), &p, WRDE_UNDEF | WRDE_NOCMD);
    if (res != 0) {
      CSVSQLDB_THROW(csvsqldb::Exception, "could not expand word: " << getError(res));
    }
    for (size_t i = 0; i < p.we_wordc; ++i) {
      files.push_back(p.we_wordv[i]);
    }
    wordfree(&p);
    return files.size();
  }
}
