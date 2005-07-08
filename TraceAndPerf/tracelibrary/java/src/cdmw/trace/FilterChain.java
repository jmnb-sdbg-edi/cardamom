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


package cdmw.trace;

import com.thalesgroup.CdmwTrace.ALL_DOMAINS;
import com.thalesgroup.CdmwTrace.ALL_VALUES;


/**
 * This class defines a sorted set of trace filters.
 * The trace filters are sorted from older to more recent filter.
 * Unnecessary filters are removed from the set when adding a new filter.
 * 
 * Features:
 * ThreadSafe.
 *
 */
class FilterChain {

    /**
     * List of filters
     */
    private java.util.LinkedList filters;

    /**
     * Constructs a new empty FilterChain.
     */
    public FilterChain() {
        filters = new java.util.LinkedList();
    }

    /**
     * Add a new activated filter with specified domain and level.
     *
     * @param domain The domain of the new filter.
     * @param level  The level of the new filter.
     */
    public synchronized void activateLevel(String domain, short level) {
        Filter newFilter =  new Filter(domain, level, true);
        removeUnnecessaryFilters(newFilter);
        
        if (isFilterNecessary(newFilter)) {
            filters.add(newFilter);
        }
    }

    /**
     * Add a new deactivated filter with specified domain and level.
     *
     * @param domain The domain of the new filter.
     * @param level  The level of the new filter.
     */
    public synchronized void deactivateLevel(String domain, short level) {
        Filter newFilter =  new Filter(domain, level, false);
        removeUnnecessaryFilters(newFilter);

        if (isFilterNecessary(newFilter)) {
            filters.add(newFilter);
        }
    }


    /**
     * Remove unnecessary filters in the set, before adding a new filter.
     *
     * @param newFilter The filter wich would be added.
     */
    private synchronized void removeUnnecessaryFilters(Filter newFilter) {
        if (   (newFilter.getDomain().compareTo(ALL_DOMAINS.value) == 0)
            && (newFilter.getLevel() == ALL_VALUES.value))
        {
            filters.clear();
            return;
        }

        for (java.util.ListIterator it = filters.listIterator(filters.size());
             it.hasPrevious(); )
        {
            Filter f = (Filter) it.previous();

            if (newFilter.matches(f.getDomain(), f.getLevel())) {
                it.remove();
            }

        }
    }



    /**
     * Indicates if a specified filter is neccessary or not to be added 
     * into the filter list.
     * 
     * @param filter The filter to be tested.
     */
    private boolean isFilterNecessary(Filter newFilter) {
        for (java.util.ListIterator it = filters.listIterator(filters.size());
             it.hasPrevious(); )
        {
            // Get filter from list in reverse order
            Filter f = (Filter) it.previous();

            // Test if f has common filter with newFilter, and
            // if they have different activation
            if (   f.hasCommonFilterWith(newFilter) 
                && (f.isActive() != newFilter.isActive())
               )
            {
                // If true, it means that new filter override f for 
                // some sub-filters. NewFilter is necessary.
                return true;
            }

            // Test if f match the newFilter.
            // (No need to test activation. If activation was different,
            //  precedent test would have return true.)
            if (f.matches(newFilter.getDomain(), newFilter.getLevel())) {
                // if true, it means that newFilter is include in f.
                // NewFilter is not necessary.
                return false;
            }
            
        }

        // If we are there, the last filter is (* * false) by default.
        // NewFilter is necessary only if it is activated.
        return newFilter.isActive();        
           
    }




    /**
     * Check if a filter is activates for specified domain and level.
     *
     * @param domain The domain to be checked.
     * @param level  The level to be checked.
     */
    public synchronized boolean isActivated(String domain, short level) {
        for (java.util.ListIterator it = filters.listIterator(filters.size());
             it.hasPrevious(); )
        {
            Filter f = (Filter) it.previous();

            if (f.matches(domain, level)) {
                return f.isActive();
            }
        }

        return false;
    }


    /**
     * Return the current sorted set of filters in an new array.
     *
     * @return The new array.
     */
    public synchronized Filter[] getFilters() {
        Filter[] ret = new Filter[filters.size()];

        int i = 0;
        for (java.util.Iterator it = filters.iterator();
             it.hasNext(); i++)
        {
            ret[i] = (Filter) it.next();
        }

        return ret;
    }

}


