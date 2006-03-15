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
 * @brief Definition of class EnforceDeadlinePolicyAsserter.
 * @author Marco Macellari <mmacellari@progesi.it>
 */

#ifndef ENFORCE_DEADLINE_POLICY_ASSERTER_HPP
#define ENFORCE_DEADLINE_POLICY_ASSERTER_HPP

#include "Asserter.hpp"

class EnforceDeadlinePolicyAsserter : public Asserter
{
    public:

        /**
         * @brief Constructor of derived class for deadline policy extended asserters.
         *        The array of points must be previously allocated.
         * @param time_origin     is the time origin of the controller.
         * @param period          is the real period of the controller.
         * @param execution_limit is the execution limit on the number of do_work invocations.
         * @param epsilon_real    is the tolerance on real coordinates.
         * @param epsilon_virt    is the tolerance on virtual coordinates.
         * @param num_points      is the number of points in the points array.
         * @param points          is the array of points.
         * @throw Asserter::InvalidTimeOrigin     if the time origin of the controller is negative.
         * @throw Asserter::InvalidPeriod         if the real period of the controller is non-positive.
         * @throw Asserter::InvalidExecutionLimit if the execution limit of the controller is negative.
         * @throw Asserter::InvalidEpsilonReal    if the tolerance on real coordinates is non-positive.
         * @throw Asserter::InvalidEpsilonVirt    if the tolerance on virtual coordinates is non-positive.
         * @throw Asserter::InvalidNumPoints      if the number of points is non-positive.
         */
        EnforceDeadlinePolicyAsserter(double        time_origin,
                                      double        period,
                                      unsigned long execution_limit,
                                      double        epsilon_real,
                                      double        epsilon_virt,
                                      int           num_points,
                                      Point_t*      points) throw (Asserter::InvalidTimeOrigin,
                                                                   Asserter::InvalidPeriod,
                                                                   Asserter::InvalidExecutionLimit,
                                                                   Asserter::InvalidEpsilonReal,
                                                                   Asserter::InvalidEpsilonVirt,
                                                                   Asserter::InvalidNumPoints);

    protected:

        /**
         * @brief Modifies point_idx_ and scale_factor_ to represent the next relevant point.
         * @return true if and only if a new relevant point was detected.
         * @throw UnexpectedAsserterExecution if no invocation of do_work is expected.
         */
        bool next_relevant_point() throw (Asserter::UnexpectedAsserterExecution);

    private:

        /**
         * @brief point_kind_t is the type defining the kind of every rel and non rel point.
         */
        typedef enum { init_point, simple_rel_point, multiple_rel_point, non_rel_point } point_kind_t;

        /**
         * @brief Prints all relevant point arguments; used by print_relevant_points.
         * @param execution is the execution number used to order output.
         */
        void print_relevant_point_arguments(int execution);

        /**
         * @brief Modifies point_idx_ and scale_factor_ to represent the next relevant point.
         *        No assumption is made wether the initial point is a relevant point or not,
         *        (so, if it is a relevant point, it will be stored as such).
         * @param initial_point_idx the index of the initial point in the search.
         * @return Returns true if and only if a new relevant point was detected.
         */
        bool next_relevant_point_starting_from_dontcare_point(int initial_point_idx);

        unsigned long execution_counter_; // Counts the execution of nex_relevant_point().
        point_kind_t  point_kind_;        // The kind of every rel and non rel point; used in next_rel_point().
        int           deadline_idx_;      // The current value of the deadline triggering do_work().
};

#endif // ENFORCE_DEADLINE_POLICY_ASSERTER_HPP
