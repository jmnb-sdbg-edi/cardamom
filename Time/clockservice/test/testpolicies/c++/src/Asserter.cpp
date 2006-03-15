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
 * @brief Implementation of class Asserter.
 * @author Marco Macellari <mmacellari@progesi.it>
 */

#include <iomanip>
#include <iostream>
//#include <cppunit/extensions/HelperMacros.h>
#include "testpolicies/Asserter.hpp"

Asserter::Asserter(double        time_origin,
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
                                                InvalidPointsArray)
    : time_origin_    (time_origin),
      period_         (period),
      num_points_     (num_points),
      points_         (points),
      execution_limit_(execution_limit),
      epsilon_real_   (epsilon_real),
      epsilon_virt_   (epsilon_virt)
{
    if (time_origin     <  0.0) throw InvalidTimeOrigin();
    if (period          <= 0.0) throw InvalidPeriod();
    if (execution_limit <    0) throw InvalidExecutionLimit();
    if (epsilon_real    <= 0.0) throw InvalidEpsilonReal();
    if (epsilon_virt    <= 0.0) throw InvalidEpsilonVirt();
    if (num_points      <=   0) throw InvalidNumPoints();
    if (points[0].real  != 0.0) throw InvalidPointsArray();
    for (int i = 0; i < num_points; i++)
        if (points[i].virt < 0.0)
            throw InvalidPointsArray();
    for (int i = 0; i < num_points - 1; i++)
        if (points[i + 1].real < points[i].real)
            throw InvalidPointsArray();

    execution_         = 0;
    failed_executions_ = 0;
}

unsigned long Asserter::get_num_remaining_executions()
{
    unsigned long remaining_executions = 0;
    try
    {
        for (unsigned long execution_counter = execution_;
             (execution_limit_ == 0 || execution_counter < execution_limit_) && this->next_relevant_point();
             remaining_executions++, execution_counter++);
    }
    catch (const UnexpectedAsserterExecution& e)
    {
    }
    return remaining_executions;
}

unsigned long Asserter::get_num_failed_executions()
{
    return failed_executions_;
}

bool Asserter::more_executions()
{
    if ((execution_limit_ > 0) && (execution_ == execution_limit_)) return false;
    if ((execution_ > 0) && (found_rel_point_ == false))            return false;
    return true;
}
                                                                                                                            
void Asserter::print_relevant_points(bool with_args)
{
    try
    {
        std::cerr << "List of relevant points:" << std::endl;
        for (unsigned long execution_counter = 0;
             (execution_limit_ == 0 || execution_counter < execution_limit_) && this->next_relevant_point();
             execution_counter++)
        {
            double real_rel = points_[point_idx_].real + scale_factor_ * (points_[point_idx_ + 1].real - points_[point_idx_].real);
            double virt_rel = points_[point_idx_].virt + scale_factor_ * (points_[point_idx_ + 1].virt - points_[point_idx_].virt);
            std::cerr << "execution: " << execution_counter << ": <real, virt> = <"
                      << std::setprecision(6) << std::fixed << real_rel << ", "
                      << std::setprecision(6) << std::fixed << virt_rel << ">"
                      << std::endl;
            if (with_args) this->print_relevant_point_arguments(execution_counter);
        }
    }
    catch (const UnexpectedAsserterExecution& e)
    {
        std::cerr << "Unexpected asserter execution exception" << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unexpected exception" << std::endl;
    }
}

bool Asserter::operator()(TimeBase::TimeT virtual_time)
{
    try
    {
        // Increment the counter of do_work invocations
        ++execution_;

        // Get current coordinates
        TimeBase::TimeT real_curr = Cdmw::clock::compute_current_time();
        TimeBase::TimeT virt_curr = virtual_time;

        // Increment execution counter and initialize real_start_ if needed
        if (execution_ == 1)
        {
            real_start_ = real_curr;
        }

        // Check for the not violation of the execution limit
        if (execution_limit_ > 0 && execution_ > execution_limit_)
        {
            // The execution limit constraint cannot be violated
            std::cerr << "error: asserter execution: " << execution_ << ": the current do_work invocation exceedes the execution limit" << std::endl;
            throw UnexpectedAsserterExecution();
        }

        // Try to detect next relevant point
        if (!this->next_relevant_point())
        {
            // Every do_work invocation should detect one relevant point.
            std::cerr << "error: asserter execution: " << execution_ << ": the current do_work invocation refers to no relevant points." << std::endl;
            failed_executions_++;
            return false;
        }
        else
        {
            double real_sec = Cdmw::clock::util::TimeT2seconds(real_curr - real_start_);
            double virt_sec = Cdmw::clock::util::TimeT2seconds(virt_curr);
            double real_rel = points_[point_idx_].real + scale_factor_ * (points_[point_idx_ + 1].real - points_[point_idx_].real);
            double virt_rel = points_[point_idx_].virt + scale_factor_ * (points_[point_idx_ + 1].virt - points_[point_idx_].virt);

            //std::cerr << '.';
            //std::cerr << std::endl;
            //std::cerr << "asserter execution: " << execution_ << ": real sampled  (seconds) .. : " << real_sec << std::endl;
            //std::cerr << "asserter execution: " << execution_ << ": virt sampled  (seconds) .. : " << virt_sec << std::endl;
            //std::cerr << "asserter execution: " << execution_ << ": real expected (seconds) .. : " << real_rel << std::endl;
            //std::cerr << "asserter execution: " << execution_ << ": virt expected (seconds) .. : " << virt_rel << std::endl;
            //std::cerr << "asserter execution: " << execution_ << ": real sampled (TimeT) ..... : " << real_curr - real_start_ << std::endl;
            //std::cerr << "asserter execution: " << execution_ << ": virt sampled (TimeT) ..... : " << virt_curr               << std::endl;

            bool real_out_of_tolerance_range = (std::fabs(real_sec - real_rel) > epsilon_real_);
            bool virt_out_of_tolerance_range = (std::fabs(virt_sec - virt_rel) > epsilon_virt_);
            if (real_out_of_tolerance_range || virt_out_of_tolerance_range)
            {
                if (real_out_of_tolerance_range)
                {
                    std::cerr << "error: asserter execution: " << execution_ << ": real coordinate not within tolerance range" << std::endl;
                }
                if (virt_out_of_tolerance_range)
                {
                    std::cerr << "error: asserter execution: " << execution_ << ": virtual coordinate not within tolerance range" << std::endl;
                }
                failed_executions_++;
                return false;
            }
        }
    }
    catch (const CosClockService::TimeUnavailable& e)
    {
        // Could not get current time
        std::cerr << "error: asserter execution: " << execution_ << ": could not get current time" << std::endl;
        failed_executions_++;
        return false;
    }
    catch (const UnexpectedAsserterExecution& e)
    {
        // Another asserter execution is unexpected.
        std::cerr << "error: asserter execution: " << execution_ << ": the current do_work execution is unexpected" << std::endl;
        failed_executions_++;
        return false;
    }
    catch (...)
    {
        // An exception is unexpected.
        std::cerr << "error: asserter execution: " << execution_ << ": unexpected exception during the execution of do_work" << std::endl;
        failed_executions_++;
        return false;
    }
    return true;
}

void Asserter::print_relevant_point_arguments(int execution)
{
    std::cerr << "execution: " << execution << ": point_idx_: "       << this->point_idx_       << std::endl;
    std::cerr << "execution: " << execution << ": scale_factor_: "    << this->scale_factor_    << std::endl;
    std::cerr << "execution: " << execution << ": found_rel_point_: " << this->found_rel_point_ << std::endl;
}

