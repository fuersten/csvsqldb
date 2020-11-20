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

#include <mpfcore/dategenerators.h>

using namespace mpf;


NthkDayOfMonth::NthkDayOfMonth(enkDay kDay, Date::enDay dayOfWeek, UInt8 month)
: _kDay(kDay)
, _dayOfWeek(dayOfWeek)
, _month(month)
{
  MPF_THROW_IF(_month < 1 || _month > 12, DateException, ("Invalid month specified: month %d") % month);
}

Date NthkDayOfMonth::date(UInt16 year) const
{
  Date date(year, _month, 1);
  while (_dayOfWeek != date.dayOfWeek()) {
    date.addDays(1);
  }
  int week = 1;
  while (week < _kDay) {
    date.addDays(7);
    ++week;
  }
  return date;
}

LastkDayOfMonth::LastkDayOfMonth(Date::enDay dayOfWeek, UInt8 month)
: _dayOfWeek(dayOfWeek)
, _month(month)
{
  MPF_THROW_IF(_month < 1 && _month > 12, DateException, ("Invalid month specified: month %d") % month);
}

Date LastkDayOfMonth::date(UInt16 year) const
{
  Date date(year, _month, Date::maximumDayInMonthFor(year, _month));
  while (_dayOfWeek != date.dayOfWeek()) {
    date.addDays(-1);
  }
  return date;
}

FirstkDayOfMonth::FirstkDayOfMonth(Date::enDay dayOfWeek, UInt8 month)
: _dayOfWeek(dayOfWeek)
, _month(month)
{
  MPF_THROW_IF(_month < 1 && _month > 12, DateException, ("Invalid month specified: month %d") % month);
}

Date FirstkDayOfMonth::date(UInt16 year) const
{
  Date date(year, _month, 1);
  while (_dayOfWeek != date.dayOfWeek()) {
    date.addDays(1);
  }
  return date;
}

FirstkDayAfter::FirstkDayAfter(Date::enDay dayOfWeek)
: _dayOfWeek(dayOfWeek)
{
}

Date FirstkDayAfter::date(Date start) const
{
  start.addDays(1);
  while (_dayOfWeek != start.dayOfWeek()) {
    start.addDays(1);
  }
  return start;
}

FirstkDayBefore::FirstkDayBefore(Date::enDay dayOfWeek)
: _dayOfWeek(dayOfWeek)
{
}

Date FirstkDayBefore::date(Date start) const
{
  start.addDays(-1);
  while (_dayOfWeek != start.dayOfWeek()) {
    start.addDays(-1);
  }
  return start;
}

CalculateGregorianEaster::CalculateGregorianEaster(UInt16 year)
{
  UInt16 H = (24 + 19 * (year % 19)) % 30;
  UInt16 I = H - (H / 28);
  UInt16 J = (year + (year / 4) + I - 13) % 7;
  Int16 L = I - J;
  UInt16 EM = 3 + ((L + 40) / 44);
  UInt16 ED = L + 28 - 31 * (EM / 4);
  _easterSunday = Date(year, EM, ED);
}

Date CalculateGregorianEaster::easterSunday()
{
  return _easterSunday;
}

NthDayOfGregorianEaster::NthDayOfGregorianEaster()
: _days(0)
{
}

NthDayOfGregorianEaster::NthDayOfGregorianEaster(int days)
: _days(days)
{
}

Date NthDayOfGregorianEaster::date(UInt16 year) const
{
  CalculateGregorianEaster easter(year);
  return easter.easterSunday().addDays(_days);
}
