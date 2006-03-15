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

// ************************************************************************
//    HighResTimer.h
// ************************************************************************
//
//                               cTools
//
//               Copyright (C) 2001-2003 by Angelo Corsaro.
//                         <corsaro@cs.wustl.edu>
//                          All Rights Reserved.
//
//   Permission to use, copy, modify, and distribute this software and
//   its  documentation for any purpose is hereby  granted without fee,
//   provided that the above copyright notice appear in all copies and
//   that both that copyright notice and this permission notice appear
//   in  supporting  documentation. I don't make  any  representations
//   about the  suitability  of this  software for any  purpose. It is
//   provided "as is" without express or implied warranty.
//
//
// *************************************************************************
//  
// *************************************************************************

#ifndef _CTOOLS_TIME_HIGHT_RES_CLOCK_H_
#define  _CTOOLS_TIME_HIGHT_RES_CLOCK_H_

#include "ctools/time/HighResClock.h"

namespace ctools { namespace time {

/**
 * This class provide a way of performing high resolution of the time
 * spent performing something.
 *
 * @author <a href="mailto:corsaro@doc.cs.wustl.edu">Angelo Corsaro</a>
 * @version 1.0
 */
class HighResTimer {
public:

    HighResTimer() { }
    
    /**
     * Starts measuring the elapsed time.
     *
     */
    inline void start() {
        startClockTick_ = ctools::time::HighResClock::getClockTickCount();
    }
    
    /**
     * Stops the time measurement.
     *
     */
    inline void stop() {
        stopClockTick_ = ctools::time::HighResClock::getClockTickCount();
    }

    /**
     * Resets the timer.
     *
     */
    inline void reset() {
        startClockTick_ = stopClockTick_ = 0;
    }

    inline ctools::time::HighResTime getElapsedTime() {
        return ctools::time::HighResTime(stopClockTick_ - startClockTick_);
    }

    inline void getElapsedTime(ctools::time::HighResTime& time) {
        time.set(stopClockTick_ - startClockTick_);
    }

private: 
    unsigned long long  startClockTick_;
    unsigned long long  stopClockTick_; 

};

} /* ctools */ } /* time */

#endif /* _CTOOLS_TIME_HIGHT_RES_CLOCK_H_ */
