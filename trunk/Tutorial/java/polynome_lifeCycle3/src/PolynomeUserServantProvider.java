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


class PolynomeUserServantProvider 
    extends cdmw.tutorial.PolynomeServantProvider 
{

    private static final String POLYNOME_REP_ID = com.thalesgroup.PolynomeModule.PolynomeHelper.id();

    
    private org.omg.CORBA.ORB orb;
    private org.omg.PortableServer.POA poa;
    private cdmw.lifecycle.ObjectDeactivator objectDeactivator;

    private double counter;
    private double a;
    private double b;
    private double c;
    
    
    
    public PolynomeUserServantProvider (org.omg.CORBA.ORB orb,
                                        org.omg.PortableServer.POA poa,
                                        cdmw.lifecycle.ObjectDeactivator deactivator)
    {
        this.orb = orb;
        this.poa = poa;
        this.objectDeactivator = deactivator;
        this.counter = 0;
        this.a = 0;
        this.b = 0;
        this.c = 0;
    }


    /**
    * This method acts as a hand shaking between the Servant Provider
    * and the generated Factory implementation. It indicates whether
    * a Servant provided by this Servant Provider may incarnate multiple
    * objects (with ObjectIds provided by this Servant Provider)
    *
    * @return <B>true</B> if a servant may incarnate multiple objects,
    * <B>false</B> otherwise.
    */
    public boolean mayIncarnateMultipleObjects() {
        return false;
    }



    /**
    * Purpose:
    * <p> This method creates the oid of the HelloInterface object
    *
    *@param  object_oid string oid of object
    *
    *@return object id
    */
    public byte[] create_Polynome()
        throws org.omg.CORBA.SystemException
    {
        
        // 
        // get the equation coefs
        //
        try {
            
            java.io.BufferedReader in = 
                new java.io.BufferedReader(new java.io.InputStreamReader(System.in));
            String answer = null;
            boolean answerOK;
            
            System.out.println("Enter the coefs of polynome ax*x + bx + c = 0");
            
            do {
                System.out.print("Coef a = ");
                System.out.flush();
                answer = in.readLine();
                try {
                    a = Double.parseDouble(answer);
                    if (a == 0) { 
                        System.out.println(">>>>> Coef a cannot be null");
                    }
                    System.out.println();
                } catch (NumberFormatException e) {
                    System.out.println("This is not a number !\n");
                    a = 0;
                }
            } while (a == 0);
            
            answerOK = false;
            do {
                System.out.print("Coef b = ");
                System.out.flush();
                answer = in.readLine();
                try {
                    b = Double.parseDouble(answer);
                    answerOK = true;
                    System.out.println();
                } catch (NumberFormatException e) {
                    System.out.println("This is not a number !\n");
                }
            } while (!answerOK);
            
            answerOK = false;
            do {
                System.out.print("Coef c = ");
                System.out.flush();
                answer = in.readLine();
                try {
                    c = Double.parseDouble(answer);
                    answerOK = true;
                    System.out.println();
                } catch (NumberFormatException e) {
                    System.out.println("This is not a number !\n");
                }
            } while (!answerOK);
         
        } catch (java.io.IOException e) {
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }
        
        //
        // create an unique oid
        //
        double oidCounter;
        synchronized (this) {
            if (counter == Double.MAX_VALUE) {
                counter = 0;
            } else {
                counter++;
            }
            
            oidCounter = counter;
        }
        
        cdmw.ossupport.Timeval timeval  = cdmw.ossupport.OS.getTime();
        String polynome_oid = "POLY_" + oidCounter + "_" + timeval.getSeconds();
        
        byte[] oid = polynome_oid.getBytes();
        
        System.out.println("<<PolynomeUserServantProvider>> Polynome '" +
                           polynome_oid + "' OID created");
        
        return oid;
    }


    /**
    * Purpose:
    * <p> This method creates the HelloInterface servant associated to
    *     the oid
    *
    *@return servant object
    */    
    public org.omg.PortableServer.Servant lookupServant(byte[] oid) 
        throws org.omg.PortableServer.ForwardRequest 
    {
        com.thalesgroup.PolynomeModule.PolynomeOperations delegate;
        
        //
        // Create implementation object 
        //
        
        // compute the discriminant Delta = b*b - 4*a*c
        double delta = b*b - 4*a*c;
        
        // if delta is negative : no roots
        if (delta < 0)
        {
            NoRootsBinomeImpl binome = new NoRootsBinomeImpl (objectDeactivator);
                
            delegate = binome;
        
            System.out.println("<<PolynomeUserServantProvider>> NoRootsBinomeImpl servant created");
        }
        
        // else if delta is zero : only one root
        else if (delta == 0)
        {
            SingleRootBinomeImpl binome = 
                 new SingleRootBinomeImpl (objectDeactivator, a, b);
                
            delegate = binome;
        
            System.out.println("<<PolynomeUserServantProvider>> SingleRootBinomeImpl servant created");
        }
        
        // else delta is positive : two roots
        else
        {
            TwoRootsBinomeImpl binome = 
                  new TwoRootsBinomeImpl (objectDeactivator, a, b, delta);
                          
            delegate = binome;
        
            System.out.println("<<PolynomeUserServantProvider>> TwoRootsBinome_impl servant created");
        }        

        // NOTE: The servant is a TIE (see note into Impls)
        org.omg.PortableServer.Servant servant = 
            new com.thalesgroup.PolynomeModule.PolynomePOATie(delegate, poa);
        
        // return the servant reference of object
        return servant;
    }


    public void releaseServant(byte[] oid,
                               org.omg.PortableServer.POA poa,
                               org.omg.PortableServer.Servant servant) 
    {
        System.out.println("<<PolynomeUserServantProvider>> Polynome servant reference removed.");
    }


    public String getSupportedInterface(byte[] oid)
    {
        return POLYNOME_REP_ID;    
    }


        
}

