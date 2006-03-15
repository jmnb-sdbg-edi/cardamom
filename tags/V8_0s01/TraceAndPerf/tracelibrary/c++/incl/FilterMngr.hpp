/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
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


#ifndef _INCL_TRACE_FILTERMNGR_HPP_
#define _INCL_TRACE_FILTERMNGR_HPP_


#include <bitset>
#include <list>
#include <stdexcept>
#include <sstream>
#include <string>

#include <Foundation/common/Exception.hpp>
#include <Foundation/common/System.hpp>
#include <Foundation/osthreads/Mutex.hpp>

#include <TraceAndPerf/idllib/CdmwTraceCommon.stub.hpp>


/**
 * ECR-0123
 * In addition to the addition of a new field (the component name) to
 * the traces, the handling of trace levels has been completely rewritten.
 * The main difference with the previous implementation is that the number
 * of possible levels is now fixed and equals to the constant NB_BITS.
 */


namespace
{
    const size_t NB_BITS = 32;
    typedef std::bitset<NB_BITS> bitmask;
}


namespace Cdmw
{

namespace Trace
{


// {{{ class Filter

/**
 * Define a filter that will either let a message be sent to a collector
 * or prevent it from reaching any collector.
 * At any time, a filter is either activated or deactivated.
 * There are NB_BITS levels.
 */
class Filter
{
    public:
        /**
         * Create a new Filter with a generic component name and
         * a generic domain.
         */
        Filter(void);


        /**
         * Create a new Filter with a generic component name and
         * all the levels deactivated.
         *
         * @param domain the domain to match
         */
        Filter(const std::string& domain);


        /**
         * Create a new Filter with a generic component name.
         *
         * @param domain the domain to match
         * @param levels bit mask for the trace levels
         */
        Filter(const std::string& domain,
               const bitmask levels);


        /**
         * Create a new Filter with all the levels deactivated.
         *
         * @param componentName the component name to match
         * @param domain the domain to match
         */
        Filter(const std::string& componentName,
               const std::string& domain);


        /**
         * Create a new Filter.
         *
         * @param componentName the component name to match
         * @param domain the domain to match
         * @param levels bit mask for the trace levels
         */
        Filter(const std::string& componentName,
               const std::string& domain,
               const bitmask levels);


        /**
         * @return the component name.
         */
        const std::string&
        get_component_name(void) const
        {
            return m_componentName;
        }


        /**
         * @return the domain.
         */
        const std::string&
        get_domain(void) const
        {
            return m_domain;
        }


        /**
         * @return the levels.
         */
        const bitmask&
        get_levels(void) const
        {
            return m_levels;
        }


        /**
         * @param the new levels.
         */
        void
        set_levels(bitmask levels)
        {
            m_levels = levels;
        }


        /**
         * @param level the level to activate
         */
        void
        activate_level(const size_t level)
            throw(BadParameterException)
        {
            try {
                m_levels.set(level);
            }
            catch (std::out_of_range&) {
                std::ostringstream oss;
                oss << level << std::ends;
                CDMW_THROW2(BadParameterException, "level", oss.str());
            }
        }


        /**
         * Activate all levels.
         */
        void
        activate_all_levels(void)
        {
            m_levels.set();
        }


        /**
         * @param level the level to deactivate
         */
        void
        deactivate_level(const size_t level)
            throw(BadParameterException)
        {
            try {
                m_levels.set(level, 0);
            }
            catch (std::out_of_range&) {
                std::ostringstream oss;
                oss << level << std::ends;
                CDMW_THROW2(BadParameterException, "level", oss.str());
            }
        }


        /**
         * Deactivate all levels.
         */
        void
        deactivate_all_levels(void)
        {
            m_levels.reset();
        }


        /**
         * Check if a level is activated.
         *
         * @return true or false
         */
        bool
        is_activated(const size_t level)
            throw(BadParameterException)
        {
            bool retval = false;

            try {
                retval = m_levels[level];
            }
            catch (std::out_of_range&) {
                std::ostringstream oss;
                oss << level << std::ends;
                CDMW_THROW2(BadParameterException, "level", oss.str());
            }

            return retval;
        }


    private:
        std::string m_componentName;
        std::string m_domain;
        bitmask m_levels;

}; // }}}


// {{{ class FilterMngr

/**
 * A class that provides operations for activating/deactivating
 * one or more Filters.
 */
class FilterMngr
{
    public:
        /**
         * Constructor.
         */
        FilterMngr(void)
            throw();


        /**
         * Destructor.
         */
        virtual
        ~FilterMngr(void)
            throw();


        /**
         * Activate the specified level.
         *
         * @param domain activate the specified level of this domain.
         * @param level the level to activate.
         *
         * @exception OutOfMemoryException
         */
        void
        activate_level(const std::string& domain,
                       const long level)
            throw(OutOfMemoryException);


        /**
         * Activate the specified level.
         *
         * @param componentName the component to which the domain is associated.
         * @param domain activate the specified level of this domain.
         * @param level the level to activate.
         *
         * @exception OutOfMemoryException
         */
        void
        activate_level(const std::string& componentName,
                       const std::string& domain,
                       const long level)
            throw(OutOfMemoryException);


        /**
         * Deactivate the specified level.
         *
         * @param domain deactivate the specified level of this domain.
         * @param level the level to activate.
         *
         * @exception OutOfMemoryException
         */
        void
        deactivate_level(const std::string& domain,
                         const long level)
            throw(OutOfMemoryException);


        /**
         * Deactivate the specified level.
         *
         * @param componentName the component to which the domain is associated.
         * @param domain activate the specified level of this domain.
         * @param level the level to activate.
         *
         * @exception OutOfMemoryException
         */
        void
        deactivate_level(const std::string& componentName,
                         const std::string& domain,
                         const long level)
            throw(OutOfMemoryException);


        /**
         * @return a sequence of filters whether activated or not.
         *
         * @exception OutOfMemoryException
         */
        CdmwTrace::TraceFilterSeq*
        get_trace_filters(void)
            throw(OutOfMemoryException);


        /**
         * Check if the specified level is activated.
         *
         * @param domain activate the specified level of this domain.
         * @param level the level to activate.
         *
         * @return true or false.
         *
         * @exception BadParameterException
         * @exception OutOfMemoryException
         */
        bool
        is_activated(const std::string& domain,
                     const long level)
            throw(BadParameterException,
                  OutOfMemoryException);


        /**
         * Check if the specified level is activated.
         *
         * @param componentName component to which the domain is associated.
         * @param domain activate the level for this domain.
         * @param level level to activate.
         *
         * @return true or false.
         *
         * @exception BadParameterException
         * @exception OutOfMemoryException
         */
        bool
        is_activated(const std::string& componentName,
                     const std::string& domain,
                     const long level)
            throw(BadParameterException,
                  OutOfMemoryException);


    private:
        /**
         * Copy constructor.
         */
        FilterMngr(const FilterMngr& rhs)
            throw();


        /**
         * Assignment operator.
         */
        FilterMngr&
        operator=(const FilterMngr& rhs)
            throw();


        /**
         * Check if the specified level is activated.
         * Not thread-safe.
         *
         * @param componentName component to which the domain is associated.
         * @param domain activate the level for this domain.
         * @param level level to activate.
         *
         * @return true or false.
         *
         * @exception BadParameterException
         * @exception OutOfMemoryException
         */
        bool
        _is_activated(const std::string& componentName,
                      const std::string& domain,
                      const long level)
            throw(BadParameterException,
                  OutOfMemoryException);


        /**
         * Add a new Filter.
         *
         * @param componentName the component to which the domain is associated.
         * @param domain activate the specified level of this domain.
         * @param level the level to activate/deactivate
         * @param activate if true the level will be deactivated
         */
        void
        add_filter(const std::string& componentName,
                   const std::string& domain,
                   const long level,
                   bool activate = true)
            throw(OutOfMemoryException);


        /**
         * Class attributes.
         */


        // A container to store all the filters.
        // The mutex must be aquired prior any manipulations.
        typedef std::list<Filter> FilterCtr;
        FilterCtr m_filters;


        // mutex to control the access to the list of filters
        OsSupport::Mutex m_filterMngr_mtx;


        // the most generic filter (C*, D*, mask)
        Filter m_filter_any;


}; // end class FilterMngr

// }}}


} // End namespace Trace

} // End namespace Cdmw


#endif // _INCL_TRACE_FILTERMNGR_HPP_
