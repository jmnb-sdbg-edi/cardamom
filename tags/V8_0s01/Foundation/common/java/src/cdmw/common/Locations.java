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


package cdmw.common;

/**
 * This class provides standard CDMW paths for CDMW services.
 *
 */
public class Locations {

    /**
     * Implementation notes:
     * CDMW services registered within the Naming and Repository Service
     * will be registered only locally in the CDMW AdminRootContext following
     * the pattern below:
     *
     * [AdminRootContext]host_name/application_name/process_name/service_name/object_name
     *
     * The AdminRootContext will be accessible through the DefaultRootContext in the
     * NameDomain bellow:
     *
     * [DefaultRootContext]CDMW/ADMIN/<N&R_ID>
     *
     * N&R_ID is the NamingAndRepository Identifier.
     *
     * CDMW distributed services that should be accessible via a Name Domain will be
     * registered within the NameDomain bellow:
     *
     * [DefaultRootContext]CDMW/SERVICES/<service>/<object_name>
     * ex: CDMW/SERVICES/TRACE/COLLECTORS
     */

    /**
     * Name of the CDMW Admin Root Context within the CDMW
     * Naming and Repository service.
     */
    public final static String ADMIN_ROOT_CONTEXT_ID =
        "AdminRootContext";

    /**
     * Name of the CDMW NameDomain where CDMW services are registered
     * within the CDMW Naming and Repository DefaultRootContext.
     */
    public final static String CDMW_SERVICES_NAME_DOMAIN =
        "CDMW/SERVICES";

    /**
     * Name of the CDMW NameDomain where CDMW administration objects
     * are registered within the CDMW Naming and Repository DefaultRootContext.
     */
    public final static String CDMW_ADMIN_NAME_DOMAIN =
        "CDMW/ADMIN";

    /**
     * NameDomain Location of CDMW Trace Collectors objects within the CDMW
     * Naming and Repository service.
     */
    public final static String TRACE_COLLECTORS_NAME_DOMAIN =
        "TRACE/COLLECTORS";

    /**
     * NameDomain Location of CDMW event administration objects
     */
    public final static String CDMW_EVENT_SERVICE_NAME_DOMAIN =
        "CDMW/SERVICES/EVENT";

    /**
     * Name of the CDMW NameDomain where CDMW objects related
     * to the Assembly and the Deployment are registered
     * within the CDMW Naming and Repository service.
     */
    public final static String ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN =
        CDMW_SERVICES_NAME_DOMAIN + "/ASSEMBLYANDDEPLOYMENT";

}

