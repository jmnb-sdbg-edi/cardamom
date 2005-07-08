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


class PolynomeClientThread
    extends java.lang.Thread
{

    private org.omg.CORBA.ORB orb;
    private String applicationName;
    private String processName;
    private com.thalesgroup.PolynomeFactories.PolynomeSessionFactory polynomeFactory;
    private boolean exitRequired;
    
    
    public PolynomeClientThread(org.omg.CORBA.ORB orb,
                                String applicationName,
                                String processName)
    {
        this.orb = orb;
        this.applicationName = applicationName;
        this.processName = processName;
        this.polynomeFactory = null;
        this.exitRequired = false;
      
        try  {      
      
            // ========================================================
            // Get the naming and repository from Repository interface
            // ========================================================
            com.thalesgroup.CdmwNamingAndRepository.Repository repository = null;
                  
            repository = cdmw.namingandrepository.RepositoryInterface.getRepository();


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
            // find the Polynome factory object from repository
            // ======================================================
            com.thalesgroup.CdmwLifeCycle.FactoryBase factory = 
                factoryFinder.find_factory (
                    "Polynome.Domain/Polynome.Factories/PolynomeSession.factory");
                            
            polynomeFactory = com.thalesgroup.PolynomeFactories.PolynomeSessionFactoryHelper.narrow(factory);
    
        } catch (org.omg.CORBA.UserException ex) {
            System.err.println(ex.toString());
            ex.printStackTrace();
        }
    }
   
   
    // Note: stop() operation is final in Thread class.
    //       This stopThread() operation is equivalent 
    //       to the HelloClientThread::stop() C++ operation
    public void stopThread() {
        exitRequired = true;
    }
                             
   
    public void run() {
        System.out.println("Hello Client Thread Started");
        
        boolean userExit = false;
        
        // ====================================================
        // thread loop
        // ====================================================
        try {
            while (!exitRequired) {
                while (!userExit) {
                
                    System.out.println("Request to server a polynome.....");
                     
                    // ======================================================
                    // get the Polynome proxy from the factory
                    // ======================================================
                    com.thalesgroup.PolynomeModule.Polynome polynome = null; 
                            
                    polynome = polynomeFactory.create_Polynome ();         
            
                    if (polynome == null)
                    {
                        System.out.println("could not create Polynome");
                        return;
                    }
    
                    // get value to compare with polynome roots
                    double x = 0;
                    java.io.BufferedReader in = 
                        new java.io.BufferedReader(new java.io.InputStreamReader(System.in));
                    String answer = null;
                    boolean answerOK;
    
                    answerOK = false;
                    do {
                        System.out.println("Enter the value to check");
                        System.out.print("Value x = ");
                        System.out.flush();
                        answer = in.readLine();
                        try {
                            x = Double.parseDouble(answer);
                            answerOK = true;
                            System.out.println();
                        } catch (NumberFormatException e) {
                            System.out.println("This is not a number !\n");
                        }
                    } while (!answerOK);
            
                    System.out.println();
                    
                                    
                    // ===================================================
                    // compute the roots, print and check
                    // ===================================================
                    polynome.computeRoots();
                    
                    polynome.printRoots();
                    
                    boolean check = polynome.isInBracket(x);
                    
                    if (check)
                    {
                        System.out.println("Result is TRUE");
                    }
                    else
                    {
                        System.out.println("Result is FALSE");
                    }
                    
                    System.out.println();
            
                    // ===================================================
                    // Remove Polynome
                    // ===================================================
                    polynome.remove();
                    
                    
                    // ===================================================
                    // User exit request
                    // ===================================================
                    String str = "y";
                    System.out.println("Request to exit (y/n) ? ");
                    str = in.readLine();
                    System.out.println();
                    
                    if (str.equals("y")) {
                        userExit = true;
                        
                        System.out.println("Stop process using supervision");
                    }

                } // end of while (!userExit)
                
            } // end of while (!exitRequired)

        } catch (Exception e) {
            System.out.println("Unexpected exception in thread loop");
            e.printStackTrace();
        }
    }
       
}
   
    




