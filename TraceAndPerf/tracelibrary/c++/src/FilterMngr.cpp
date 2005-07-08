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


#include "TraceAndPerf/tracelibrary/FilterMngr.hpp"

#include "Foundation/osthreads/MutexGuard.hpp"


namespace Cdmw
{
namespace Trace
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


    
//
// Ctor
//
Filter::Filter (const std::string& domain, long level)
   : m_domain(domain),
     m_level(level)
{
   
}



//
// Allow the the Filter to be sorted by STL
//
bool operator < (const Filter& lhs, const Filter& rhs) 
{
   if (lhs.m_domain < rhs.m_domain) 
   {
        return true;
   } 
   else if (lhs.m_domain == rhs.m_domain) 
   {
        return lhs.m_level < rhs.m_level;
   }
    
   return false;
}


const std::string& Filter::get_domain () const
{
  return m_domain;
}

const long Filter::get_level() const
{
  return m_level;
}

//
// Ctor
//
FilterMngr::FilterMngr()
    // FIXME: map may throw, but no way to catch it without
    //  function body try/catch
    throw()
	: m_all_domains_levels(false),
	  m_all_domains(false),
	  m_all_levels(false)
{

}




//
// Dtor
//
FilterMngr::~FilterMngr()
    throw()
{

}



//
// Define the specified level as activated
//
void FilterMngr::activate_level (const std::string& domain, const long level) 
    throw (OutOfMemoryException)

{
    CDMW_MUTEX_GUARD (m_filterMngr_mtx);

    try 
	{	    
	   // if all domains and levels to activate : all messages are traced
	   if (domain == CdmwTrace::ALL_DOMAINS && level == CdmwTrace::ALL_VALUES)
	   {
	     m_all_domains_levels = true;
	     
	     // suppress all filters of map
	     m_filters.clear();
	     m_all_domains = false;
	     m_all_levels = false;

	   }
	
	   // if all domains filter or all levels filter to activate
	   else if (domain == CdmwTrace::ALL_DOMAINS || level == CdmwTrace::ALL_VALUES)
       {	       
	     // set insertion flag : false if all domains and levels activated
	     //   (may be not necessary to insert this filter)
	     bool insert_filter = true;	
	     if (m_all_domains_levels == true)
	     {
	       insert_filter = false;
	     }
	     
	     long level_of_filter;
	     std::string domain_of_filter;

	     // if all domains to activate : activate all defined filters with the specified level 
		 // if filter with all level is found with high order, set its order to low
		 //   and force insertion flag to true if it was deactivated to override it
	     if (domain == CdmwTrace::ALL_DOMAINS)
	     {
           FilterCtr::iterator itrFilter = m_filters.begin(); 
	       for ( ; itrFilter != m_filters.end(); itrFilter++)
	       {
	         level_of_filter = ((*itrFilter).first).get_level();
	         domain_of_filter = ((*itrFilter).first).get_domain();

	         if (level_of_filter == level)
	         {
	           (*itrFilter).second.activation = true;
	           (*itrFilter).second.order = 4;
	         }
			 else if (level_of_filter == CdmwTrace::ALL_VALUES)
	         {
	           if ((*itrFilter).second.order != 0)
	           {
			     // set low order for all levels filter if different of all domains / all levels
			     // if it is equivalent it can be inhibited
                 if ((*itrFilter).second.activation == m_all_domains_levels)
                 {
                   (*itrFilter).second.order = 0;
				 }
				 else
                 {
                   (*itrFilter).second.order = 1;
                 }
                 
                 if ((*itrFilter).second.activation == false)
                 {
                   insert_filter = true;
                 }  
               }
	         }
	       }
	       
	       m_all_domains = true;
	     }
	     
	     // if all levels to activate : activate all defined filters with the specified domain
		 // if filter with all domain is found with high order, set its order to low
		 //   and force insertion flag to true if it was deactivated to override it
	     else
	     {
           FilterCtr::iterator itrFilter = m_filters.begin(); 
	       for ( ; itrFilter != m_filters.end(); itrFilter++)
	       {
	         level_of_filter = ((*itrFilter).first).get_level();
	         domain_of_filter = ((*itrFilter).first).get_domain();

	         if (domain_of_filter == domain)
	         {
	           (*itrFilter).second.activation = true;
	           (*itrFilter).second.order = 4;
	         }
			 else if (domain_of_filter == CdmwTrace::ALL_DOMAINS)
	         {
               if ((*itrFilter).second.order != 0)
	           {
			     // set low order for all levels filter if different of all domains / all levels
			     // if it is equivalent it can be inhibited
                 if ((*itrFilter).second.activation == m_all_domains_levels)
                 {
                   (*itrFilter).second.order = 0;
				 }
				 else
                 {
                   (*itrFilter).second.order = 1;
                 }
                 
                 if ((*itrFilter).second.activation == false)
                 {
                   insert_filter = true;
                 }
               }
	         }
	       }
	       
	       m_all_levels = true;
		 }
		 	 

         // if filter to insert
         if (insert_filter == true)
         {
           // create filter if does not exist and activate it
           FilterData data;
           data.activation = true;
           data.order = 2;
        
           m_filters[Filter(domain,level)] = data;
         }
 
         // filter has not to be inserted as it does not change filtering
         // but if it exists in map, inhibit it        
         else
         {
           // find the filter
           FilterCtr::iterator itr = m_filters.find (Filter(domain,level));
         
           // If it exists, inhibit it with order 0
           if (itr != m_filters.end())
	       {
	         itr->second.order = 0;
	         itr->second.activation = true;
	       }
         }
       }
       
       // else specific domain/level filter to activate
       else
	   {
		 // if filter already activated, ignore the command	
	     if (check_activated_filter (domain, level) == true)
	     {
	       return;
	     } 
	       

         // search if filter already created
         FilterCtr::iterator itr = m_filters.find (Filter(domain,level));
         
         // If it exists, check if it is necessary for filtering
         if (itr != m_filters.end())
	     {
	       // first step inhibit filter and check result filtering
	       itr->second.order = 0;
	       itr->second.activation = true;
	       
	       // if request filtering not activated, inhibited specific filter
	       // must be activated with high order	
	       if (check_activated_filter (domain, level) == false)
	       {
	         itr->second.order = 3;
	       } 
	       // else set high order but not visible
	       else
	       {
	         itr->second.order = 4;
	       }	      
	     }
	     
	     // create filter if does not exist and activate it
	     else
	     {
           FilterData data;
           data.activation = true;
           data.order = 3;

           m_filters[Filter(domain,level)] = data;
         }
       }
    }
   
   
    catch (std::bad_alloc&) 
    {
      CDMW_THROW (OutOfMemoryException);
    }
}




//
// Define the specified level as de-activated
//
void FilterMngr::deactivate_level (const std::string& domain, const long level)
    throw (OutOfMemoryException)

{
   CDMW_MUTEX_GUARD (m_filterMngr_mtx);

   try 
   {    
	   // if all domains and levels to deactivate : no message are traced
	   if (domain == CdmwTrace::ALL_DOMAINS && level == CdmwTrace::ALL_VALUES)
	   {
	     m_all_domains_levels = false;

	     // suppress all filters of map
	     m_filters.clear();
	     m_all_domains = false;
	     m_all_levels = false;
	   }
	   
	   // if all domains filter or all levels filter to deactivate
	   else if (domain == CdmwTrace::ALL_DOMAINS || level == CdmwTrace::ALL_VALUES)
       {	       
	     // set insertion flag : false if all domains and levels deactivated
	     //   (may be not necessary to insert this filter)
	     bool insert_filter = true;	
	     if (m_all_domains_levels == false)
	     {
	       insert_filter = false;
	     }
	     
	   	         
	     long level_of_filter;
	     std::string domain_of_filter;

	     // if all domains to deactivate : deactivate all filters with the specified level 
	     // if filter with all level is found, set its order to low
	     //   and force insertion flag to true if it was activated to override it
	     if (domain == CdmwTrace::ALL_DOMAINS)
	     {
           FilterCtr::iterator itrFilter = m_filters.begin(); 
	       for ( ; itrFilter != m_filters.end(); itrFilter++)
	       {
	         level_of_filter = ((*itrFilter).first).get_level();
	         domain_of_filter = ((*itrFilter).first).get_domain();

	         if (level_of_filter == level)
	         {
	           (*itrFilter).second.activation = false;
	           (*itrFilter).second.order = 4;
	         }
			 else if (level_of_filter == CdmwTrace::ALL_VALUES)
	         {
	           if ((*itrFilter).second.order != 0)
	           {
			     // set low order for all levels filter if different of all domains / all levels
			     // if it is equivalent it can be inhibited
                 if ((*itrFilter).second.activation == m_all_domains_levels)
                 {
                   (*itrFilter).second.order = 0;
				 }
				 else
                 {
                   (*itrFilter).second.order = 1;
                 }
                 
                 if ((*itrFilter).second.activation == true)
                 {
                   insert_filter = true;
                 }
               }
	         }
	       }
	       
	       m_all_domains = true;
	     }
	     
	     // if all levels to deactivate : deactivate all filters with the specified domain
		 // if filter with all domain is found, set its order to low
		 //   and force insertion flag to true if it was deactivated to override it
	     else if (level == CdmwTrace::ALL_VALUES)
	     {
           FilterCtr::iterator itrFilter = m_filters.begin(); 
	       for ( ; itrFilter != m_filters.end(); itrFilter++)
	       {
	         level_of_filter = ((*itrFilter).first).get_level();
	         domain_of_filter = ((*itrFilter).first).get_domain();

	         if (domain_of_filter == domain)
	         {
	           (*itrFilter).second.activation = false;
	           (*itrFilter).second.order = 4;
	         }
			 else if (domain_of_filter == CdmwTrace::ALL_DOMAINS)
	         {
	           if ((*itrFilter).second.order != 0)
	           {            
			     // set low order for all levels filter if different of all domains / all levels
			     // if it is equivalent it can be inhibited
                 if ((*itrFilter).second.activation == m_all_domains_levels)
                 {
                   (*itrFilter).second.order = 0;
				 }
				 else
                 {
                   (*itrFilter).second.order = 1;
                 }
                 
	             if ((*itrFilter).second.activation == true)
                 {
                   insert_filter = true;
                 }
               }
	         }
	       }
	       
	       m_all_levels = true;
         }
         
         
         // if filter to insert
         if (insert_filter == true)
         {
           // create filter if does not exist and deactivate it
           FilterData data;
           data.activation = false;
           data.order = 2;

           m_filters[Filter(domain,level)] = data;          
         }
 
         // filter has not to be inserted as it does not change filtering
         // but if it exists in map, inhibit it        
         else
         {
           // find the filter
           FilterCtr::iterator itr = m_filters.find (Filter(domain,level));
         
           // If it exists, inhibit it with order 0
           if (itr != m_filters.end())
	       {
	         itr->second.order = 0;
	         itr->second.activation = false;
	       }
         }		
       }
       
       
       // else specific domain/level filter to deactivate
       else
	   {
		 // if filter already deactivated, ignore it	
	     if (check_activated_filter (domain, level) == false)
	     {
	       return;
	     } 
	       
	       
	     // search if filter already created
         FilterCtr::iterator itr = m_filters.find (Filter(domain,level));
         
         // If it exists, check if it is necessary for filtering
         if (itr != m_filters.end())
	     {
	       // first step inhibit filter and check result filtering
	       itr->second.order = 0;
	       itr->second.activation = false;
	       
	       // if request filtering not deactivated, inhibited specific filter
	       // must be deactivated with high order	
	       if (check_activated_filter (domain, level) == true)
	       {
	         itr->second.order = 3;
	       } 
	       // else set high order but not visible
	       else
	       {
	         itr->second.order = 4;
	       }	      
	     }
	     
	     // create filter if does not exist and deactivate it
	     else
	     {
           FilterData data;
           data.activation = false;
           data.order = 3;

           m_filters[Filter(domain,level)] = data;
         }
       }
    }
   
   
    catch (std::bad_alloc&) 
    {
      CDMW_THROW (OutOfMemoryException);
    }
}


//
// Get the activated or not level filters
//
CdmwTrace::TraceFilterSeq* FilterMngr::get_trace_filters ()
    throw (OutOfMemoryException)
{
    try
	{
	  // allocate the level sequence with the number of activated levels
      CdmwTrace::TraceFilterSeq* p_filterSeq = new CdmwTrace::TraceFilterSeq;

      // set sequence length
      unsigned int seq_len = m_filters.size() + 1;    
      p_filterSeq->length(seq_len);
       
      // add all domains / all values levels
      (*p_filterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	  (*p_filterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	  (*p_filterSeq)[0].activation = m_all_domains_levels;
    
      // initialize sequence index
      unsigned int inx = 1;
      
      
      FilterCtr::iterator itrFilter;
 
      // first fill sequence with filter D*,L or D,L* having low order
      // then  fill sequence with filter D*,L or D,L* having medium order
      // then  fill sequence with filter D,L having high order but visible
      for (int order = 1 ; order <= 3 ; order++)
      {         
        itrFilter = m_filters.begin(); 
	    for ( ; itrFilter != m_filters.end(); itrFilter++)
	    {
		  if ((*itrFilter).second.order == order)
          {
		    // extract domain and level to store in sequence
		    (*p_filterSeq)[inx].the_value = ((*itrFilter).first).get_level();
	        (*p_filterSeq)[inx].the_domain = CORBA::string_dup(((*itrFilter).first).get_domain().c_str());
	        (*p_filterSeq)[inx].activation = (*itrFilter).second.activation;
	        inx++;
          }
        }
      }  
      
      // resize sequence to correct length with selected filter
	  if (inx < seq_len)
      {
	    p_filterSeq->length(inx);
      }

      return p_filterSeq;
    }
    catch (std::bad_alloc&)
    {
      CDMW_THROW (OutOfMemoryException);
	}
}
    

//
// Returns if the specified level is activated
//
bool FilterMngr::is_activated (const std::string& domain, long level) 
    throw (OutOfMemoryException)

{
   CDMW_MUTEX_GUARD (m_filterMngr_mtx);

   return check_activated_filter (domain, level) ;
}

//
// Check if filter is activated (no Mutex used)
//
bool FilterMngr::check_activated_filter (const std::string& domain, long level) 
    throw (OutOfMemoryException)

{
   // WARNING : Mutex m_filterMngr_mtx must have been previously acquired !!
   
   try 
   {    
	   // by default message is not traced
	   bool trace_mesg = false;

       // filter value = -1 if undefined
       //              = 0  if deactivated
       //              = 1  if activated
       int all_domain_filter = -1;
       int all_level_filter  = -1;
       
       // filter order = inhibited
       //              = low order
       //              = medium order
       //              = high order
       int all_domain_order = -1;
       int all_level_order = -1;
       
       bool filter_found = false;
       
       FilterCtr::iterator itr;    
         
       // find the specific domain level filter activation
       itr = m_filters.find (Filter(domain,level));
         
       // If the specific domain / level exists and not inhibited: 
       //   get its boolean status
       if (itr != m_filters.end())
	   {
	     if (itr->second.order != 0)
	     {
	       trace_mesg = itr->second.activation;
	       filter_found = true;
	     }
       }
       
       // if no specific filter has been found
       if (filter_found == false)
       {     
         if (m_all_domains == true)
         {
           // find the all domains filter activation
           itr = m_filters.find (Filter(CdmwTrace::ALL_DOMAINS,level));
         
           // If the all domain / specific level exist : is it activated ?
           //   get its boolean status
           if (itr != m_filters.end())
	       {
	         if (itr->second.order != 0)
	         {
               all_domain_filter = 0;
               if (itr->second.activation == true)
               {
                 all_domain_filter = 1;
               }  
           
               all_domain_order = itr->second.order;
             }
           }
         }
    
         if (m_all_levels == true)
         {
           // find the all levels filter activation
           itr = m_filters.find (Filter(domain,CdmwTrace::ALL_VALUES));
         
           // If the specific domain / all level exist : is it activated ?
           //   get its boolean status
           if (itr != m_filters.end())
	       {
	         if (itr->second.order != 0)
	         {
               all_level_filter = 0;
               if (itr->second.activation == true)
               {
                 all_level_filter = 1;
               }  
           
               all_level_order = itr->second.order;
             }
           }
         }
       
       
         // if D*,L and D,L* filters exist
	     if (all_domain_filter != -1 && all_level_filter != -1)
	     {
	       // if D*,L filter has the highest order (it is the latest)
	       if (all_domain_order == 2)
	       {
	         if (all_domain_filter == 0)
	         {
	           trace_mesg = false;
	         }
	         else
	         {
	           trace_mesg = true;
	         }
	       }
	       // else D,L* filter has the highest order (it is the latest)
	       else
	       {
	         if (all_level_filter == 0)
	         {
	           trace_mesg = false;
	         }
	         else
	         {
	           trace_mesg = true;
	         }
	       }
	     }
	   
	     // if D*,L filter exist, gets its activation
	     else if (all_domain_filter != -1)
	     {
	       if (all_domain_filter == 0)
	       {
	         trace_mesg = false;
	       }
	       else
	       {
	         trace_mesg = true;
	       }
	     }
	    
	     // if D,L* filter exist, get its activation
	     else if (all_level_filter != -1)
	     {
	       if (all_level_filter == 0)
	       {
	         trace_mesg = false;
	       }
	       else
	       {
	         trace_mesg = true;
	       }
	     }
	    
	     // else no filter for this domain and this level
	     // then get all domain / all level *,* filter activation
	     else
	     {
	       trace_mesg = m_all_domains_levels;
	     }
	   }
	   
       return trace_mesg;
   } 
   catch (std::bad_alloc&) 
   {
        CDMW_THROW (OutOfMemoryException);
   }
}

} // End namespace Trace
} // End namespace Cdmw


