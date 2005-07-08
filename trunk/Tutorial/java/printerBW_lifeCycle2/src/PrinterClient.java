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


package cdmw.tutorial;


public class PrinterClient {

    private static final int POA_THREAD_POOL_SIZE = 5;
    
    
    public static void main(String[] args) {
        
        System.out.println("Start Printer Client");
        
        org.omg.CORBA.ORB orb = null;              // orb reference
        
        try  {
            // ===================================================
            // Initialize the ORB
            // ===================================================        
            cdmw.orbsupport.StrategyList orbStrategies = 
                new cdmw.orbsupport.StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
        
            orb = cdmw.orbsupport.ORBSupport.ORBInit(args, orbStrategies);

            // ===================================================
            // Get the root POA 
            // ===================================================
            org.omg.CORBA.Object obj = orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA rootPOA = 
                org.omg.PortableServer.POAHelper.narrow(obj);
            
            // ===================================================
            // Activate the root POA manager
            // ===================================================
            org.omg.PortableServer.POAManager poaManager = rootPOA.the_POAManager();
            poaManager.activate();

            
            // ===================================================
            // Call generated cdmwinit
            // ===================================================
            cdmw.generated.PrinterClientCdmwInterface.cdmwInit(orb, args, null);
            
            
            // ===================================================
            // Get the process name and application name
            // ===================================================
            String processName = cdmw.cdmwinit.InitUtils.getCdmwProcessName();
            
            String applicationName = cdmw.cdmwinit.InitUtils.getCdmwApplicationName();
            
            
            // ========================================================
            // Get the naming and repository from Repository interface
            // ========================================================
            com.thalesgroup.CdmwNamingAndRepository.Repository repository =
                cdmw.namingandrepository.RepositoryInterface.getRepository();
        
        
            // ========================================================
            // Get the Default Root context from Repository
            // ========================================================
            org.omg.CORBA.Object obj2 = 
                repository.resolve_root_context (
                    com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);
            org.omg.CosNaming.NamingContextExt rootCtx = 
                org.omg.CosNaming.NamingContextExtHelper.narrow(obj2);
                                   
                                   
            // ========================================================
            // Get the Factory Finder from Default Root Context
            // ========================================================
            org.omg.CORBA.Object obj3 = rootCtx.resolve_str (
                com.thalesgroup.CdmwNamingAndRepository.FACTORY_FINDER.value);
            com.thalesgroup.CdmwLifeCycle.FactoryFinder factoryFinder = 
                com.thalesgroup.CdmwLifeCycle.FactoryFinderHelper.narrow(obj3);
                                       
             
            // ======================================================
            // find the Printer factory object from repository
            // ======================================================
            com.thalesgroup.CdmwLifeCycle.FactoryBase factory = 
                factoryFinder.find_factory (
                    "Printer.Domain/Printer.Factories/PrinterSession.factory");
                            
            com.thalesgroup.PrinterFactories.PrinterSessionFactory printerFactory = 
                 com.thalesgroup.PrinterFactories.PrinterSessionFactoryHelper.narrow(factory);
                                   
                                   
                            
            // ===================================================
            // get the Printer proxy from the factory
            // ===================================================
            com.thalesgroup.PrinterModule.Printer printer = 
                printerFactory.create_printer ("lpr10");      
    
            if (printer == null)
            {
                System.out.println ("could not narrow object to type Printer");
                return;
            }            
            
            
            System.out.println("Printer Client Started");
            
            String baseMsg = "process <" + applicationName + "/" + processName + ">" ;
            
            

            // ===================================================
            // Call print_message method of Printer object
            // ===================================================
            printer.print_message(">>>>> HELLO <<<<<");


            // ===================================================
            // Remove Printer
            // ===================================================
            printer.remove();


            // ===================================================
            // wait for CORBA message
            // ===================================================
            orb.run();            
            
        } catch  (cdmw.common.Exception ex) {
            System.err.println(ex.what());
            ex.printStackTrace();
            
        } catch (org.omg.CORBA.SystemException ex) {
            System.err.println(ex.toString());
            ex.printStackTrace();

        } catch (org.omg.CORBA.UserException ex) {
            System.err.println(ex.toString());
            ex.printStackTrace();
        }

        
        // ========================================================
        // program stopping
        // ========================================================
        if (orb != null) {
            try {
                // ================================================================
                // Call generated Cdmw cleanup
                // ================================================================
                cdmw.generated.PrinterClientCdmwInterface.cdmwCleanup(orb);
                            
                // =====================================================
                // Destroy orb
                // =====================================================
                orb.destroy();

            } catch (org.omg.CORBA.SystemException ex) {
                System.out.println(ex.toString());
                ex.printStackTrace();
            }
        }
    
    }
    
}
