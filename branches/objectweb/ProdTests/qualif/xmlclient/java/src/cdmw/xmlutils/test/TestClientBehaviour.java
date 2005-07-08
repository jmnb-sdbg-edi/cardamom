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

package cdmw.xmlutils.test;

import cdmw.platformmngt.PlatformInterface;


class TestClientBehaviour
    extends cdmw.platformmngt.ProcessBehaviour
{
    
    private static final String XML_SERVICE_NAME = "XMLService";
    
    
    /**
     * Reference to the parent TestClient
     */
    private TestClient client;
    
    /**
     * Reference to the current ORB
     */
    private org.omg.CORBA.ORB orb;
    
    
    public TestClientBehaviour(TestClient client,
                               org.omg.CORBA.ORB orb)
    {
        this.client = client;
        this.orb = orb;
    }
    
    
    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) 
    {
        try {
            org.omg.CORBA.Object obj = 
                PlatformInterface.getService(XML_SERVICE_NAME);
            
            com.thalesgroup.CdmwXML.XMLParser xmlParser = 
                com.thalesgroup.CdmwXML.XMLParserHelper.narrow(obj);
        
            client.setXmlParser(xmlParser);
        
        } catch (com.thalesgroup.CdmwPlatformMngtService.ServiceNotFound e) {
            System.out.println("Test Failed: XML Service not found.");
            stop();

        } catch (com.thalesgroup.CdmwPlatformMngtService.ServiceUnavailable e) {
            System.out.println("Test Failed: XML Service not available.");
            stop();

        } catch (cdmw.common.BadOrderException e) {
            System.out.println("Test Failed: " +
                "BadOrderException while retrieving XML Service.");
            stop();

        } catch (cdmw.common.BadParameterException e) {
            System.out.println("Test Failed: " +
                "BadParameterException while retrieving XML Service.");
            stop();
        }
    }


    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    public void run()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun 
    {
        client.start();
    }
    
    
    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    public void stop() 
    {
        orb.shutdown(false);
    }
    
    
}

