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
 * Define a filter that will either let a message be sent to a collector
 * or prevent it from reaching any collector.
 * At any time, a filter is either activated or deactivated.
 * There are NB_BITS levels.
 */
class Filter {
    public static short NB_BITS = 32;


    private String componentName;
    private String domain;
    private BitSet levels;


    /**
     * The level of this filter.
     */ 
    //private short level;

    /**
     * Indicate if the filter is activated or not.
     */
    //private boolean active;


    /**
     * Create a new Filter with a generic component name and
     * a generic domain.
     */
    public Filter()
    {
        this.componentName = ALL_COMPONENT_NAMES.value;
        this.domain = ALL_DOMAINS.value;
        this.levels = new BitSet(NB_BITS);
    }


    /**
     * Create a new Filter.
     *
     * @param domain the domain to match
     */
    public Filter(String domain)
    {
        this.componentName = ALL_COMPONENT_NAMES.value;
        this.domain = domain;
        this.levels = new BitSet(NB_BITS);
    }


    /**
     * Create a new Filter.
     *
     * @param domain the domain to match
     * @param levels bit mask for the trace levels
     */
    public Filter(String domain, BitSet levels)
    {
        this.componentName = ALL_COMPONENT_NAMES.value;
        this.domain = domain;
        this.levels = levels;
    }


    /**
     * Create a new Filter.
     *
     * @param componentName the component name to match
     * @param domain the domain to match
     */
    public Filter(String componentName, String domain)
    {
        this.componentName = componentName;
        this.domain = domain;
        this.levels = new BitSet(NB_BITS);
    }


    /**
     * Create a new Filter.
     *
     * @param componentName the component name to match
     * @param domain the domain to match
     * @param levels bit mask for the trace levels
     */
    public Filter(String componentName, String domain, BitSet levels)
    {
        this.componentName = componentName;
        this.domain = domain;
        this.levels = levels;
    }


    /**
     * Constructs a new filter, activated or not, 
     * for specified domain and level.
     */
    /*
     *public Filter(String domain,
     *              short level,
     *              boolean active)
     *{
     *    this.domain = domain;
     *    this.level = level;
     *    this.active = active;
     *}
     */


    /**
     * Get the component name.
     *
     * @return the component name
     */
    public String getComponentName()
    {
        return componentName;
    }


    /**
     * Return the domain for this filter
     *
     * @return the domain
     */
    public String getDomain() {
        return domain;
    }


    /**
     * Return the level for this filter
     *
     * @return the level
     */
    /*
     *public short getLevel() {
     *    return level;
     *}
     */


    /**
     * Get the levels.
     *
     * @return the bit mask.
     */
    public BitSet getLevels()
    {
        return levels;
    }


    /**
     * Set the levels.
     *
     * @param levels the new levels.
     */
    public void setLevels(BitSet levels)
    {
        this.levels = levels;
    }


    /**
     * @param level the level to activate
     */
    public void activateLevel(short level)
    {
        levels.set(level);
    }


    /**
     * Activate all levels.
     */
    public void activateAllLevels()
    {
        levels.clear();
        levels.flip(0, NB_BITS);
    }


    /**
     * @param level the level to deactivate
     */
    public void deactivateLevel(short level)
    {
        levels.clear(level);
    }


    /**
     * Deactivate all levels.
     */
    public void deactivateAllLevels()
    {
        levels.clear();
    }


    /**
     * Check if a level is activated.
     *
     * @return true or false
     */
    public boolean isActivated(short level)
    {
        return levels.get(level);
    }


    /**
     * Indicates if this filter is activated or not.
     *
     * @return true is this filter is activated.
     */
    /* ECR-0123
     *public boolean isActive() {
     *    return active;
     *}
     */


    /**
     * Indicates if this filter matches the specified domain.
     *
     * @param domain the domain to be tested.
     *
     * @return true if the domain is matched by this filter.
     */
    /* ECR-0123
     *private boolean matchesDomain(String domain) {
     *    return    (this.domain.compareTo(ALL_DOMAINS.value) == 0)
     *           || (this.domain.compareTo(domain) == 0);
     *}
     */


    /**
     * Indicates if this filter matches the specified level.
     *
     * @param level the level to be tested.
     *
     * @return true if the level is matched by this filter.
     */
    /* ECR-0123
     *private boolean matchesLevel(short level) {
     *    return    (this.level == ALL_VALUES.value)
     *           || (this.level == level);
     *}
     */


    /**
     * Indicates if this filter matches the specified domain and level.
     *
     * @param domain the domain to be tested.
     * @param level the level to be tested.
     *
     * @return true if the domain and the level are matched by this filter.
     */
    /* ECR-0123
     *public boolean matches(String domain, short level) {
     *    return matchesDomain(domain) && matchesLevel(level);
     *}
     */


    /**
     * Indicates if this Filter has common filters with specified filter.
     * If true, it means that one of the filter could override the other for
     * common filters, if the activation are different.
     *
     * @param filter The filter to be compared.
     */
    /* ECR-0123
     *public boolean hasCommonFilterWith(Filter filter) {
     *    
     *    if (   (this.domain.compareTo(ALL_DOMAINS.value) == 0) 
     *        || (filter.domain.compareTo(ALL_DOMAINS.value) == 0) 
     *        || (this.domain.compareTo(filter.domain) == 0)
     *       )
     *    {
     *        // if domain is common, check the level.
     *
     *        if (    (this.level == ALL_VALUES.value)
     *             || (filter.level == ALL_VALUES.value)
     *             || (this.level == filter.level)
     *           )
     *        { 
     *            // if level is common, filters have common filters
     *            return true;
     *        }
     *    } 
     *
     *    return false;
     *}
     */


}
