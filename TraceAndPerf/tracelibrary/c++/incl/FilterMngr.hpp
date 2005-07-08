/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
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
 * =========================================================================== */


#ifndef INCL_TRACE_FILTERMNGR_HPP 
#define INCL_TRACE_FILTERMNGR_HPP 

#include "Foundation/common/System.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
#include "TraceAndPerf/idllib/CdmwTraceCommon.stub.hpp"

#include <map>
#include <string>


/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW Trace types and data.
*/
namespace Trace
{

/**
*Purpose:
*<p>    Contains all information defining a filter. A filter is
*   a combination of a user level and domain. A filter is enabled
*   when the user has ask to activate the specified Level/Domain
*   If a Level/Domain has never been activated, it is considered as
*   disabled.
*
*Features:
*<p> No thread safe
*
*
*/
class Filter {

    public:

       /**
       *Purpose:
       *<p> Contruct a filter defined the specified Domain/Level
       */
       Filter (const std::string& domain, long level);


       /**
       *Purpose:
       *<p> Allows filter to be sorted by STL algorithm.
       *
       */
       friend bool operator < (const Filter& lhs, const Filter& rhs);



       /**
       *Purpose:
       *<p> Returns the domain managed by this filter
       *
       */
       const std::string& get_domain() const;


       /**
       *Purpose:
       *<p> Returns the level managed by this filter
       *
       */
       const long get_level() const;
       

    private:

       /**
       * Domain name associated with this filter.
       */
       std::string m_domain;

       /**
       * Which level in the Domain is associted with this filter.
       */
       long m_level;
};


/**
*Purpose:
*<p>    Holds all the filter currently activated in the process.
*
*Features:
*<p> Thread safe
*
*@see  Filter 
*
*/
class FilterMngr 
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        */ 
        FilterMngr()
                throw();


        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~FilterMngr()
                throw();





        /**
        * Purpose:
        * <p> Defines the specified level as activated
        * 
        *@param domain the domain where the level must be activated
        *@param level  the level to be activated
        *
        *@exception OutOfMemoryException
        */ 
        void activate_level (const std::string& domain, const long level)
            throw (OutOfMemoryException);


        

        /**
        * Purpose:
        * <p> Defines the specified level as deactivated
        * 
        *@param domain the domain where the level must be deactivated
        *@param level  the level to be deactivated
        *
        *@exception OutOfMemoryException
        *
        */ 
        void deactivate_level (const std::string& domain, const long level)
            throw (OutOfMemoryException);

        /**
		* Purpose:
		* <p> get the sequence of registered domain/level activated or not filters 
		* 
		*@return The sequence of trace filters
		*
		*@exception CORBA::SystemException
		*/ 
		CdmwTrace::TraceFilterSeq* get_trace_filters ()
		    throw(OutOfMemoryException);
				

        
        /**
        * Purpose:
        * <p> Returns true if the specified level is activated
        * 
        *@param domain the domain of trace message
        *@param level  the level of trace message
        *
        *@return true if the level is activated, false elsewhere
        *
        *@exception OutOfMemoryException
        */ 
        bool is_activated (const std::string& domain, long level)
            throw (OutOfMemoryException);


    protected:


    private:


        /**
        * Purpose:
        * <p>  Copy constructor 
        * 
        */ 
        FilterMngr(const FilterMngr& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignement operator d
        * 
        */ 
        FilterMngr&
        operator=(const FilterMngr& rhs)
                throw();


        /**
        * Purpose:
        * <p> Check if the specified filter is activated
        * 
        *@param domain the domain of trace message
        *@param level  the level of trace message
        *
        *@return true if the level is activated, false elsewhere
        *
        *@exception OutOfMemoryException
        */ 
        bool check_activated_filter (const std::string& domain, long level)
            throw (OutOfMemoryException);

        /**
        * This variable indicates if all domains and level filter is activated
        * if set, messages from all domains with any level can be traced
        */
        bool m_all_domains_levels;

        /**
        * This variable indicates if all domains filter has been defined in filter container
        */
        bool m_all_domains;

        /**
        * This variable indicates if all level filter has been defined in filter container
        */
        bool m_all_levels;


        /**
        * This variable indicates the data filter : activation and order
        */
        struct FilterData
        {
            /**
            * activation of filter
            */
            bool   activation;
            
            /**
            * user activation order of filter
            * : 0 filter is inhibited
            * : 1 low order for D*,L and D,L* filters
            * : 2 medium order for D*,L and D,L* filters
            * : 3 high order for D,L filter
            * : 4 high order for D,L filter but not visible
            */
            int    order;
        };
        
        /**
        * Defines the container used to store all filter activated
        * or not
        */
        typedef std::map <Filter, FilterData> FilterCtr;




        /**
        * This variable contains all the filter currently register
        * activated or not. This variable must be manipulated with
        * the Mutex m_filterMngr_mtx acquired.
        */
        FilterCtr m_filters;



        /**
        * Mutex protecting all access to the FilterMngr.
        */
        OsSupport::Mutex m_filterMngr_mtx;

        

}; // End class FilterMngr 

} // End namespace Trace
} // End namespace Cdmw
#endif // INCL_TRACE_FILTERMNGR_HPP

