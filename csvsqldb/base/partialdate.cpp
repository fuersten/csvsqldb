/***************************************************************************
 *                                                                         *
 *   Copyright (c) 2004-2007 by Athelas GmbH. All Rights Reserved.         *
 *                                                                         *
 *   The contents of this file are subject to the Mozilla Public License   *
 *   Version 1.1 (the "License"); you may not use this file except in      *
 *   compliance with the License. You may obtain a copy of the License at  *
 *   http://www.mozilla.org/MPL/                                           *
 *                                                                         *
 *   Software distributed under the License is distributed on an "AS IS"   *
 *   basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See   *
 *   the License for the specificlanguage governing rights and             *
 *   limitations under the License.                                        *
 *                                                                         *
 *   The Original Code is mpf.                                             *
 *                                                                         *
 *   The Initial Developer of the Original Code is Lars-Christian          *
 *   Fuerstenberg, with contributions from Steffen Schuemann. Portions     *
 *   created by Lars-Christian Fuerstenberg are Copyright (c) 2002-2003    *
 *   All Rights Reserved.                                                  *
 *                                                                         *
 ***************************************************************************/

#include <mpfcore/partialdate.h>

using namespace mpf;


PartialDate::PartialDate()
: _day(1)
, _month(1)
{
}

PartialDate::PartialDate(UInt8 day, UInt8 month)
: _day(day)
, _month(month)
{
  MPF_THROW_IF((_month < 1 || _month > 12) || (_day < 1 && _day > 31), DateException,
               ("Invalid month/day specified: month %d, day %d") % month % day);
}

mpf::Date PartialDate::date(UInt16 year) const
{
  return Date(year, _month, _day);
}
