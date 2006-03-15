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
 * This class manages a set of trace filters.
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
        filterChain.activateLevel(
                ALL_COMPONENT_NAMES.value, // ECR-0123
                domain,
                level);
    }


    /**
     * Activate the filter for the requested domain and level.
     *
     * @param componentName The component name.
     * @param domain The domain of the filter to be activated.
     * @param level  The level of the filter to be activated.
     */
    // ECR-0123
    public void activateLevel(String componentName,
                              String domain,
                              short level)
    {
        filterChain.activateLevel(componentName, domain, level);
    }


    /**
     * Deactivate the filter for the requested domain and level.
     *
     * @param domain The domain of the filter to be dectivated.
     * @param level  The level of the filter to be deactivated.
     */
    public void deactivateLevel(String domain, short level) {
        filterChain.deactivateLevel(
                ALL_COMPONENT_NAMES.value, // ECR-0123
                domain,
                level);
    }


    /**
     * Deactivate the filter for the requested domain and level.
     *
     * @param componentName The component name.
     * @param domain The domain of the filter to be dectivated.
     * @param level  The level of the filter to be deactivated.
     */
    // ECR-0123
    public void deactivateLevel(String componentName,
                                String domain,
                                short level)
    {
        filterChain.deactivateLevel(componentName, domain, level);
    }


    /**
     * Indicates if a filter is activated for the requested domain and level.
     *
     * @param domain The domain to be tested.
     * @param level  The level to be tested.
     */
    public boolean isActivated(String domain, short level) {
        return filterChain.isActivated(
                ALL_COMPONENT_NAMES.value, // ECR-0123
                domain,
                level);
    }


    /**
     * Indicates if a filter is activated for the requested domain and level.
     *
     * @param componentName The component name.
     * @param domain The domain to be tested.
     * @param level  The level to be tested.
     */
    // ECR-0123
    public boolean isActivated(String componentName,
                               String domain,
                               short level)
    {
        return filterChain.isActivated(componentName, domain, level);
    }


    /**
     * Returns a CORBA sequence of Trace_Filter representing
     * currently defined filters.
     *
     * @return The CORBA sequence of Trace_Filter
     */
    public com.thalesgroup.CdmwTrace.Trace_Filter[] getTraceFilters() {
        Filter[] filters = filterChain.getFilters();

        /*
         *com.thalesgroup.CdmwTrace.Trace_Filter[] traceFilters =
         *    new com.thalesgroup.CdmwTrace.Trace_Filter[filters.length];
         *
         *for (int i = 0; i < traceFilters.length; i++) {
         *    traceFilters[i] = new com.thalesgroup.CdmwTrace.Trace_Filter(
         *            filters[i].getLevel(),
         *            filters[i].getDomain(),
         *            filters[i].isActive());
         *}
         */

        // reminder: filters[0] is the most generic filter (C*, D*, mask)

        BitSet mask_filter_any = filters[0].getLevels();

        // the most generic filter is considered activated
        // if at least half of the bits in the mask is set (bit = 1)
        boolean filter_any_is_activated =
            (mask_filter_any.cardinality() >= (Filter.NB_BITS / 2));

        // set the length of the sequence of filters
        int seqLen = filters.length + 1;
        if (filter_any_is_activated) {
            seqLen += Filter.NB_BITS - mask_filter_any.cardinality();
        }
        else {
            seqLen += mask_filter_any.cardinality();
        }

        // create a sequence of filters
        com.thalesgroup.CdmwTrace.Trace_Filter[] traceFilters =
            new com.thalesgroup.CdmwTrace.Trace_Filter[seqLen];

        // add the most generic filter first
        traceFilters[0] = new com.thalesgroup.CdmwTrace.Trace_Filter(
                            ALL_VALUES.value,
                            ALL_DOMAINS.value,
                            ALL_COMPONENT_NAMES.value,
                            filter_any_is_activated);

        // then add the generic filters than are deduced from mask_filter_any
        int seq_idx = 1;
        if ((mask_filter_any.cardinality() != 0) &&
            (mask_filter_any.cardinality() != Filter.NB_BITS))
        {
            for (short bit = 0; bit < Filter.NB_BITS; bit++) {
                if (filter_any_is_activated && !mask_filter_any.get(bit)) {
                    traceFilters[seq_idx] =
                        new com.thalesgroup.CdmwTrace.Trace_Filter(
                                bit,
                                ALL_DOMAINS.value,
                                ALL_COMPONENT_NAMES.value,
                                false);
                    seq_idx++;
                }
                else if (!filter_any_is_activated && mask_filter_any.get(bit)) {
                    traceFilters[seq_idx] =
                        new com.thalesgroup.CdmwTrace.Trace_Filter(
                                bit,
                                ALL_DOMAINS.value,
                                ALL_COMPONENT_NAMES.value,
                                true);
                    seq_idx++;
                }
            }
        }

        // now add the filters from the list (if any)
        for (int i = 1; i < filters.length; i++) {
            BitSet mask = filters[i].getLevels();

            if (mask != mask_filter_any) {
                // the filter is considered activated
                // if at least half of the bits in the mask is set
                boolean filter_is_activated =
                    (mask.cardinality() >= (Filter.NB_BITS / 2));

                if (filter_is_activated != filter_any_is_activated) {
                    traceFilters[seq_idx] =
                        new com.thalesgroup.CdmwTrace.Trace_Filter(
                                ALL_VALUES.value,
                                filters[i].getDomain(),
                                filters[i].getComponentName(),
                                filter_is_activated);
                    seq_idx++;
                }

                if ((mask.cardinality() != 0) &&
                    (mask.cardinality() != Filter.NB_BITS))
                {
                    for (short bit = 0; bit < Filter.NB_BITS; bit++) {
                        if ((mask.get(bit) != mask_filter_any.get(bit)) ||
                            ((mask.get(bit) == mask_filter_any.get(bit)) &&
                             (filter_is_activated != filter_any_is_activated)))
                        {
                            if (filter_is_activated && !mask.get(bit)) {
                                traceFilters[seq_idx] =
                                    new com.thalesgroup.CdmwTrace.Trace_Filter(
                                            bit,
                                            filters[i].getDomain(),
                                            filters[i].getComponentName(),
                                            false);
                                seq_idx++;
                            }
                            else if (!filter_is_activated && mask.get(bit)) {
                                traceFilters[seq_idx] =
                                    new com.thalesgroup.CdmwTrace.Trace_Filter(
                                            bit,
                                            filters[i].getDomain(),
                                            filters[i].getComponentName(),
                                            true);
                                seq_idx++;
                            }
                        }
                    }
                }
            }
        }

        return traceFilters;
    }


}

