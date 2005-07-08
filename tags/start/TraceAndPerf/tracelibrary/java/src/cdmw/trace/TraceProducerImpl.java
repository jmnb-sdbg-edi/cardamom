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
 * This class implements the TraceProducer interface. This
 * class allows collector to register to receive message
 * and register the process into the Repository
 * 
 * Features:
 * Thread safe
 */
public class TraceProducerImpl extends com.thalesgroup.CdmwTrace.TraceProducerPOA {

    /**
     * The area flusher in charge sending information to collector
     */
    private AreaFlusher areaFlusher;

    /**
     * Contains the reference on the object in charge of
     * informing the trace system, if an information, from
     * a domain must be traced or not
     */
    private FilterMngr filterMngr;




    /**
     * Constructs a TraceProducer servant with references to an AreaFlusher 
     * and a FilterMngr
     *
     * @param areaFlusher  In charge of flushing data to Collectors
     * @param filterMngr   FilterMngr in charge of determining if a Level/Domain
     *                    must be trace or not.
     */
    public TraceProducerImpl(AreaFlusher areaFlusher, FilterMngr filterMngr) {
        super();
        this.areaFlusher = areaFlusher;
        this.filterMngr = filterMngr;
    }




    //
    // IDL:thalesgroup.com/CdmwTrace/TraceProducer/register_collector:1.0
    //
    /**
     * This operation allows a Collector to be registered
     * within the TraceProducer.
     *
     * @param the_collector The collector to be registered
     * @param the_objName   The name of the collector object
     * @param the_mnemoName The mnemonic name that may be used as a help
     *                      to identify the collector.
     *
     * @return The actual registration identifier of the collector.
     *
     * @exception com.thalesgroup.CdmwTrace.TraceProducerPackage.InvalidRegistration if
     * collectorId cannot be allocated
     */
    public int register_collector(com.thalesgroup.CdmwTrace.Collector the_collector,
                                  String the_objName,
                                  String the_mnemoName)
        throws com.thalesgroup.CdmwTrace.TraceProducerPackage.InvalidRegistration
    {
        try {
            return areaFlusher.registerCollector(the_collector,
                                                the_objName,
                                                the_mnemoName);
        } catch (OutOfMemoryError e) {
            throw new org.omg.CORBA.NO_RESOURCES();
        }
    }



    //
    // IDL:thalesgroup.com/CdmwTrace/TraceProducer/unregister_collector:1.0
    //
    /**
     * This operation unregisters a collector identified by
     * its registration identifier.
     *
     * @param collectorId The actual registration identifier of the collector
     *                    to be unregistered.
     *
     * @exception com.thalesgroup.CdmwTrace.TraceProducerPackage.InvalidRegistration
     * if collectorId doesn't denote a valid registration
     */
    public void unregister_collector(int collectorId)
        throws com.thalesgroup.CdmwTrace.TraceProducerPackage.InvalidRegistration
    {
        areaFlusher.unregisterCollector(collectorId);
    }



    //
    //IDL:thalesgroup.com/CdmwTrace/TraceProducer/get_registered_collectors:1.0
    //
    /**
     * This operation returns the registered collectors.
     *
     * @return a CORBA sequence of registrations
     */
    public com.thalesgroup.CdmwTrace.TraceProducerPackage.Registration[]
    get_registered_collectors()
    {
        try {
            return areaFlusher.getRegisteredCollectors();
        } catch (OutOfMemoryError e) {
            throw new org.omg.CORBA.NO_RESOURCES();
        }
    }


    //
    // IDL:thalesgroup.com/CdmwTrace/TraceProducer/get_trace_filters:1.0
    //
    /**
     * This operation returns the current trace filters.
     *
     * @return a CORBA sequence of Trace_Filter
     */
    public com.thalesgroup.CdmwTrace.Trace_Filter[] get_trace_filters() {
        try {
            return filterMngr.getTraceFilters();
        } catch (OutOfMemoryError e) {
            throw new org.omg.CORBA.NO_RESOURCES();
        }
    }



    //
    // IDL:thalesgroup.com/CdmwTrace/TraceProducer/activate_level:1.0
    //
    /**
     * This operation activates the trace level specified
     * by a domain and a value.
     *
     * @param the_domain The domain
     * @param the_value  The value
     */
    public void activate_level(String the_domain, short the_value) {
        filterMngr.activateLevel(the_domain, the_value);
    }



    //
    // IDL:thalesgroup.com/CdmwTrace/TraceProducer/deactivate_level:1.0
    //
    /**
     * This operation deactivates the trace level specified
     * by a domain and a value.
     *
     * @param the_domain The domain
     * @param the_value  The value
     */
    public void deactivate_level(String the_domain, short the_value) {
        filterMngr.deactivateLevel(the_domain, the_value);
    }


}

