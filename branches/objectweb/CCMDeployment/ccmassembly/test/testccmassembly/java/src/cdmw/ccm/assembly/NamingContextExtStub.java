/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


package cdmw.ccm.assembly;

import org.omg.CosNaming.NameComponent;
import org.omg.CosNaming.NamingContext;
import org.omg.CosNaming.BindingListHolder;
import org.omg.CosNaming.BindingIteratorHolder;



class NamingContextExtStub 
    extends org.omg.CosNaming.NamingContextExtPOA
{

    public void
    bind(NameComponent[] n,
         org.omg.CORBA.Object obj)
        throws org.omg.CosNaming.NamingContextPackage.NotFound,
               org.omg.CosNaming.NamingContextPackage.CannotProceed,
               org.omg.CosNaming.NamingContextPackage.InvalidName,
               org.omg.CosNaming.NamingContextPackage.AlreadyBound
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public void
    rebind(NameComponent[] n,
           org.omg.CORBA.Object obj)
        throws org.omg.CosNaming.NamingContextPackage.NotFound,
               org.omg.CosNaming.NamingContextPackage.CannotProceed,
               org.omg.CosNaming.NamingContextPackage.InvalidName
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public void
    bind_context(NameComponent[] n,
                 NamingContext nc)
        throws org.omg.CosNaming.NamingContextPackage.NotFound,
               org.omg.CosNaming.NamingContextPackage.CannotProceed,
               org.omg.CosNaming.NamingContextPackage.InvalidName,
               org.omg.CosNaming.NamingContextPackage.AlreadyBound
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public void
    rebind_context(NameComponent[] n,
                   NamingContext nc)
        throws org.omg.CosNaming.NamingContextPackage.NotFound,
               org.omg.CosNaming.NamingContextPackage.CannotProceed,
               org.omg.CosNaming.NamingContextPackage.InvalidName
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public org.omg.CORBA.Object
    resolve(NameComponent[] n)
        throws org.omg.CosNaming.NamingContextPackage.NotFound,
               org.omg.CosNaming.NamingContextPackage.CannotProceed,
               org.omg.CosNaming.NamingContextPackage.InvalidName
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public void
    unbind(NameComponent[] n)
        throws org.omg.CosNaming.NamingContextPackage.NotFound,
               org.omg.CosNaming.NamingContextPackage.CannotProceed,
               org.omg.CosNaming.NamingContextPackage.InvalidName
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public NamingContext
    new_context()
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public NamingContext
    bind_new_context(NameComponent[] n)
        throws org.omg.CosNaming.NamingContextPackage.NotFound,
               org.omg.CosNaming.NamingContextPackage.AlreadyBound,
               org.omg.CosNaming.NamingContextPackage.CannotProceed,
               org.omg.CosNaming.NamingContextPackage.InvalidName
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public void
    destroy()
        throws org.omg.CosNaming.NamingContextPackage.NotEmpty
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public void
    list(int how_many,
         BindingListHolder bl,
         BindingIteratorHolder bi)
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }


    public String
    to_string(NameComponent[] n)
        throws org.omg.CosNaming.NamingContextPackage.InvalidName
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public NameComponent[]
    to_name(String sn)
        throws org.omg.CosNaming.NamingContextPackage.InvalidName
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public String
    to_url(String addr,
           String sn)
        throws org.omg.CosNaming.NamingContextExtPackage.InvalidAddress,
               org.omg.CosNaming.NamingContextPackage.InvalidName
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public org.omg.CORBA.Object
    resolve_str(String sn)
        throws org.omg.CosNaming.NamingContextPackage.NotFound,
               org.omg.CosNaming.NamingContextPackage.CannotProceed,
               org.omg.CosNaming.NamingContextPackage.InvalidName
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    
}



