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

/**
 * This class manages a set of trace filters.
 * These filters are defined as activated or deactivated for.
 * 
 * Features:
 * ThreadSafe.
 *
 */
public class FilterMngr {

    /**
     * List of filters currently defined.
     */
    FilterChain filterChain;

    /**
     * Constructs a new FilterMngr with an empty set of filters.
     */
    public FilterMngr() {
        filterChain = new FilterChain();
    }

    /**
     * Activate the filter for the requested domain and level.
     *
     * @param domain The domain of the filter to be activated.
     * @param level  The level of the filter to be activated.
     */
    public void activateLevel(String domain, short level) {
        filterChain.activateLevel(domain, level);
    }

    /**
     * Deactivate the filter for the requested domain and level.
     *
     * @param domain The domain of the filter to be dectivated.
     * @param level  The level of the filter to be deactivated.
     */
    public void deactivateLevel(String domain, short level) {
        filterChain.deactivateLevel(domain, level);
    }

    /**
     * Indicates if a filter is activated for the requested domain and level.
     *
     * @param domain The domain to be tested.
     * @param level  The level to be tested.
     */
    public boolean isActivated(String domain, short level) {
        return filterChain.isActivated(domain, level);
    }

    /**
     * Returns a CORBA sequence of Trace_Filter representing
     * currently defined filters.
     *
     * @return The CORBA sequence of Trace_Filter
     */
    public com.thalesgroup.CdmwTrace.Trace_Filter[] getTraceFilters() {
        Filter[] filters = filterChain.getFilters();

        com.thalesgroup.CdmwTrace.Trace_Filter[] traceFilters =
            new com.thalesgroup.CdmwTrace.Trace_Filter[filters.length];

        for (int i = 0; i < traceFilters.length; i++) {
            traceFilters[i] = new com.thalesgroup.CdmwTrace.Trace_Filter(
                    filters[i].getLevel(),
                    filters[i].getDomain(),
                    filters[i].isActive());
        }

        return traceFilters;
    }


}

