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


package cdmw.demoperf;

import cdmw.cdmwinit.ProcessControl;
import cdmw.cdmwinit.CdmwInterface;
import cdmw.namingandrepository.RepositoryInterface;
import com.thalesgroup.CdmwNamingAndRepository.Repository;

public class ClientProcessControl extends ProcessControl {

    private org.omg.CORBA.ORB orb;
    
    private String applicationName;
    private String processName;
    
    private IImpl iImpl;
    
    private BenchmarkThread benchThread;
    private com.thalesgroup.Benchmark.Tester tester;
    
    
    
    /**
     * The constructor.
     *
     */ 
    public ClientProcessControl(org.omg.CORBA.ORB orb) {
        this.orb = orb;
        this.applicationName = "";
        this.processName = "";
        iImpl = null;
        benchThread = null;
        tester = null;        
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    public void onInitialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {
        
        try {
            // ===================================================
            // Get the process name and application name
            // ===================================================
            processName = cdmw.platformmngt.PlatformInterface.getProcessName();
            applicationName = cdmw.platformmngt.PlatformInterface.getApplicationName();
    
            // ==============================================================
            // example of using the PlatformInterface for notifying a message
            // ==============================================================
            cdmw.platformmngt.PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                processName, 
                ">>>>>>>>>>>>>> Initialisation requested by supervision");
                                         
        } catch (cdmw.common.BadParameterException e) {
            // This exception shouldn't append if notify level is not CdmwPlatformMngtBase.FTL,
            // if notify issuer is not null or CdmwPlatformMngtBase.PFMMNG, 
            // and if notify message is not null.
            e.printStackTrace();
            cdmw.common.Assert.check(false);
            
        } catch (cdmw.common.BadOrderException e) {
            // This exception shouldn't append if creation  
            // acknowledgement has been performed
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }

        // ===============================
        // create I and Tester objects
        // ===============================
        iImpl = new IImpl();
        com.thalesgroup.Benchmark.I i = iImpl._this(orb);
        
        // =========================================
        // retrieve Tester object from Repository
        // =========================================
        String registration_name = "Tester";
        try {
            // get NamingInterface to demo_perf
            cdmw.namingandrepository.NamingInterface demoPerfNamingInterface =
                RepositoryInterface.getDomainNamingInterface ("demo_perf");
        
             // Retrieve Tester from NamingInterface
             org.omg.CORBA.Object obj = demoPerfNamingInterface.resolve (registration_name);
             tester = com.thalesgroup.Benchmark.TesterHelper.narrow(obj);
         
         }
         catch(com.thalesgroup.CdmwNamingAndRepository.NoNameDomain e)
         {
             System.err.println("'demo_perf' is not a NameDomain !");
         }
         catch(com.thalesgroup.CdmwNamingAndRepository.InvalidName e)
         {
             System.err.println("'demo_perf' is not a valid name !");
         }
         catch(org.omg.CosNaming.NamingContextPackage.NotFound e)
         {
             System.err.println("Resolve " + registration_name +
                                " => NamingContext::NotFound raised!");
         }
         catch(org.omg.CosNaming.NamingContextPackage.CannotProceed e)
         {
             System.err.println("Resolve " + registration_name +
                                " => NamingContext::CannotProceed raised!");
         }
         catch(org.omg.CosNaming.NamingContextPackage.InvalidName e)
         {
             System.err.println("Resolve " + registration_name +
                                " => NamingContext::InvalidName raised!");
         }
         catch (org.omg.CORBA.SystemException e)
         {
             System.err.println("Resolve " + registration_name +
                                " => CORBA::SystemException raised!");
         }                
    
        // ============================================
        // Register Value valuetype factory into ORB
        // ============================================
        org.omg.CORBA.portable.ValueFactory factory = new ValueFactoryImpl();
        org.omg.CORBA.portable.ValueFactory oldFactory =
            oldFactory = ((org.omg.CORBA_2_3.ORB) orb).register_value_factory(
                        "IDL:thalesgroup.com/Benchmark/Value:1.0",
                        factory);
    
        // =======================================
        // Activate the performance measurement
        // =======================================
        try {
            cdmw.performance.PerformanceMngr.activate();
        }
        catch (org.omg.CORBA.SystemException e) 
        {
            // Shoudn't happen: PerformanceMngr::init() has been 
            //                  called by CDMW_init()
            cdmw.common.Assert.check(false);
        }
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
     * operation
     */
    public void onNextStep()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep {

    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    public void onRun()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun {

        try {
            // =================================================================
            // example of using the PlatformInterface for notifying a message
            // =================================================================
            cdmw.platformmngt.PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                processName, 
                ">>>>>>>>>>>>>> Run requested by supervision");
    
        } catch (cdmw.common.BadParameterException e) {
            // This exception shouldn't append if notify level is not CdmwPlatformMngtBase.FTL,
            // if notify issuer is not null or CdmwPlatformMngtBase.PFMMNG, 
            // and if notify message is not null.
            e.printStackTrace();
            cdmw.common.Assert.check(false);
            
        } catch (cdmw.common.BadOrderException e) {
            // This exception shouldn't append if creation  
            // acknowledgement has been performed
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }

        // ======================
        // run Benchmark thread
        // ======================
        com.thalesgroup.Benchmark.I i = iImpl._this();
        benchThread = new BenchmarkThread(tester, i, orb);
    
        benchThread.start();
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    public void onStop() {

        try {
            // =================================================================
            // example of using the PlatformInterface for notifying a message
            // =================================================================
            cdmw.platformmngt.PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                processName, 
                ">>>>>>>>>>>>>> Stop requested by supervision");
 
        } catch (cdmw.common.BadParameterException e) {
            // This exception shouldn't append if notify level is not CdmwPlatformMngtBase.FTL,
            // if notify issuer is not null or CdmwPlatformMngtBase.PFMMNG, 
            // and if notify message is not null.
            e.printStackTrace();
            cdmw.common.Assert.check(false);
            
        } catch (cdmw.common.BadOrderException e) {
            // This exception shouldn't append if creation  
            // acknowledgement has been performed
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }

        try {
            benchThread.stop();
            benchThread.join();
            
            orb.shutdown(false);
        } catch (InterruptedException ie) {}
    }

}

