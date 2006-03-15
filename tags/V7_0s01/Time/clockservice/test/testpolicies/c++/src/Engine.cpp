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
 * @brief Implementation of class Engine.
 * @author Marco Macellari <mmacellari@progesi.it>
 */

#include <ctime>
#include <iomanip>
#include <iostream>
#include "testpolicies/Engine.hpp"
#include "Time/clockservice/Util.hpp"

const double sec2nsec = 1000000000.0;

Engine::Engine(Cdmw::clock::ControlledExecutor_ptr executor,
               double                              period,
               unsigned long                       execution_limit,
               int                                 num_points,
               Point_t*                            points) throw (InvalidExecutor,
                                                                  InvalidPeriod,
                                                                  InvalidExecutionLimit,
                                                                  InvalidNumPoints,
                                                                  InvalidPointsArray)
    : period_         (period),
      execution_limit_(execution_limit),
      num_points_     (num_points),
      points_         (points)
                                                                                                                                                       
{
    if (CORBA::is_nil(executor  )) throw InvalidExecutor();
    if (period          <= 0.0) throw InvalidPeriod();
    if (execution_limit <    0) throw InvalidExecutionLimit();
    if (num_points      <=   0) throw InvalidNumPoints();
    if (points[0].real  != 0.0) throw InvalidPointsArray();
    for (int i = 0; i < num_points; i++)
        if (points[i].virt < 0.0)
            throw InvalidPointsArray();
    for (int i = 0; i < num_points - 1; i++)
        if (points[i + 1].real < points[i].real)
            throw InvalidPointsArray();

    executor_ = Cdmw::clock::ControlledExecutor::_duplicate(executor);
}

void Engine::print_relevant_actions()
{
    try
    {
        std::cerr << "List of relevant actions:" << std::endl;

        // Drive the executor.
        bool in_pause = false;
        for (int i = 0; i < num_points_ - 1; i++)
        {
            if (points_[i].real == points_[i + 1].real)
            {
                if (in_pause == true) std::cerr << "executor_->resume()" << std::endl;
                std::cerr << "executor_->set("
                          << Cdmw::clock::util::seconds2TimeT(points_[i + 1].virt)
                          << " [" << points_[i + 1].virt << " sec]"
                          << ')' << std::endl;
                in_pause = false;
            }
            else if (points_[i].virt == points_[i + 1].virt)
            {
                if (in_pause == false) std::cerr << "executor_->pause()" << std::endl;
                unsigned long secs  = (unsigned long) std::floor(points_[i + 1].real - points_[i].real);
                unsigned long nsecs = (unsigned long) std::floor((points_[i + 1].real - points_[i].real - secs) * 1000000000.0);
                std::cerr << "Cdmw::OsSupport::OS::sleep("
                          << 1000000000 * (unsigned long long) secs + (unsigned long long) nsecs
                          << " [" << points_[i + 1].real - points_[i].real << " secs]"
                          << ')' << std::endl;
                in_pause = true;
            }
            else
            {
                if (in_pause == true) std::cerr << "executor_->resume()" << std::endl;
                std::cerr << "executor_->set_rate("
                          << (points_[i + 1].virt - points_[i].virt) / (points_[i + 1].real - points_[i].real)
                          << ')' << std::endl;
                unsigned long secs  = (unsigned long) std::floor(points_[i + 1].real - points_[i].real);
                unsigned long nsecs = (unsigned long) std::floor((points_[i + 1].real - points_[i].real - secs) * 1000000000.0);
                std::cerr << "Cdmw::OsSupport::OS::sleep("
                          << 1000000000 * (unsigned long long) secs + (unsigned long long) nsecs
                          << " [" << points_[i + 1].real - points_[i].real << " secs]"
                          << ')' << std::endl;
                in_pause = false;
            }
        }
    }
    catch (...)
    {
        // An exception is unexpected.
        std::cerr << "Unexpected exception: engine halted." << std::endl;
    }
}

void Engine::run()
{
    try
    {
        // Drive the executor
        bool in_pause = false;
        for (int i = 0; i < num_points_ - 1; i++)
        {
            double dx = points_[i + 1].real - points_[i].real;
            double dy = points_[i + 1].virt - points_[i].virt;

            if (dx == 0)
            {
                if (in_pause == true) executor_->resume();
                executor_->set(Cdmw::clock::util::seconds2TimeT(points_[i + 1].virt));
                in_pause = false;
            }
            else if (dy ==0 )
            {
                if (in_pause == false) executor_->pause();
                unsigned long secs  = (unsigned long) std::floor(dx);
                unsigned long nsecs = (unsigned long) std::floor((dx - secs) * sec2nsec);
                Cdmw::OsSupport::OS::sleep((unsigned int) secs, (unsigned int) nsecs);
                in_pause = true;
            }
            else
            {
                if (in_pause == true) executor_->resume();
                double tmp = dy / dx;
                executor_->set_rate(tmp);
                unsigned long secs  = (unsigned long) std::floor(dx);
                unsigned long nsecs = (unsigned long) std::floor((dx - secs) * sec2nsec);
                Cdmw::OsSupport::OS::sleep((unsigned int) secs, (unsigned int) nsecs);
                in_pause = false;
            }
        }
    }
    catch (...)
    {
        // An exception is unexpected.
        std::cerr << "Unexpected exception: engine halted." << std::endl;
    }
}

