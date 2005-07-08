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
 * This class contains all information defining a filter.
 * A filter is a combination of a user level and a domain.
 * It could be activated or disabled.
 * If a Level/Domain has never been activated, it is considered as disabled.
 *
 */
class Filter {

    /**
     * The domain of this filter.
     */
    private String domain;

    /**
     * The level of this filter.
     */ 
    private short level;

    /**
     * Indicate if the filter is activated or not.
     */
    private boolean active;


    /**
     * Constructs a new filter, activated or not, 
     * for specified domain and level.
     */
    public Filter(String domain,
                  short level,
                  boolean active)
    {
        this.domain = domain;
        this.level = level;
        this.active = active;
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
    public short getLevel() {
        return level;
    }

    /**
     * Indicates if this filter is activated or not.
     *
     * @return true is this filter is activated.
     */
    public boolean isActive() {
        return active;
    }

    /**
     * Indicates if this filter matches the specified domain.
     *
     * @param domain the domain to be tested.
     *
     * @return true if the domain is matched by this filter.
     */
    private boolean matchesDomain(String domain) {
        return    (this.domain.compareTo(ALL_DOMAINS.value) == 0)
               || (this.domain.compareTo(domain) == 0);
    }

    /**
     * Indicates if this filter matches the specified level.
     *
     * @param level the level to be tested.
     *
     * @return true if the level is matched by this filter.
     */
    private boolean matchesLevel(short level) {
        return    (this.level == ALL_VALUES.value)
               || (this.level == level);
    }

    /**
     * Indicates if this filter matches the specified domain and level.
     *
     * @param domain the domain to be tested.
     * @param level the level to be tested.
     *
     * @return true if the domain and the level are matched by this filter.
     */
    public boolean matches(String domain, short level) {
        return matchesDomain(domain) && matchesLevel(level);
    }


    /**
     * Indicates if this Filter has common filters with specified filter.
     * If true, it means that one of the filter could override the other for
     * common filters, if the activation are different.
     *
     * @param filter The filter to be compared.
     */
    public boolean hasCommonFilterWith(Filter filter) {
        
        if (   (this.domain.compareTo(ALL_DOMAINS.value) == 0) 
            || (filter.domain.compareTo(ALL_DOMAINS.value) == 0) 
            || (this.domain.compareTo(filter.domain) == 0)
           )
        {
            // if domain is common, check the level.

            if (    (this.level == ALL_VALUES.value)
                 || (filter.level == ALL_VALUES.value)
                 || (this.level == filter.level)
               )
            { 
                // if level is common, filters have common filters
                return true;
            }
        } 

        return false;
    }


}




