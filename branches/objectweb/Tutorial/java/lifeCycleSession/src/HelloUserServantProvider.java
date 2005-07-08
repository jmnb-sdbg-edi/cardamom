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


class HelloUserServantProvider 
    extends cdmw.tutorial.HelloServantProvider 
{

    private static final String HELLOINTERFACE_REP_ID = com.thalesgroup.Hello.HelloInterfaceHelper.id();

    
    private org.omg.CORBA.ORB orb;
    private org.omg.PortableServer.POA poa;
    private cdmw.lifecycle.ObjectDeactivator objectDeactivator;
    private long counter;


    public HelloUserServantProvider (org.omg.CORBA.ORB orb,
                                     org.omg.PortableServer.POA poa,
                                     cdmw.lifecycle.ObjectDeactivator deactivator)
    {
        this.orb = orb;
        this.poa = poa;
        this.objectDeactivator = deactivator;
        this.counter = 0;        
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
    public byte[] create_HelloInterface()
        throws org.omg.CORBA.SystemException
    {
        //
        // create an unique id
        //
        long oidCounter;
        synchronized(this) {
            if (counter == Double.MAX_VALUE) {
                counter = 0;
            } else {
                counter++;
            }
            
            oidCounter = counter;
        }
 
        cdmw.ossupport.Timeval timeval = cdmw.ossupport.OS.getTime();
        
        String object_oid = "Hello_" + oidCounter + "_" + timeval.getSeconds(); 
        
        byte[] oid = object_oid.getBytes();
        
        System.out.println("<<PrinterUserServantProvider>> Printer '" +
                           object_oid + "' OID created");
        
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
        //
        // Create implementation object 
        //
        HelloInterfaceImpl helloImpl = new HelloInterfaceImpl(objectDeactivator);
        
        // NOTE: The servant is a TIE (see note into HelloInterfaceImpl)
        org.omg.PortableServer.Servant servant = 
            new com.thalesgroup.Hello.HelloInterfacePOATie(helloImpl, poa);
        
        System.out.println("<<HelloUserServantProvider>> HelloInterface servant created");
        
        // return the servant reference of object
        return servant;
    }


    public void releaseServant(byte[] oid,
                               org.omg.PortableServer.POA poa,
                               org.omg.PortableServer.Servant servant) 
    {
        System.out.println("<<HelloUserServantProvider>> HelloInterface servant reference removed.");
    }


    public String getSupportedInterface(byte[] oid)
    {
        return HELLOINTERFACE_REP_ID;    
    }


        
}

