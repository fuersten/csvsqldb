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

#ifndef MPF_DATEGENERATORS_H
#define MPF_DATEGENERATORS_H

#include <mpfcore/inc.h>

#include <mpfcore/date.h>


namespace mpf
{

/** This dategenerator calculates the date of the first to fifth weekday of the specified 
 *  month for a given year.
 *  Example:
 *  @code
 *  NthkDayOfMonth mothersday(NthkDayOfMonth::second, Date::Sunday, 5);
 *  @endcode
 *  This generator calculates the german mother's day, which is always on the second
 *  sunday of May.
 */

class MPFCORE_EXPORTS NthkDayOfMonth
{
public:
    enum enkDay
    {
        first=1, /**< First k-day of month */
        second,  /**< Second k-day of month */
        third,   /**< Third k-day of month */
        fourth,  /**< Fourth k-day of month */
        fifth    /**< Fifth k-day of month */
    };

    /** Constructs a dategenerator.
     *  If an invalid month is specified a DateException is thrown.
     *  @param kDay Specifies the first to fifth k-day of the month.
     *  @param dayOfWeek Specifies the day of the week.
     *  @param month Specifies the month.
     */
    NthkDayOfMonth(enkDay kDay, Date::enDay dayOfWeek, UInt8 month);
    /** Calculates the corresponding date using the given year.
     *  @param year The year for which the calculation shall be done.
     *  @return The correspondig date.
     */
    Date date(UInt16 year) const;

protected:
    enkDay _kDay;
    Date::enDay _dayOfWeek;
    UInt8 _month;
};

/** This dategenerator calculates the date of the last weekday of the specified month 
 *  for a given year.
 *  Example:
 *  @code
 *  LastkDayOfMonth summerTime(Date::Sunday, 3);
 *  @endcode
 *  This generator calculates the start of the european summer time, which is
 *  always on the last sunday of March.
 */
class MPFCORE_EXPORTS LastkDayOfMonth
{
public:
    /** Constructs a dategenerator.
     *  If an invalid month is specified a DateException is thrown.
     *  @param dayOfWeek Specifies the day of the week.
     *  @param month Specifies the month.
     */
    LastkDayOfMonth(Date::enDay dayOfWeek, UInt8 month);
    /** Calculates the corresponding date using the given year.
     *  @param year The year for which the calculation shall be done.
     *  @return The correspondig date.
     */
    Date date(UInt16 year) const;

protected:
    Date::enDay _dayOfWeek;
    UInt8 _month;
};

/** This dategenerator calculates the date of the first weekday of the specified month 
 *  for a given year.
 *  Example:
 *  @code
 *  FirstkDayOfMonth fkdom(Date::Monday, 9);
 *  @endcode
 *  This generator calculates the first monday of September.
 */
class MPFCORE_EXPORTS FirstkDayOfMonth
{
public:
    /** Constructs a dategenerator.
     *  If an invalid month is specified a DateException is thrown.
     *  @param dayOfWeek Specifies the day of the week.
     *  @param month Specifies the month.
     */
    FirstkDayOfMonth(Date::enDay dayOfWeek, UInt8 month);
    /** Calculates the corresponding date using the given year.
     *  @param year The year for which the calculation shall be done.
     *  @return The correspondig date.
     */
    Date date(UInt16 year) const;

protected:
    Date::enDay _dayOfWeek;
    UInt8 _month;
};

/** This dategenerator calculates the date of the first weekday after
 *  the given date.
 *  Example:
 *  @code
 *  FirstkDayAfter fkda(Date::Monday);
 *  @endcode
 *  This generator calculates the first monday after the given date.
 */
class MPFCORE_EXPORTS FirstkDayAfter
{
public:
    /** Constructs a dategenerator.
     *  @param dayOfWeek Specifies the day of the week.
     */
    FirstkDayAfter(Date::enDay dayOfWeek);
    /** Calculates the corresponding date using the given date.
     *  @param start The date for which the calculation shall be done.
     *  @return The correspondig date.
     */
    Date date(Date start) const;

protected:
    Date::enDay _dayOfWeek;
};

/** This dategenerator calculates the date of the first weekday before
 *  the given date.
 *  Example:
 *  @code
 *  FirstkDayBefore fkdb(Date::Monday);
 *  @endcode
 *  This generator calculates the first monday before the given date.
 */
class MPFCORE_EXPORTS FirstkDayBefore
{
public:
    /** Constructs a dategenerator.
     *  @param dayOfWeek Specifies the day of the week.
     */
    FirstkDayBefore(Date::enDay dayOfWeek);
    /** Calculates the corresponding date using the given date.
     *  @param start The date for which the calculation shall be done.
     *  @return The correspondig date.
     */
    Date date(Date start) const;

protected:
    Date::enDay _dayOfWeek;
};

/** This dategenerator calculates the date of the Gregorian easter sunday 
 *  for the given year.
 */
class MPFCORE_EXPORTS CalculateGregorianEaster
{
public:
    /** Constructs a dategenerator.
     *  @param year The year for which Gregorian easter sunday shall be
     *  calculated.
     */
    CalculateGregorianEaster(UInt16 year);
    /** Calculates the corresponding date.
     *  @return The correspondig date.
     */
    Date easterSunday();

protected:
    Date _easterSunday;
};

/** This dategenerator calculates an offset to the date of the Gregorian
 *  easter sunday for the given year.
 *  Example:
 *  @code
 *  NthDayOfGregorianEaster carnivalMonday(-48);
 *  @endcode
 *  This generator calculates the carnival monday which is exactly 48 days
 *  before the Gregorian easter sunday.
 */
class MPFCORE_EXPORTS NthDayOfGregorianEaster
{
public:
    /** Constructs a dategenerator.
     *  days are set to 0.
     */
    NthDayOfGregorianEaster();

    /** Constructs a dategenerator.
     *  @param days Specifies the offset from the Gregorian easter sunday.
     */
    NthDayOfGregorianEaster(int days);
    /** Calculates the corresponding date using the given year.
     *  @param year The year for which the calculation shall be done.
     *  @return The correspondig date.
     */
    Date date(UInt16 year) const;

protected:
    int _days;
};

}

#endif // MPF_DATEGENERATORS_H

