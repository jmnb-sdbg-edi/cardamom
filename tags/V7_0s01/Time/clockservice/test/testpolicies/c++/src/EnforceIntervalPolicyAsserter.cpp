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
 * @brief Implementation of class EnforceIntervalPolicyAsserter.
 * @author Marco Macellari <mmacellari@progesi.it>
 */

#include <iomanip>
#include <iostream>
#include "testpolicies/EnforceIntervalPolicyAsserter.hpp"

EnforceIntervalPolicyAsserter::
EnforceIntervalPolicyAsserter(double        time_origin,
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
                                                           Asserter::InvalidNumPoints)
    : Asserter(time_origin,
               period,
               execution_limit,
               epsilon_real,
               epsilon_virt,
               num_points,
               points)
{
    execution_counter_ = 0;
    point_kind_        = init_point;
}

bool EnforceIntervalPolicyAsserter::
     next_relevant_point() throw (Asserter::UnexpectedAsserterExecution)
{
    execution_counter_++;
    if (execution_counter_ == 1)
    {
        return next_relevant_point_starting_from_dontcare_point(0);
    }
    else
    {
        if (found_rel_point_ == false) throw UnexpectedAsserterExecution();
        return next_relevant_point_starting_from_dontcare_point(point_idx_);
    }
}

void EnforceIntervalPolicyAsserter::
     print_relevant_point_arguments(int execution)
{
    Asserter::print_relevant_point_arguments(execution);
    std::cerr << "execution: " << execution << ": period_idx_: " << this->period_idx_ << std::endl;
}

bool EnforceIntervalPolicyAsserter::
     next_relevant_point_starting_from_dontcare_point(int initial_point_idx)
{
    for (int i = initial_point_idx; i < num_points_ - 1; i++)
    {
        // The variable idx satisfies the following invariant:
        // 1) if points_[i].virt = time_origin_ + q * period_     with q integer, then idx = q
        // 2) if points_[i].virt = time_origin_ + q * period_ + r with q integer and 0 < r < period_, then idx = q.
        int idx = 0;
        if (points_[i].virt >= time_origin_)
        {
            idx = (int) std::floor((points_[i].virt - time_origin_) / period_);
        }
        if (points_[i].virt <  time_origin_)
        {
            idx = (int) -std::floor((time_origin_ - points_[i].virt) / period_);
            if (points_[i].virt < time_origin_ + idx * period_) idx--;
        }

        // Handle the case of initial point.
        if (point_kind_ == init_point)
        {
            // Try to establish if the current point is a rel point.
            if (points_[i].virt == time_origin_ + idx * period_)
            {
                point_idx_    = i;
                scale_factor_ = 0.0;
                found_rel_point_ = true;
                period_idx_ = idx;
                point_kind_ = rel_point;
                return true;
            }
            point_kind_ = non_rel_point;
        }

        // Handle the case of simple rel point and of non rel point.
        if (point_kind_ == rel_point || point_kind_ == non_rel_point)
        {
            // Try to establish if the current vector leads to a rel point.
            double v0 = 0.0, v1 = 0.0;
            int    v0_idx = 0, v1_idx = 0;
            if (point_kind_ == non_rel_point)
            {
                if (points_[i].virt == time_origin_ + idx * period_)
                {
                    v0 = time_origin_ + (idx - 1) * period_;
                    v1 = time_origin_ + (idx + 1) * period_;
                    v0_idx = idx - 1;
                    v1_idx = idx + 1;
                }
                if (points_[i].virt != time_origin_ + idx * period_)
                {
                    v0 = time_origin_ + idx * period_;
                    v1 = time_origin_ + (idx + 1) * period_;
                    v0_idx = idx;
                    v1_idx = idx + 1;
                }
            }
            if (point_kind_ == rel_point)
            {
                v0 = time_origin_ + (period_idx_ - 1) * period_;
                v1 = time_origin_ + (period_idx_ + 1) * period_;
                v0_idx = period_idx_ - 1;
                v1_idx = period_idx_ + 1;
            }
            if ((points_[i + 1].virt > points_[i].virt) &&
                (points_[i + 1].real > points_[i].real) &&
                (points_[i + 1].virt >= v1))
            {
                // Current vector has a positive finite rate.
                if (points_[i + 1].virt == v1)
                {
                    point_idx_    = i + 1;
                    scale_factor_ = 0.0;
                }
                else
                {
                    point_idx_    = i;
                    scale_factor_ = (v1 - points_[i].virt) / (points_[i + 1].virt - points_[i].virt);
                }
                found_rel_point_ = true;
                period_idx_ = v1_idx;
                point_kind_ = rel_point;
                return true;
            }
            if (v0 >= 0.0)
            {
                if ((points_[i + 1].virt < points_[i].virt) &&
                    (points_[i + 1].real > points_[i].real) &&
                    (points_[i + 1].virt <= v0))
                {
                    // Current vector has a negative finite rate.
                    if (points_[i + 1].virt == v0)
                    {
                        point_idx_    = i + 1;
                        scale_factor_ = 0.0;
                    }
                    else
                    {
                        point_idx_    = i;
                        scale_factor_ = (v0 - points_[i].virt) / (points_[i + 1].virt - points_[i].virt);
                    }
                    found_rel_point_ = true;
                    period_idx_ = v0_idx;
                    point_kind_ = rel_point;
                    return true;
                }
            }
            if (points_[i + 1].real == points_[i].real)
            {
                // Current vector has an infinite rate.
                // The variable aux_idx satisfies the following invariant:
                // 1) if points_[i].virt = time_origin_ + q * period_     with q int, then aux_idx = q
                // 2) if points_[i].virt = time_origin_ + q * period_ + r with q int and 0 < r < period_, then aux_idx = q.
                int aux_idx = 0;
                if (points_[i + 1].virt >= time_origin_)
                {
                    aux_idx = (int) std::floor((points_[i + 1].virt - time_origin_) / period_);
                }
                if (points_[i + 1].virt <  time_origin_)
                {
                    aux_idx = (int) -std::floor((time_origin_ - points_[i + 1].virt) / period_);
                    if (points_[i + 1].virt < time_origin_ + aux_idx * period_) aux_idx--;
                }
                if (points_[i + 1].virt == time_origin_ + aux_idx * period_)
                {
                    point_idx_    = i + 1;
                    scale_factor_ = 0.0;
                    found_rel_point_ = true;
                    period_idx_ = aux_idx;
                    point_kind_ = rel_point;
                    return true;
                }
            }
            point_kind_ = non_rel_point;
        }
    }
    found_rel_point_ = false;
    return false;
}

