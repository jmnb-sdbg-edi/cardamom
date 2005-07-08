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
                                                                                                 
#ifndef __CTOOLS_TIME_HIGHRESCLOCK__
#define __CTOOLS_TIME_HIGHRESCLOCK__

#include "ctools/time/HighResTime.h"

namespace ctools { namespace time {

/**
 * This class provide a way of getting access to high resolution clock
 * available on Pentium microprocessors. This clock has the same 
 * resolution of the hardware clock of the machine. The time measured by
 * this clock is that elapsed since the boot up of the machine.
 * 
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0
 */
class HighResClock {
public:
    static HighResTime getTime();
    
    static void getTime(HighResTime& time);

    static unsigned long long getClockTickCount();
    
    static double getClockFrequency ();
    
    static double getClockPeriod ();
    
private:
    HighResClock(); 
    HighResClock(HighResClock&); 
    HighResClock operator=(HighResClock&);
};

} /* ctools */ } /* time */    
#endif /* __CTOOLS_TIME_HIGHRESCLOCK__ */
