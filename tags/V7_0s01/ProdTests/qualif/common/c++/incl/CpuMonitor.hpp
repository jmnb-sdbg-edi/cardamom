/* =========================================================================== *
 * Copyright (c) 2003-2005 THALES All rights reserved.
 * Software commonly developed by THALES and AMS.
 *
 * This file is part of CARDAMOM.
 *
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with CARDAMOM; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * =========================================================================== */

#ifndef INCL_CDMW_CPU_MONITOR_HPP
#define INCL_CDMW_CPU_MONITOR_HPP

#include <iostream>
#include <string>
#include <signal.h>


#include <fcntl.h>
#include <unistd.h>

#include <Foundation/osthreads/Thread.hpp>

namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;  

    const float DEFAULT_WINDOW_SIZE=5;    // 5 sec measurement window
    const float DEFAULT_WINDOW_DELAY=1;   // 1 sec delay between measurement windows
    const float DEFAULT_PCPU_LIMIT=100;   // 100% CPU is alert limit (i.e. no alert)
};

namespace Cdmw
{

    // measure of jiffies spent at a fixed time:
    //    - by system (since boot)
    //    - by a process (since its start-up)
    struct jiffies 
    {
        long long cpu_jiffies;
        long long proc_jiffies;
    };
    
    
    // measures of jiffies of the system and of a process
    // at the begining and at the end of a measurement window
    struct window_measures 
    {
        jiffies jiffies_at_start;
        jiffies jiffies_at_end;
    };



    class CpuMonitor :  public Cdmw::OsSupport::Thread
    
    {
    public:
        /**
         * constructor
         */
        CpuMonitor(unsigned int pid,
                   unsigned int window_size = DEFAULT_WINDOW_SIZE,
                   unsigned int window_delay = DEFAULT_WINDOW_DELAY,
                   unsigned int cpu_limit = DEFAULT_PCPU_LIMIT);

        /**
         * destuctor
         */
        virtual ~CpuMonitor()
            throw();
        

        /**
         * print the cpu consumption on the screen
         */
        void print_percent_cpu(const float& pcpu);
        

        /**
         * open_stat_files() 
         */
        void open_stat_files();
        

        /**
         * close_stat_files()
         */
        void close_stat_files();
        

        /**
         * get_jiffies()
         */
        jiffies get_jiffies();
        

        /**
         * sleep(const float& seconds)
         */
        void sleep(const float& seconds);


        /**
         * start_measurement()
         */
        void start_measurement();
        
                
        /**
         * stop_measurement()
         */  
        void stop_measurement();
        
        /**
         * limit_is_exceeded()
         */
        bool limit_is_exceeded()
        {
            return m_limit_exceed;
        }
        

    protected:

        void run() throw();
        

            
    private:
        unsigned int m_pid;
        unsigned int m_window_size;
        unsigned int m_window_delay;
        unsigned int m_cpu_limit;
        int m_cpu_stat_file;
        int m_proc_stat_file;
        bool m_stop_measure;
        bool m_limit_exceed;
        
    };
    
};

#endif //INCL_CDMW_CPU_MONITOR_HPP
