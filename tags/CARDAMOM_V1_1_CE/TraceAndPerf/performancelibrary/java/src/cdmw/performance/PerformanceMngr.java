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


package cdmw.performance;

/**
 * This class is the central class of the Cdmw Performance service. This
 * class is used as a singleton to provide access to the performance system
 * from everywhere. The PerformanceMngr is in charge of:
 * <li> allocate and initialize the Probe Collector managing the api performance 
 *      statistics
 * <li> initialised the CORBA object in charge of handling client Performance activation
 *      requests (PerformanceAdmin).
 *
 * @see  PerformanceAdminImpl
 * @see  ProbeCollector
 */
public class PerformanceMngr {

    /**
     * This variable is set to true, since the init() function 
     * has been called.
     * The cleanup() function reset this variable to false.
     */
    private static boolean isInitDone = false;
    
    /**
     * Reference to the Probe Collector.
     */
    private static ProbeCollector probeCollector = null;
    
    /**
     * Reference to Performance Admin servant.
     */
    private static com.thalesgroup.CdmwPerformanceAdmin.
        PerformanceAdmin performanceAdmin = null;
        
    /**
     * Contains name of the Performance log file.
     */
    private static String perfoLogFile = "";
    
    
    
    
    /**
     * Class constructor.
     */
    protected PerformanceMngr() {
    }
    
    
    
    /**
     * Initialize the Performance library. This function must be called
     * before any other call. The Performance service must be closed
     * by a call to cleanup().
     * 
     * @param perfomancePOA  A POA ptr the performance admin servant is registered
     *       (POA Policy must be set to RETAIN and SYSTEM_ID)
     * @param perfomanceFile  A string to indicate the Performance file name to log
     *       performance statistics from Probe Collector
     *       
     * @exception BadParameterException If POA policy is wrong.
     *
     * @see PerformanceMngr#cleanup
     */    
    public static com.thalesgroup.CdmwPerformanceAdmin.PerformanceAdmin
    init(org.omg.PortableServer.POA performancePOA, String performanceFile)
    throws cdmw.common.BadParameterException
    {
        // The user try to initialise twice the performance library
        cdmw.common.Assert.check(!isInitDone);
        
        // We create the Probe Collector object 
        // in charge of recording the API performance
        probeCollector = ProbeCollector.getInstance();
        
        // Register Performance log file name
        perfoLogFile = performanceFile;
        
        // We must create the Servant to handle performance activation
        PerformanceAdminImpl servant = new PerformanceAdminImpl();
        
        try {
            // activate servant on POA
            byte[] oid = performancePOA.activate_object(servant);
            org.omg.CORBA.Object object = performancePOA.id_to_reference(oid);
            performanceAdmin = com.thalesgroup.
                CdmwPerformanceAdmin.PerformanceAdminHelper.narrow(object);

        } catch (org.omg.PortableServer.POAPackage.WrongPolicy e) {
            throw new cdmw.common.BadParameterException(
                "Wrong POA Policy for activate_object",
                "NULL");
        } catch (org.omg.CORBA.UserException e) {
            System.err.println("Performance Admin servant activation error");
            System.err.println(e.toString());
            cdmw.common.Assert.assertionFailed();
        } catch (org.omg.CORBA.SystemException e) {
            System.err.println("Performance Admin servant activation error");
            System.err.println(e.toString());
            cdmw.common.Assert.assertionFailed();
        }
            
        isInitDone = true;
         
        // We return the reference on the object managing communication with 
        // collector, it will be registered in the repository by the caller
        return performanceAdmin;
    }
    
            
    /**
     * Free all allocated memory, stop the performance service and write
     * statistics into performanceFile (given at init).
     *
     * @exception BadParameterException If performanceFile given at init()
     *                                 cannot be opened.
     *
     * @see PerformanceMngr#init
     */
    public static void cleanup()
    throws cdmw.common.BadParameterException {
        if (isInitDone) {
            isInitDone = false;
            
            // write statistics recorded by Probe Collector in log file
            probeCollector.writeStatistic(perfoLogFile);    
        }
    }

    //
    // Activate the performance measurements
    //
    public static void activate()
        throws org.omg.CORBA.SystemException
    {
        if (!isInitDone) {
            throw new org.omg.CORBA.BAD_INV_ORDER(
                cdmw.orbsupport.ExceptionMinorCodes.BAD_INV_ORDERInitNotCalled,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        probeCollector.setActivity(true);
    }
    
    
    //
    // Deactivate the performance measurements
    //
    public static void deactivate()
        throws org.omg.CORBA.SystemException
    {
        if (!isInitDone) {
            throw new org.omg.CORBA.BAD_INV_ORDER(
                cdmw.orbsupport.ExceptionMinorCodes.BAD_INV_ORDERInitNotCalled,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        probeCollector.setActivity(false);
    }
    
    
    //
    // Write the performance measurements into performance file
    //
   public static  void writeStatistic()
        throws org.omg.CORBA.SystemException,
               cdmw.common.BadParameterException
    {
        if (!isInitDone) {
            throw new org.omg.CORBA.BAD_INV_ORDER(
                cdmw.orbsupport.ExceptionMinorCodes.BAD_INV_ORDERInitNotCalled,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        probeCollector.writeStatistic(perfoLogFile);
    }


}


