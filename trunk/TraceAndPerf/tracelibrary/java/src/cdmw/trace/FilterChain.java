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


package cdmw.trace;


import com.thalesgroup.CdmwTrace.ALL_COMPONENT_NAMES;
import com.thalesgroup.CdmwTrace.ALL_DOMAINS;
import com.thalesgroup.CdmwTrace.ALL_VALUES;

import java.util.BitSet;


/**
 * ECR-0123
 * In addition to the addition of a new field (the component name) to
 * the traces, the handling of trace levels has been completely rewritten.
 * The main difference with the previous implementation is that the number
 * of possible levels is now fixed and equals to the constant NB_BITS.
 */


/**
 * This class defines a sorted set of trace filters.
 * The filters are ordered by age, the most recent filter is the tail.
 * 
 * Features:
 * ThreadSafe.
 *
 */
class FilterChain {

    private java.util.LinkedList filters;
    private Filter filterAny;


    /**
     * Constructs a new empty FilterChain.
     */
    public FilterChain() {
        filters = new java.util.LinkedList();
        filterAny = new Filter();
    }


    /**
     * Add a new activated filter with specified domain and level.
     * If the level is not valid, this operation does nothing.
     *
     * @param domain The domain of the new filter.
     * @param level  The level of the new filter.
     */
    public void activateLevel(String domain, short level) {
        activateLevel(ALL_COMPONENT_NAMES.value, domain, level);
    }


    /**
     * Add a new activated filter with specified domain and level.
     *
     * @param componentName The component name.
     * @param domain The domain of the new filter.
     * @param level  The level of the new filter.
     */
    public synchronized void activateLevel(String componentName,
                                           String domain,
                                           short level)
    {
        /*
         *Filter newFilter =  new Filter(domain, level, true);
         *removeUnnecessaryFilters(newFilter);
         *
         *if (isFilterNecessary(newFilter)) {
         *    filters.add(newFilter);
         *}
         */

        // check the level
        if ((level != ALL_VALUES.value) &&
            !((level >= 0) && (level < Filter.NB_BITS)))
        {
            return;
        }

        // if we have already activated everything
        // there is no need to add a filter that activates level(s)
        if ((filters.size() == 0) &&
            (filterAny.getLevels().cardinality() == Filter.NB_BITS))
        {
            return;
        }

        if (componentName == ALL_COMPONENT_NAMES.value) {
            // activate the level(s) of any domain of any component
            if (domain == ALL_DOMAINS.value) {
                if (level == ALL_VALUES.value) {
                    // activate all the levels of the most generic filter
                    filterAny.activateAllLevels();

                    // and discard everything from the list as anything is
                    // activated from now on
                    filters.clear();
                }
                // activate a particular level of any domain of any component
                else {
                    // update the level of the most generic filter
                    filterAny.activateLevel(level);

                    // update the level of all the other filters (if any)
                    for (java.util.ListIterator it = filters.listIterator(0);
                         it.hasNext(); )
                    {
                        Filter f = (Filter) it.next();
                        f.activateLevel(level);
                    }
                }
            }

            // activate the level(s) of a particular domain of any component
            else {
                // update the level(s) of the filters that
                // correspond to the specified domain
                for (java.util.ListIterator it = filters.listIterator(0);
                     it.hasNext(); )
                {
                    Filter f = (Filter) it.next();
                    if (f.getDomain() == ALL_DOMAINS.value) {
                        if (level == ALL_VALUES.value) {
                            f.activateAllLevels();
                        }
                        else {
                            f.activateLevel(level);
                        }
                    }
                }

                // add the filter (C*, D, mask) to the list
                // if it already exists then it will be moved to the tail
                // of the list
                addFilter(ALL_COMPONENT_NAMES.value, domain, level);
            }
        }

        // componentName != CdmwTrace::ALL_COMPONENT_NAMES
        else {
            // activate level(s) of any domain of a particular component
            if (domain == ALL_DOMAINS.value) {
                // update the level(s) of the filters of the list that
                // correspond to the specified component name
                for (java.util.ListIterator it = filters.listIterator(0);
                     it.hasNext(); )
                {
                    Filter f = (Filter) it.next();
                    if (f.getComponentName() == ALL_COMPONENT_NAMES.value) {
                        if (level == ALL_VALUES.value) {
                            f.activateAllLevels();
                        }
                        else {
                            f.activateLevel(level);
                        }
                    }
                }

                // add the filter (C, D*, mask) to the list
                // if it already exists then it will be moved to the tail
                // of the list
                addFilter(componentName, ALL_DOMAINS.value, level);
            }

            // activate level(s) of a particular domain of a particular component
            else {
                // add the filter (C, D, mask) to the list
                // if it already exists then it will be moved to the tail
                // of the list
                addFilter(componentName, domain, level);
            }
        }
    }


    /**
     * Add a new deactivated filter with specified domain and level.
     *
     * @param domain The domain of the new filter.
     * @param level  The level of the new filter.
     */
    public void deactivateLevel(String domain, short level) {
        deactivateLevel(ALL_COMPONENT_NAMES.value, domain, level);
    }


    /**
     * Add a new deactivated filter with specified domain and level.
     *
     * @param componentName The component name.
     * @param domain The domain of the new filter.
     * @param level  The level of the new filter.
     */
    public synchronized void deactivateLevel(String componentName,
                                             String domain,
                                             short level)
    {
        /*
         *Filter newFilter =  new Filter(domain, level, false);
         *removeUnnecessaryFilters(newFilter);
         *
         *if (isFilterNecessary(newFilter)) {
         *    filters.add(newFilter);
         *}
         */

        // check the level
        if ((level != ALL_VALUES.value) &&
            !((level >= 0) && (level < Filter.NB_BITS)))
        {
            return;
        }

        // if we have already deactivated everything
        // there is no need to add a filter that deactivates level(s)
        if ((filters.size() == 0) &&
            (filterAny.getLevels().cardinality() == 0))
        {
            return;
        }

        if (componentName == ALL_COMPONENT_NAMES.value) {
            // deactivate the level(s) of any domain of any component
            if (domain == ALL_DOMAINS.value) {
                if (level == ALL_VALUES.value) {
                    // deactivate all the levels of the most generic filter
                    filterAny.deactivateAllLevels();

                    // and discard everything from the list as anything is
                    // activated from now on
                    filters.clear();
                }
                // deactivate a particular level of any domain of any component
                else {
                    // update the level of the most generic filter
                    filterAny.deactivateLevel(level);

                    // update the level of all the other filters (if any)
                    for (java.util.ListIterator it = filters.listIterator(0);
                         it.hasNext(); )
                    {
                        Filter f = (Filter) it.next();
                        f.deactivateLevel(level);
                    }
                }
            }

            // deactivate the level(s) of a particular domain of any component
            else {
                // update the level(s) of the filters that
                // correspond to the specified domain
                for (java.util.ListIterator it = filters.listIterator(0);
                     it.hasNext(); )
                {
                    Filter f = (Filter) it.next();
                    if (f.getDomain() == ALL_DOMAINS.value) {
                        if (level == ALL_VALUES.value) {
                            f.deactivateAllLevels();
                        }
                        else {
                            f.deactivateLevel(level);
                        }
                    }
                }

                // add the filter (C*, D, mask) to the list
                // if it already exists then it will be moved to the tail
                // of the list
                addFilter(ALL_COMPONENT_NAMES.value, domain, level, false);
            }
        }

        // componentName != CdmwTrace::ALL_COMPONENT_NAMES
        else {
            // deactivate level(s) of any domain of a particular component
            if (domain == ALL_DOMAINS.value) {
                // update the level(s) of the filters of the list that
                // correspond to the specified component name
                for (java.util.ListIterator it = filters.listIterator(0);
                     it.hasNext(); )
                {
                    Filter f = (Filter) it.next();
                    if (f.getComponentName() == ALL_COMPONENT_NAMES.value) {
                        if (level == ALL_VALUES.value) {
                            f.deactivateAllLevels();
                        }
                        else {
                            f.deactivateLevel(level);
                        }
                    }
                }

                // add the filter (C, D*, mask) to the list
                // if it already exists then it will be moved to the tail
                // of the list
                addFilter(componentName, ALL_DOMAINS.value, level, false);
            }

            // activate level(s) of a particular domain of a particular component
            else {
                // add the filter (C, D, mask) to the list
                // if it already exists then it will be moved to the tail
                // of the list
                addFilter(componentName, domain, level, false);
            }
        }
    }


    /**
     * Remove unnecessary filters in the set, before adding a new filter.
     *
     * @param newFilter The filter wich would be added.
     */
    /*
     *private synchronized void removeUnnecessaryFilters(Filter newFilter) {
     *    if (   (newFilter.getDomain().compareTo(ALL_DOMAINS.value) == 0)
     *        && (newFilter.getLevel() == ALL_VALUES.value))
     *    {
     *        filters.clear();
     *        return;
     *    }
     *
     *    for (java.util.ListIterator it = filters.listIterator(filters.size());
     *         it.hasPrevious(); )
     *    {
     *        Filter f = (Filter) it.previous();
     *
     *        if (newFilter.matches(f.getDomain(), f.getLevel())) {
     *            it.remove();
     *        }
     *
     *    }
     *}
     */



    /**
     * Indicates if a specified filter is neccessary or not to be added 
     * into the filter list.
     * 
     * @param filter The filter to be tested.
     */
    /*
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
    */


    /**
     * Check if a filter is activates for specified domain and level.
     *
     * @param domain The domain to be checked.
     * @param level  The level to be checked.
     */
    public synchronized boolean isActivated(String domain, short level) {
        /*
         *for (java.util.ListIterator it = filters.listIterator(filters.size());
         *     it.hasPrevious(); )
         *{
         *    Filter f = (Filter) it.previous();
         *
         *    if (f.matches(domain, level)) {
         *        return f.isActive();
         *    }
         *}
         *
         *return false;
         */

        return _isActivated(ALL_COMPONENT_NAMES.value, domain, level);
    }


    /**
     * Check if a filter is activates for specified domain and level.
     *
     * @param componentName The component name.
     * @param domain The domain to be checked.
     * @param level  The level to be checked.
     */
    public synchronized boolean isActivated(String componentName,
                                            String domain,
                                            short level)
    {
        return _isActivated(componentName, domain, level);
    }


    /**
     * Check if the specified level is activated.
     *
     * @param componentName component to which the domain is associated.
     * @param domain check a level of this domain.
     * @param level  level to be checked.
     *
     * @return true or false
     */
    private boolean _isActivated(String componentName,
                                 String domain,
                                 short level)
    {
        // try to find a filter with the matching component name
        // and domain (the levels are ignored)
        for (java.util.ListIterator it = filters.listIterator(0);
                it.hasNext(); )
        {
            Filter f = (Filter) it.next();
            if ((f.getComponentName() == componentName) &&
                (f.getDomain() == domain))
            {
                return f.isActivated(level);
            }
        }

        // we could not find it previously therefore we are going to
        // apply all the filters that matches either the specified
        // componentName or domain from the most recent to the oldest
        BitSet mask = new BitSet();
        boolean first = true;
        boolean noneApplied = true;

        for (java.util.ListIterator it = filters.listIterator(filters.size());
                it.hasPrevious(); )
        {
            Filter f = (Filter) it.previous();
            String componentName_ = f.getComponentName();
            String domain_ = f.getDomain();

            if ((componentName_ == ALL_COMPONENT_NAMES.value) ||
                (componentName_ == componentName))
            {
                if ((domain_ == ALL_DOMAINS.value) ||
                    (domain_ == domain))
                {
                    if (first) {
                        mask = f.getLevels();
                        first = false;
                    }
                    else {
                        mask.or(f.getLevels());
                    }

                    noneApplied = false;
                }
            }
        }

        return (noneApplied ? filterAny.isActivated(level)
                            : mask.get(level));
    }


    /**
     * Add a new Filter.
     *
     * @param componentName the component to which the domain is associated.
     * @param domain activate the specified level of this domain.
     * @param level the level to activate/deactivate
     */
    private void addFilter(String componentName,
                           String domain,
                           short level)
    {
        addFilter(componentName, domain, level, true);
    }


    /**
     * Add a new Filter.
     *
     * @param componentName the component to which the domain is associated.
     * @param domain activate the specified level of this domain.
     * @param level the level to activate/deactivate
     * @param activate if true the level will be deactivated
     */
    private void addFilter(String componentName,
                           String domain,
                           short level,
                           boolean activate)
    {
        Filter f = new Filter(componentName, domain);
        boolean isNewFilter = true;

        // look for the filter
        for (java.util.ListIterator it = filters.listIterator(0);
                it.hasNext(); )
        {
            Filter f_ = (Filter) it.next();
            if ((f_.getComponentName() == componentName) &&
                (f_.getDomain() == domain))
            {
                // make a copy of the filter
                f = f_;
                // and remove it from the list
                it.remove();

                isNewFilter = false;
                break;
            }
        }

        // update the level(s)
        if (level == ALL_VALUES.value) {
            if (activate) {
                f.activateAllLevels();
            }
            else {
                f.deactivateAllLevels();
            }
        }
        else {
            if (isNewFilter) {
                // the initial mask of the new filter is the mast that results
                // from all the filters in the list that can to the specified
                // componentName and domain

                // init the mask with the mask from the most generic filter
                BitSet mask = filterAny.getLevels();

                for (java.util.ListIterator it = filters.listIterator(0);
                        it.hasNext(); )
                {
                    Filter f_ = (Filter) it.next();
                    String componentName_ = f_.getComponentName();
                    String domain_ = f_.getDomain();

                    if ((componentName_ == ALL_COMPONENT_NAMES.value) ||
                        (componentName_ == componentName))
                    {
                        if ((domain_ == ALL_DOMAINS.value) ||
                            (domain_ == domain))
                        {
                            mask.or(f_.getLevels());
                        }
                    }
                }

                f.setLevels(mask);
            }
            
            if (activate) {
                f.activateLevel(level);
            }
            else {
                f.deactivateLevel(level);
            }
        }

        // append it to the list
        filters.add(f);
    }


    /**
     * Return the current sorted set of filters in an new array.
     *
     * @return The new array.
     */
    public synchronized Filter[] getFilters() {
        Filter[] ret = new Filter[filters.size() + 1];

        // insert the most generic filter first
        ret[0] = filterAny;

        // and then the filters from the list
        int i = 1;
        for (java.util.Iterator it = filters.iterator();
             it.hasNext(); i++)
        {
            ret[i] = (Filter) it.next();
        }

        return ret;
    }

}


