/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 *
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Library General
 * Public License along with CARDAMOM; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ========================================================================= */
                                                                                                 
#ifndef CTOOLS_LOCALCLOCK_H
#define CTOOLS_LOCALCLOCK_H
/**
 *@file LocalClock.h
 *@brief some useful function for calculate the current time in gregorian format.
 *
 *@author Lello Mele <lellomele@yahoo.com>
 */

// needed for definition of TimeT
namespace ctools { namespace localclock {
    
typedef unsigned long long TimeT;

// return the current gregorian time.
TimeT compute_current_time();
    
} /* localclock */ } /* ctools */

#endif // CTOOLS_LOCALCLOCK_H

