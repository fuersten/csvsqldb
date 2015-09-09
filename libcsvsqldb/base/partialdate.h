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

#ifndef MPF_PARTIALDATE_H
#define MPF_PARTIALDATE_H

#include <mpfcore/inc.h>

#include <mpfcore/date.h>


namespace mpf
{

/** This class calculates the date of the specified day and month for a given
 *  year.
 *  Example:
 *  @code
 *  PartialDate germanUnificationDay(3,10);
 *  germanUnificationDay.date(2011);
 *  @endcode
 *  This generator calculates the german unification day, which is always on
 *  the third October.
 */

class MPFCORE_EXPORTS PartialDate
{
public:
    /** Constructs a PartialDate.
     *  day and month are set to first and january.
     */
    PartialDate();

    /** Constructs a PartialDate.
     *  If an invalid day or month is specified a DateException is thrown.
     *  @param day Specifies the day.
     *  @param month Specifies the month.
     */
    PartialDate(UInt8 day, UInt8 month);
    /** Calculates the corresponding date using the given year.
     *  @param year The year for which the calculation shall be done.
     *  @return The correspondig date.
     */
    Date date(UInt16 year) const;

protected:
    UInt8 _day;
    UInt8 _month;
};

}

#endif // MPF_PARTIALDATE_H
