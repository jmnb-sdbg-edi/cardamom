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


package cdmw.resources;

import cdmw.lifecycle.ObjectDeactivator;

import com.thalesgroup.CF.ApplicationPOATie;
import com.thalesgroup.CF.DataType;
import com.thalesgroup.CF.InvalidResourceNumber;
import com.thalesgroup.CF.InvalidProperties;
import com.thalesgroup.CF.CreationError;
import com.thalesgroup.CF.CreateApplicationError;
import com.thalesgroup.CF.DevicePackage.UsageType;
import com.thalesgroup.CF.DevicePackage.InvalidLocation;

import org.omg.CORBA.ORB;
import org.omg.CORBA.SystemException;
import org.omg.CORBA.OBJECT_NOT_EXIST;
import org.omg.CORBA.BAD_OPERATION;
import org.omg.CORBA.BAD_PARAM;
import org.omg.PortableServer.POA;
import org.omg.PortableServer.Servant;
import org.omg.PortableServer.ForwardRequest;


/**
 * UserAnonymousResourceServantProvider
 *
 */
public class UserAnonymousResourceServantProvider
    extends ResourceServantProvider {

    public static final short DEV = 1;
    public static final short APP = 2;

    public static final String DEV_REP_ID
        = "IDL:thalesgroup.com/CF/Device:1.0";
    public static final String APP_REP_ID
        = "IDL:thalesgroup.com/CF/Application:1.0";

    // Properties Identifiers
    public static final String LABEL_ID = "LABEL";
    public static final String NAME_ID  = "NAME";

    /**
     * [Attribute description]
     */
    private ORB orb;

    /**
     * [Attribute description]
     */
    private POA poa;

    /**
     * [Attribute description]
     */
    private ObjectDeactivator objectDeactivator;

    /**
     * Constructor
     *
     * @param orb
     * @param poa
     * @param deactivator
     */
    protected UserAnonymousResourceServantProvider(ORB orb, POA poa,
        ObjectDeactivator deactivator) {

        this.orb = orb;
        this.poa = poa;
        this.objectDeactivator = deactivator;

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
        return true;
    }

    /**
     * Create a resource.
     *
     * @param resourceNumber
     * @param qualifiers
     *
     * @return Resource Object Id
     *
     * @exception InvalidResourceNumber
     * @exception InvalidProperties
     * @exception CreationError
     */
    public byte[] createResource(short resourceNumber, DataType[] qualifiers)
        throws  InvalidResourceNumber, InvalidProperties, CreationError {

        if (resourceNumber == DEV) {
            if ( (qualifiers.length == 1) &&
                (qualifiers[0].id.equals(LABEL_ID)) ) {
                try {
                    String label = qualifiers[0].value.extract_string();
                    byte[] oid = createDevice(label);
                    return oid;
                } catch (BAD_OPERATION bp) {
                    String[] messages = {"Wrong label value."};
                    throw new InvalidProperties(messages);
                } catch (InvalidLocation il) {
                    throw new CreationError();
                }
            } else {
                String[] messages = {"Label property is missing."};
                throw new InvalidProperties(messages);
            }
        } else if (resourceNumber == APP) {
            if ( (qualifiers.length == 1) &&
                (qualifiers[0].id.equals(NAME_ID)) ) {
                try {
                    String name = qualifiers[0].value.extract_string();
                    return createApplication(name);
                } catch (BAD_OPERATION bp) {
                    String[] messages = {"Wrong label value."};
                    throw new InvalidProperties(messages);
                } catch (CreateApplicationError cae) {
                    throw new CreationError();
                }
            } else {
                String[] messages = {"Name property is missing."};
                throw new InvalidProperties(messages);
            }
        } else {
            throw new InvalidResourceNumber();
        }

    }

    /**
     * Create a device.
     *
     * @param label
     *
     * @return Device Object Id
     *
     * @exception InvalidLocation
     */
    public byte[] createDevice(String label)
        throws InvalidLocation {

        try {
            String ostr = "DEV " + label;
            byte[] oid = ostr.getBytes();
            System.out.println("DEVICE <" + ostr
                + "> created with LABEL='" + label + "'");
            return oid;
        } catch (Exception e) {
            throw new InvalidLocation();
        }

    }

    /**
     * Create an application.
     *
     * @param name
     * @return Application Object Id
     * @exception CreateApplicationError
     */
    public byte[] createApplication(String name)
        throws CreateApplicationError {

        try {
            String oid_s = "APP " + name;
            byte[] oid = oid_s.getBytes();
            System.out.println("APPLICATION <" + oid_s +
                "> created with NAME='" + name + "'");
            return oid;
        } catch (Exception e) {
            throw new CreateApplicationError();
        }

    }

    /**
     * Creates a servant depending on its type
     *
     * @param oid Object Id to lookup for
     * @return A new servant
     * @exception ForwardRequest
     */
    public Servant lookupServant(byte[] oid)
        throws ForwardRequest {

        try  {
            String s = new String(oid);
            java.util.StringTokenizer st = new java.util.StringTokenizer(s);
            String type = st.nextToken();
            String info = st.nextToken();

            Servant servant = null;
            if (type.equals("DEV")) {
                // create it
                servant =
                    new DeviceImpl(poa, objectDeactivator, UsageType.IDLE, info);
            } else if (type.equals("APP")) {
                // create it
                ApplicationImpl applicationImpl 
                    = new ApplicationImpl(objectDeactivator, info);
                servant = new ApplicationPOATie(applicationImpl, poa);
            } else {
                throw new OBJECT_NOT_EXIST(); // TODO Minor codes
            }
            System.out.println("Activating " + s + " ... ");
            return servant;

        } catch (BAD_PARAM bp) { // Pb with ObjectId
            throw new OBJECT_NOT_EXIST(); // TODO Minor codes
        }

    }

    /**
     * Releases the servant passed in parameter
     *
     * @param oid
     * @param poa
     * @param servant
     */
    public void releaseServant(byte[] oid, POA poa, Servant servant) {

        try  {
            String s = new String(oid);
            System.out.println("Deactivating " + s + " ... ");
        } catch (BAD_PARAM bp) { // Pb with ObjectId
            // TODO throw an exception
        }

    }

    /**
     * Returns the repository id of the object with oid
     *
     * @param oid
     * @return The supported interface
     * @exception OBJECT_NOT_EXIST
     */
    public String getSupportedInterface(byte[] oid)
        throws OBJECT_NOT_EXIST {

        try {
            String s = new String(oid);
            java.util.StringTokenizer st = new java.util.StringTokenizer(s);
            String type = st.nextToken();
            String result = new String("");

            if (type.equals("DEV")) {
                result = DEV_REP_ID;
            } else if (type.equals("APP")) {
                result = APP_REP_ID;
            } else {
                throw new OBJECT_NOT_EXIST(); // TODO Minor codes
            }
            return result;

        } catch (BAD_PARAM bp) { // Pb with ObjectId
            // TODO throw an exception
        }
        return ""; // to silence the compiler

    }

}

