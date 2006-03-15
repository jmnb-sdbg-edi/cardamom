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

#include <sstream>
#include <ProdTests/qualif/common/CpuMonitor.hpp>




namespace Cdmw
{
    
    CpuMonitor::CpuMonitor(unsigned int pid,
                           unsigned int window_size,
                           unsigned int window_delay,
                           unsigned int cpu_limit)
        : m_pid(pid),
          m_window_size(window_size),
          m_window_delay(window_delay),
          m_cpu_limit(cpu_limit),
          m_cpu_stat_file(0),
          m_proc_stat_file(0),
          m_stop_measure(false),
          m_limit_exceed(false)
    {
  
    }
    
        
    
    /**
     * destuctor
     */
    CpuMonitor::~CpuMonitor()
        throw()
    {
        close_stat_files();
    }
    
        

    /**
     * print the cpu consumption on the screen
     */
    void CpuMonitor::print_percent_cpu(const float& pcpu)
    {
        // round down, 2 decimal precision
        float round_pcpu = ( trunc(pcpu*100) ) / 100;
        
        printf("  %4.2f %%", round_pcpu);
        std::cout << std::flush;
    }

    /**
     * open_stat_files() 
     */
    void CpuMonitor::open_stat_files()
    {
        m_cpu_stat_file = open("/proc/stat", O_RDONLY, 0);
        if (m_cpu_stat_file == 0) {
            std::cerr << "fopen failed on /proc/stat" << std::endl;
            ::exit(FAILURE);
        }
        
        std::ostringstream proc_stat_filename;
        proc_stat_filename<<"/proc/"<< m_pid << "/stat";
        m_proc_stat_file = open(proc_stat_filename.str().c_str(), O_RDONLY, 0);
        if (m_proc_stat_file == 0) {
            std::cerr << "fopen failed on " << proc_stat_filename << std::endl;
            std::cerr << "Is really " << m_pid << " an existing pid ?" << std::endl;
            exit(FAILURE);
        }
    };
    
    
    /**
     * close_stat_files()
     */
    void CpuMonitor::close_stat_files()
    {
        if (m_cpu_stat_file != 0)
            close(m_cpu_stat_file);
        if (m_proc_stat_file != 0)
            close(m_proc_stat_file);
        m_cpu_stat_file = 0;
        m_proc_stat_file = 0;
    }
    
    
    /**
     * get_jiffies()
     */
    jiffies CpuMonitor::get_jiffies()
    {
        open_stat_files();
        
        char cpu_stat_buf[128];
        char proc_stat_buf[2048];
        
        // read system stat file
        // if (fgets(cpu_stat_buf, sizeof cpu_stat_buf, cpu_stat_file) == NULL) {
        if (read(m_cpu_stat_file, cpu_stat_buf, sizeof cpu_stat_buf) <= 0) {
            std::cerr << "read failed on /proc/stat" << std::endl;
            exit(FAILURE);
        }
    
        // read process stat file
        // if (fgets(proc_stat_buf, sizeof proc_stat_buf, proc_stat_file) == NULL) {
        if (read(m_proc_stat_file, proc_stat_buf, sizeof proc_stat_buf) <= 0) {
            std::cerr << "fgets failed on /proc/" << m_pid << "/stat" << std::endl;
            exit(FAILURE);
        }
        
        close_stat_files();
        
        jiffies to_return;
        
        // Parse first line of /proc/stat
        // content is: "cpu %llu %llu %llu %llu ..." with following variables:
        long long jiffies_in_user_mode;
        long long jiffies_in_nice_mode;
        long long jiffies_in_system_mode;
        long long jiffies_in_other_mode;
        if (sscanf(cpu_stat_buf, "cpu %llu %llu %llu %llu", 
                   &jiffies_in_user_mode, 
                   &jiffies_in_nice_mode, 
                   &jiffies_in_system_mode, 
                   &jiffies_in_other_mode)
            < 4)
        {
            std::cerr << "Error while parsing 1st line of /proc/stat" << std::endl;
            exit(FAILURE);
        }
        // total jiffies for cpu is sum of those jiffies
        to_return.cpu_jiffies = 
        jiffies_in_user_mode+jiffies_in_nice_mode+jiffies_in_system_mode+jiffies_in_other_mode;
        
        // Parse /proc/pid/stat
        // content starts with "pid (command) state ..." 
        // utime and stime are 14th and 15th fields.
        // If using nptl, threads utime & ctime are in 
        // newly added field at the end of stat file.
        // WARNING: threads utime & ctime already include
        // standard utime & ctime !!
        long long proc_jiffies_in_user_mode;
        long long proc_jiffies_in_system_mode;
        long long threads_jiffies_in_user_mode = 0;
        long long threads_jiffies_in_sytem_mode = 0;
        int nb_read_field = 
        sscanf(proc_stat_buf, 
               "%*d %*s %*c "             /* pid (cmd) state */
               "%*d %*d %*d %*d %*d "     /* ppid pgrp session tty tpgid */
               "%*u %*u %*u %*u %*u "     /* flags minflt cminflt majflt cmajflt */
               "%llu %llu %*u %*u "       /* utime stime cutime cstime */
               "%*d %*d %*d %*d "         /* priority nice 0 itrealvalue */
               "%*u %*u %*d %*u "         /* starttime vsize rss rlimit */
               "%*u %*u %*u %*u %*u "     /* startcode endcode startstack kstkesp kstkeip */
               "%*u %*u %*u %*u %*u "     /* signal blocked sigignore sigcatch wchan */
               "%*u %*u %*d %*d "         /* nswap cnswap exit_signal processor */
               "%*u %*u "                 /* rtprio policy (since 2.5.18) */
               "%llu %llu %*u %*u",       /* threadgroup - utime stime cutime cstime */
               &proc_jiffies_in_user_mode, &proc_jiffies_in_system_mode,
               &threads_jiffies_in_user_mode, &threads_jiffies_in_sytem_mode);
        
        if (nb_read_field == 2) {
            // no threadgroup fields in stat file
            to_return.proc_jiffies =
            proc_jiffies_in_user_mode + proc_jiffies_in_system_mode;
        } else if (nb_read_field == 4) {
            // use utime and stime from threadgroup fields
            to_return.proc_jiffies =
            threads_jiffies_in_user_mode + threads_jiffies_in_sytem_mode;
        } else {         
            std::cerr << "Error while parsing 1st line of /proc/" << m_pid << "/stat" << std::endl;
            exit(FAILURE);
        }
        
        return to_return;
    };
    
    
    /**
     * sleep(const float& seconds)
     */
    void CpuMonitor::sleep(const float& seconds)
    {
        struct timespec tv;
        tv.tv_sec = (long int) seconds;
        tv.tv_nsec = (long int) ((seconds - tv.tv_sec) * 1000000000);
        
        int status_nanosleep;
        struct timespec tv_not_elapsed;
        while ( (status_nanosleep = nanosleep(&tv, &tv_not_elapsed)) < 0 && (errno == EINTR) )
            tv = tv_not_elapsed;
    };
    
    
    /**
     * start_measurement()
     */
    void CpuMonitor::start_measurement()
    {
        // Compute number of measurement windows to use.
        float nb_windows_float = m_window_size / m_window_delay;
        if (trunc(nb_windows_float) != nb_windows_float) {
            std::cerr << "Error: window size must be a multiple of windows delay." << std::endl;
            exit(FAILURE);
        }
        
        // create measurement windows
        int nb_windows = (int) nb_windows_float;
        window_measures* windows = new window_measures[nb_windows];
        
        std::cout << "A full measurement cycle contains " << nb_windows 
                  << " measurement windows of " << m_window_delay << " seconds each." << std::endl;
        std::cout << "Wait " << m_window_size << " seconds before first measure..." << std::endl;
        
        // first cycle: init windows start times
        for (int i=0; i<nb_windows; ++i) {
            // for window i: get jiffies at start
            windows[i].jiffies_at_start = get_jiffies();
            sleep(m_window_delay);
        }
        
        // perform cycles until ctrl+C or kill
        while (!m_stop_measure) {
            
            bool limit_exceed = false;
            for (int i=0; i<nb_windows; ++i) {
                // for window i: get jiffies at end
                windows[i].jiffies_at_end = get_jiffies();
                
                // compute spent jiffies by cpu and by process
                long long jiffies_spent_by_cpu = 
                windows[i].jiffies_at_end.cpu_jiffies -
                windows[i].jiffies_at_start.cpu_jiffies;
                long long jiffies_spent_by_proc = 
                windows[i].jiffies_at_end.proc_jiffies -
                windows[i].jiffies_at_start.proc_jiffies;
                
                // compute percent of CPU spent by process
                float percent_cpu = (float) jiffies_spent_by_proc*100/jiffies_spent_by_cpu;
                
                // check limit
                if (percent_cpu >= m_cpu_limit)
                    limit_exceed=true;
                
                // print it
                print_percent_cpu(percent_cpu);
                
                // set jiffies at end as jiffies at start for next measure
                windows[i].jiffies_at_start = windows[i].jiffies_at_end;
                
                // wait delay to next window
                sleep(m_window_delay);
            }
            
            // if limit exceed once during the cycle, print alarm 
            if (limit_exceed)
            {
                std::cout << "   !! limit exceeded (" << m_cpu_limit << "%)" << std::endl;
                m_limit_exceed = true;
            } else {
                std::cout << std::endl;
            }
            
        }
        std::cout<<"stop the measure"<<std::endl;
        
        
    };
    
    /**
     * stop_measurement()
     */  
    void CpuMonitor::stop_measurement()
    {
        m_stop_measure = true;
    }
    

    
    void CpuMonitor::run() throw()
    {
        // start measurement loop
        start_measurement();
    };
    
    
}; // end of namespace 






