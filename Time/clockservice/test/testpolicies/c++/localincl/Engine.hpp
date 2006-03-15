/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */

/**
 * @brief Definition of class Engine.
 * @author Marco Macellari <mmacellari@progesi.it>
 */

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "CdmwTime.stub.hpp"
#include "CosClockService.stub.hpp"

#ifndef TYPE_POINT_T
#define TYPE_POINT_T
typedef struct _point { double real; double virt; } Point_t;
#endif // TYPE_POINT_T

class Engine
{
    public:

        /**
         * @brief Base class for execption from extended asserters.
         */
        class Exception {};

        /**
         * @brief The controlled executor reference must be valid.
         * This exception class is thrown when the controlled executor reference is not valid.
         */
        class InvalidExecutor : public Exception {};

        /**
         * @brief The real period of the controlled clock must be positive.
         * This exception class is thrown when the real period of the controlled clock is non-positive.
         */
        class InvalidPeriod : public Exception {};

        /**
         * @brief The execution limit of the controller must be non-negative.
         * This exception class is thrown when the execution limit of the controller is negative.
         */
        class InvalidExecutionLimit : public Exception {};

        /**
         * @brief The number of points must be positive.
         * This exception class is thrown when the number of points is non-positive.
         */
        class InvalidNumPoints : public Exception {};

        /**
         * @brief The following rules on the points in a point array must hold:
         * 1. points[i].virt >= 0                  (for i in [0, num_points])
         * 1. points[0].real == 0                  (test start real time is zero)
         * 2. points[i + 1].real >= points[i].real (for i in [0, num_points - 1])
         * This exception class is thrown when these rules are not satisfied.
         */
        class InvalidPointsArray : public Exception {};

        /**
         * @brief Constructor of class for extended engines.
         *        The array of points must be previously allocated.
         * @param executor        is the controlled executor to be tested.
         * @param controller      is the controller used to test the executor.
         * @param period          is the real period of the controller.
         * @param execution_limit is the execution limit on the number of do_work invocations.
         * @param num_points      is the number of points in the points array.
         * @param points          is the array of points.
         * @throw InvalidExecutor       if the executor reference is not valid.
         * @throw InvalidPeriod         if the real period of the controller is non-positive.
         * @throw InvalidExecutionLimit if the execution limit of the controller is negative.
         * @throw InvalidNumPoints      if the number of points is non-positive.
         * @throw InvalidPointsArray    if the points array is not valid.
         */
        Engine(Cdmw::clock::ControlledExecutor_ptr executor,
               double                              period,
               unsigned long                       execution_limit,
               int                                 num_points,
               Point_t*                            points) throw (InvalidExecutor,
                                                                  InvalidPeriod,
                                                                  InvalidExecutionLimit,
                                                                  InvalidNumPoints,
                                                                  InvalidPointsArray);

        /**
         * @brief Destructor of class for extended engines.
         */
        ~Engine()
        {
            CORBA::release(executor_);
        }

        /**
         * @brief Print all relevant actions detected according to the given policy.
         */
        void print_relevant_actions();

        /**
         * @brief Runs the engine to simulate a test.
         */
        void run();

    private:

        Cdmw::clock::ControlledExecutor_ptr executor_; // The controlled executor.

        const double        period_;          // The real period of the controller.
        const unsigned long execution_limit_; // The execution limit on the number of do_work invocations.
        const int           num_points_;      // The number of points in the points array.
        const Point_t*      points_;          // The points array.
};

#endif // ENGINE_HPP
