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


package cdmw.ccm.cif;

/**
 * This class provides helper functions to be used by the CDMW generated 
 * CdmwInterface.java file.
 * 
 */
public class InitUtils {

    /**
     * Register the value type factories in the ORB
     * 
     * @param orb A reference to the ORB pseudo-object
     */
    public static void registerValuetypeFactories(org.omg.CORBA.ORB orb) {

        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            org.omg.Components.FacetDescriptionHelper.id(),
                new FacetDescriptionFactoryImpl());

        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            org.omg.Components.PortDescriptionHelper.id(),
                new PortDescriptionFactoryImpl());

        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            org.omg.Components.CookieHelper.id(),
                new cdmw.ccm.common.CookieFactoryImpl());

        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            org.omg.Components.ConnectionDescriptionHelper.id(),
                new ConnectionDescriptionFactoryImpl());

        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            org.omg.Components.ReceptacleDescriptionHelper.id(),
                new ReceptacleDescriptionFactoryImpl());

        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            org.omg.Components.ConsumerDescriptionHelper.id(),
                new ConsumerDescriptionFactoryImpl());

        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            org.omg.Components.EmitterDescriptionHelper.id(),
                new EmitterDescriptionFactoryImpl());

        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            org.omg.Components.SubscriberDescriptionHelper.id(),
                new SubscriberDescriptionFactoryImpl());

        ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
            org.omg.Components.PublisherDescriptionHelper.id(),
                new PublisherDescriptionFactoryImpl());

    }

}

