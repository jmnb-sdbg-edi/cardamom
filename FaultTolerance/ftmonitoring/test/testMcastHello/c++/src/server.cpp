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
// server.cpp,v 1.4 2002/01/29 20:20:55 okellogg Exp

#include "testMcastHello/McastHello.h"
#include "ace/Get_Opt.h"

ACE_RCSID (McastHello,
           server,
           "server.cpp,v 1.4 2002/01/29 20:20:55 okellogg Exp")

static const char *ior_output_file = "test.ior";
static const char *group_ior = "corbaloc:miop:1.0@1.0-TestDomain-1/225.1.1.225:1234";

int
parse_args (int argc, char *argv[])
{
  ACE_Get_Opt get_opts (argc, argv, "o:");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'o':
        ior_output_file = get_opts.opt_arg ();
        break;

      case 'g':
        group_ior = get_opts.opt_arg ();
        break;

      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s "
                           "-o <iorfile>"
                           "-g <group ior corbaloc>"
                           "\n",
                           argv [0]),
                          -1);
      }
  // Indicates sucessful parsing of the command line
  return 0;
}

int
main (int argc, char *argv[])
{
  ACE_TRY_NEW_ENV
    {
      CORBA::ORB_var orb =
        CORBA::ORB_init (argc, argv, "" ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      CORBA::Object_var poa_object =
        orb->resolve_initial_references("RootPOA" ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      PortableServer::POA_var root_poa =
        PortableServer::POA::_narrow (poa_object.in () ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      if (CORBA::is_nil (root_poa.in ()))
        ACE_ERROR_RETURN ((LM_ERROR,
                           " (%P|%t) Panic: nil RootPOA\n"),
                          1);

      PortableServer::POAManager_var poa_manager =
        root_poa->the_POAManager (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;

      if (parse_args (argc, argv) != 0)
        return 1;

      // Get the group IOR.
      CORBA::String_var ior = CORBA::string_dup (group_ior);
      CORBA::Object_var group1 =
        orb->string_to_object (ior.in () ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      // Output the Group IOR to the <ior_output_file>
      FILE *output_file= ACE_OS::fopen (ior_output_file, "w");
      if (output_file == 0)
        ACE_ERROR_RETURN ((LM_ERROR,
                           "Cannot open output file for writing IOR: %s",
                           ior_output_file),
                              1);
      ACE_OS::fprintf (output_file, "%s", ior.in ());
      ACE_OS::fclose (output_file);

      ACE_DEBUG ((LM_DEBUG, "(%P|%t) create_id_for_reference of "));
      ACE_DEBUG ((LM_DEBUG, group_ior));
      ACE_DEBUG ((LM_DEBUG, "\n"));

      PortableServer::ObjectId_var id =
        root_poa->create_id_for_reference (group1.in ()
                                           ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      // Create and activate an instance of our servant.
      CORBA::Boolean servant0_status = 0;
      McastHello server_impl (orb.in (), 0, servant0_status);

      root_poa->activate_object_with_id (id.in (),
                                         &server_impl
                                         ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;


      poa_manager->activate (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;

      // Validate that our servants got the right requests.
      if (servant0_status != 0)
        ACE_ERROR_RETURN ((LM_ERROR,
                           "Multicast Servant already receive expected requests!"),
                          1);
      else
        ACE_DEBUG ((LM_DEBUG, "(%P|%t) server - Success init!\n"));

      orb->run (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;

      ACE_DEBUG ((LM_DEBUG, "(%P|%t) server - event loop finished\n"));

      root_poa->destroy (1, 1 ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      orb->destroy (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;

      // Validate that our servants got the right requests.
      if (servant0_status == 0)
        ACE_ERROR_RETURN ((LM_ERROR,
                           "Multicast Servant did not receive expected requests!"),
                          1);
      else
        ACE_DEBUG ((LM_DEBUG, "(%P|%t) server - Success!\n"));
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                           "Exception caught:");
      return 1;
    }
  ACE_ENDTRY;

  return 0;
}
