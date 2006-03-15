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
 * @brief Definition of class Asserter.
 * @author Marco Macellari <mmacellari@progesi.it>
 */

#ifndef ASSERTER_HPP
#define ASSERTER_HPP

#include "Time/clockservice/test/testcommon/anExecution.hpp"

#ifndef TYPE_POINT_T
#define TYPE_POINT_T
typedef struct _point { double real; double virt; } Point_t;
#endif // TYPE_POINT_T

class Asserter : public anExecutionCallback
{
    public:

        /**
         * @brief Base class for execption from extended asserters.
         */
        class Exception {};

        /**
         * @brief The time origin of the controlled clock must be non-negative.
         * This exception class is thrown when the time origin of the controlled clock is negative.
         */
        class InvalidTimeOrigin : public Exception {};

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
         * @brief The tolerance on real coordinates must be positive.
         * This exception class is thrown when the tolerance on real coordinates is non-positive.
         */
        class InvalidEpsilonReal : public Exception {};

        /**
         * @brief The tolerance on virtual coordinates must be positive.
         * This exception class is thrown when the tolerance on virtual coordinates is non-positive.
         */
        class InvalidEpsilonVirt : public Exception {};

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
         * @brief The set of relevant points is well defined according to the policy used.
         * This exception class is thrown when an unexpected execution of the asserter takes place.
         */
        class UnexpectedAsserterExecution : public Exception {};

        /**
         * @brief Constructor of base class for extended asserters.
         *        The array of points must be previously allocated.
         * @param time_origin     is the time origin of the controller.
         * @param period          is the real period of the controller.
         * @param execution_limit is the execution limit on the number of do_work invocations.
         * @param epsilon_real    is the tolerance on real coordinates.
         * @param epsilon_virt    is the tolerance on virtual coordinates.
         * @param num_points      is the number of points in the points array.
         * @param points          is the array of points.
         * @throw InvalidTimeOrigin     if the time origin of the controller is negative.
         * @throw InvalidPeriod         if the real period of the controller is non-positive.
         * @throw InvalidExecutionLimit if the execution limit of the controller is negative.
         * @throw InvalidEpsilonReal    if the tolerance on real coordinates is non-positive.
         * @throw InvalidEpsilonVirt    if the tolerance on virtual coordinates is non-positive.
         * @throw InvalidNumPoints      if the number of points is non-positive.
         * @throw InvalidPointsArray    if the points array is not valid.
         */
        Asserter(double        time_origin,
                 double        period,
                 unsigned long execution_limit,
                 double        epsilon_real,
                 double        epsilon_virt,
                 int           num_points,
                 Point_t*      points) throw (InvalidTimeOrigin,
                                              InvalidPeriod,
                                              InvalidExecutionLimit,
                                              InvalidEpsilonReal,
                                              InvalidEpsilonVirt,
                                              InvalidNumPoints,
                                              InvalidPointsArray);

        /**
         * @brief Virtual destructor of base class.
         */
        virtual ~Asserter() throw () {}

        /**
         * @brief Get the number of do_work invocations done till now.
         * @return the number of do_work invocations done till now.
         */
        unsigned long get_num_executions() const { return execution_; }

        /**
         * @brief Get the number of expected do_work invocations still to be done.
         *        This method has side effects on point_idx_ and scale_factor_, so it
         *        should be called only after the test has ended for a final check.
         * @return the number of expected do_work invocations still to be done.
         */
        unsigned long get_num_remaining_executions();

        /**
         * @brief Get the current number of executions of operator() in which the check failed.
         * @return the current number of executions of operator() in which the check failed.
         */
        unsigned long get_num_failed_executions();

        /**
         * @brief Returns false if no more do_work invocations are expected.
         * @return false => no more do_work invocations are to be done
         *         true  => cannot tell wether or not more do_work invocations are to be done.
         */
        bool more_executions();

        /**
         * @brief Print all relevant points detected according to the given policy.
         *        This method has side effects on point_idx_ and scale_factor_,
         *        so it should not be called in conjunction with operator().
         * @param with_args true if and only if also relevant point arguments are to be printed.
         */
        void print_relevant_points(bool with_args);

        /**
         * @brief Does the real check for the correct application of the policy.
         * @param virtual_time the virtual time registered when Periodic::do_work was lastly called.
         * @return true if and only if the check completed successfully.
         */
        bool operator()(TimeBase::TimeT virtual_time);

    protected:

        /**
         * @brief Modifies point_idx_ and scale_factor_ to represent the next relevant point.
         * @return true if and only if a new relevant point was detected.
         * @throw UnexpectedAsserterExecution if no invocation of do_work is expected.
         */
        virtual bool next_relevant_point() throw (UnexpectedAsserterExecution) = 0;

        /**
         * @brief Prints all relevant point arguments; used by print_relevant_points.
         * @param execution is the execution number used to order output.
         */
        virtual void print_relevant_point_arguments(int execution);

        int    point_idx_;       // The index of the point in the points array which is the origin of the rel vector.
        double scale_factor_;    // The scale factor indicating an internal point in the rel vector: values in [0.0 1.0).
        bool   found_rel_point_; // True if and only if a rel point was found by last exec of next_relevant_point.

        const double   time_origin_; // The time origin of the controller.
        const double   period_;      // The real period of the controller.
        const int      num_points_;  // The number of points in the points array.
        const Point_t* points_;      // The points array.

    private:

        unsigned long   execution_;         // The number of times function do_work was called till now.
        unsigned long   failed_executions_; // The number of times operator() failed its check.
        TimeBase::TimeT real_start_;        // The real time of controller start-up.

        const unsigned long execution_limit_; // The execution limit on the number of do_work invocations.
        const double        epsilon_real_;    // The tolerance on real coordinates.
        const double        epsilon_virt_;    // The tolerance on virtual coordinates.
};

#endif // ASSERTER_HPP
