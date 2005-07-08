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


package cdmw.tools;

import cdmw.common.Assert;
import cdmw.orbsupport.ORBUtils;
import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.namingandrepository.RepositoryInterface;

import org.omg.CORBA.CompletionStatus;

public class RepAdmin {

    public static final int SUCCESS = 0;
    public static final int FAILURE = 1;

    public static void usage(java.io.PrintStream out, String programName) {
        out.println("Usage:\n"
           + programName + " options\n"
           + "\n"
           + "Options:\n"
           + "--help       Display this information.\n"
           + "--url  IOR   URL of the CDMW Naming and Repository service.\n");
    }

    public static com.thalesgroup.CdmwNamingAndRepository.Repository
        getCdmwRepository(org.omg.CORBA.ORB orb, String url)
        throws org.omg.CORBA.TRANSIENT {

        Assert.check(orb!=null);

        org.omg.CORBA.Object obj = null;
        //
        // Get naming service
        //
        try {
            obj = orb.string_to_object(url);
        } catch(Exception e) {
            System.out.println("Invalid URL");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_NO);
        }

        if (obj==null) {
            System.out.println("URL denotes a nil object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_NO);
        }

        // Get reference to the repository interface
        com.thalesgroup.CdmwNamingAndRepository.Repository rep = null;
        try {
            rep = com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper.narrow(obj);
            if (rep==null) {
                System.out.println("Couldn't retrieve reference of the Repository");
                throw new org.omg.CORBA.TRANSIENT(
                    ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                        CompletionStatus.COMPLETED_NO);
            }
        } catch (Exception e) {
            System.out.println("Couldn't retrieve reference of the Repository");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_NO);
        }

        return rep;
    }

    public static int run(org.omg.CORBA.ORB orb, String url) {
        // Retrive Repository object reference
        com.thalesgroup.CdmwNamingAndRepository.Repository rep
            = getCdmwRepository(orb,url);

        cdmw.tools.RepositoryAdmin adm
            = new cdmw.tools.RepositoryAdmin(orb,rep);

        adm.run(new java.io.BufferedReader(
            new java.io.InputStreamReader(System.in)), System.out);

        return SUCCESS;
    }

    public static void main(String[] args) {
        int status = SUCCESS;
        org.omg.CORBA.ORB orb = null;

        if(args.length == 1) {
            usage(System.out, args[0]);
            status = FAILURE;
            return;
        }

        String narUrl = null;
        //
        // Execute command
        //
        if( args[1].equals("--help") ) {
            usage(System.out, args[0]);
            status = SUCCESS;
        } else if( args[1].equals("--url") ) {
            if(args.length != 3) {
                usage(System.out,args[0]);
                status = FAILURE;
            } else {
                narUrl = args[2];
            }
        } else {
            usage(System.out,args[0]);
            status = FAILURE;
        }


        try {
            orb = org.omg.CORBA.ORB.init(args, ORBUtils.getProperties());
            status = run(orb, narUrl);
        } catch(Exception e) {
            System.out.println(e.getMessage());
            status = FAILURE;
        }

        if(orb!=null) {
            try {
                orb.destroy();
            } catch(Exception e) {
                System.out.println(e.getMessage());
                status = FAILURE;
            }
        }

        if( status==SUCCESS ) {
            System.out.println("RepAdmin execution ok.");
        } else {
            System.out.println("RepAdmin execution failed !");
        }
    }

}

