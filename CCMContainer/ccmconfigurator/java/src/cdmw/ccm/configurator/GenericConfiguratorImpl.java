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


package cdmw.ccm.configurator;

import cdmw.ccm.deployment.ConfigValueImpl;

import org.omg.Components.CCMObject;
import org.omg.Components.CCMHome;
import org.omg.Components.WrongComponentType;
import org.omg.Components.ConfigValue;
import org.omg.CORBA.*;

public class GenericConfiguratorImpl
    extends com.thalesgroup.CdmwDeployment.GenericConfiguratorPOA {

    /**
     * Interface id of the supported objects of the configurator
     */
    private String supportedId;

    /**
     * Current orb instance
     */
    private org.omg.CORBA.ORB orb = null;

    /**
     * Reference to the configurator POA
     */
    private org.omg.PortableServer.POA poa;

    /**
     * List of the ConfigValues
     */
    private ConfigValueImpl[] config;

    /**
     * Names of the TCKind values
     */
    private static java.util.Hashtable typeNames;

    static {
        //
        // Put the names of the TCKind values in a hashtable
        //
        typeNames = new java.util.Hashtable();
        typeNames.put(org.omg.CORBA.TCKind.tk_null, "null");
        typeNames.put(org.omg.CORBA.TCKind.tk_void, "void");
        typeNames.put(org.omg.CORBA.TCKind.tk_short, "short");
        typeNames.put(org.omg.CORBA.TCKind.tk_ushort, "unsigned short");
        typeNames.put(org.omg.CORBA.TCKind.tk_long, "long");
        typeNames.put(org.omg.CORBA.TCKind.tk_ulong, "unsigned long");
        typeNames.put(org.omg.CORBA.TCKind.tk_float, "float");
        typeNames.put(org.omg.CORBA.TCKind.tk_double, "double");
        typeNames.put(org.omg.CORBA.TCKind.tk_boolean, "boolean");
        typeNames.put(org.omg.CORBA.TCKind.tk_char, "char");
        typeNames.put(org.omg.CORBA.TCKind.tk_octet, "octet");
        typeNames.put(org.omg.CORBA.TCKind.tk_any, "any");
        typeNames.put(org.omg.CORBA.TCKind.tk_TypeCode, "TypeCode");
        typeNames.put(org.omg.CORBA.TCKind.tk_Principal, "Principal");
        typeNames.put(org.omg.CORBA.TCKind.tk_objref, "objref");
        typeNames.put(org.omg.CORBA.TCKind.tk_struct, "struct");
        typeNames.put(org.omg.CORBA.TCKind.tk_union, "union");
        typeNames.put(org.omg.CORBA.TCKind.tk_enum, "enum");
        typeNames.put(org.omg.CORBA.TCKind.tk_string, "string");
        typeNames.put(org.omg.CORBA.TCKind.tk_sequence, "sequence");
        typeNames.put(org.omg.CORBA.TCKind.tk_array, "array");
        typeNames.put(org.omg.CORBA.TCKind.tk_alias, "alias");
        typeNames.put(org.omg.CORBA.TCKind.tk_except, "exception");
        typeNames.put(org.omg.CORBA.TCKind.tk_longlong, "long long");
        typeNames.put(org.omg.CORBA.TCKind.tk_ulonglong,
                       "unsigned long long");
        typeNames.put(org.omg.CORBA.TCKind.tk_longdouble, "long double");
        typeNames.put(org.omg.CORBA.TCKind.tk_wchar, "wchar");
        typeNames.put(org.omg.CORBA.TCKind.tk_wstring, "wstring");
        typeNames.put(org.omg.CORBA.TCKind.tk_fixed, "fixed");
        typeNames.put(org.omg.CORBA.TCKind.tk_value, "valuetype");
        typeNames.put(org.omg.CORBA.TCKind.tk_value_box, "valuebox");
        typeNames.put(org.omg.CORBA.TCKind.tk_native, "native");
        typeNames.put(org.omg.CORBA.TCKind.tk_abstract_interface,
                       "abstract interface");
    }

    /**
     * Constructor
     *
     * @param orb reference to the current ORB
     * @param typeId interface id of the supported objects
     */
    public GenericConfiguratorImpl(org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA configuratorPOA, String typeId) {
        this.orb = orb;
        this.poa = configuratorPOA;
        this.supportedId = typeId;
    }

    /**
     * Returns the default POA.
     *
     * @return Default POA.
     */
    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

    //
    // IDL:omg.org/Components/Configurator/configure:1.0
    //
    /**
     * This operation established its encapsulated configuration
     * on the target CCMObject.
     *
     * @param comp The CCMObject to be configured.
     *
     * @exception WrongComponentType If the CCMObject is not
     * of the type expected by the configurator.
     */
    public void configure(CCMObject comp) throws WrongComponentType {
        configureObject(comp);
    }

    //
    // IDL:omg.org/Components/StandardConfigurator/set_configuration:1.0
    //
    /**
     * This operation sets the configuration which will be applied
     * to CCMObjects.
     *
     * @param descr The list of ConfigValues, where each ConfigValue contains
     * the name of an attribute and a value for that attribute.
     */
    public void set_configuration(ConfigValue[] descr) {
        setConfiguration(descr);
    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/HomeConfigurator/configure_home:1.0
    //
    /**
     * This operation established its encapsulated configuration
     * on the target home.
     *
     * @param home_ref The home to be configured.
     *
     * @exception WrongComponentType If the home is not
     * of the type expected by the configurator.
     */
    public void configure_home(CCMHome home_ref) throws WrongComponentType {
        configureObject(home_ref);
    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/StandardHomeConfigurator/set_home_configuration:1.0
    //
    /**
     * This operation sets the configuration which will be applied to homes.
     *
     * @param descr The list of ConfigValues, where each ConfigValue contains
     * the name of an attribute and a value for that attribute.
     */
    public void set_home_configuration(ConfigValue[] descr) {
        setConfiguration(descr);
    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/GenericConfigurator/remove:1.0
    //
    /**
     * This operation stops the GenericConfigurator and removes it.
     */
    public void remove() {
        try {
            byte[] oid = this.poa.reference_to_id(this._this());
            this.poa.deactivate_object(oid);
        } catch(org.omg.PortableServer.POAPackage.WrongAdapter wa) {
            throw new org.omg.CORBA.INTERNAL(wa.toString());
        } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            throw new org.omg.CORBA.INTERNAL(wp.toString());
        } catch(org.omg.PortableServer.POAPackage.ObjectNotActive ona) {
            throw new org.omg.CORBA.INTERNAL(ona.toString());
        }

    }

    /**
     * This operation sets the configuration which will be applied
     * to both homes and CCMObjects.
     * The configuration should be valid (existing attributes, coherent types
     * and no readonly attributes). The CpfReader check the validity at
     * configuration creation.
     *
     * @param descr The list of ConfigValues, where each ConfigValue contains
     * the name of an attribute and a value for that attribute.
     */
    private void setConfiguration(ConfigValue[] descr) {
        config = new ConfigValueImpl[descr.length];
        for(int i=0; i<descr.length; i++) {
            config[i] = (ConfigValueImpl) descr[i];
        }
    }

    /**
     * This operation sets the configuration which will be applied
     * to both homes and CCMObjects.
     *
     * @param obj The CORBA object to be configured
     * @exception WrongComponentType If the object is not
     * of the type expected by the configurator.
     */
    private void configureObject(org.omg.CORBA.Object obj)
        throws org.omg.Components.WrongComponentType {

        if ( !obj._is_a(supportedId) ) {
            throw new org.omg.Components.WrongComponentType(
                "interface " + supportedId + " not supported");
        }

        for (int i=0; i<config.length; i++) {
	    // method name
            String method = "_set_" + config[i].name;

            // argument
            org.omg.CORBA.NVList arg = orb.create_list(1);
            arg.add_value(config[i].name,
                config[i].value, org.omg.CORBA.ARG_IN.value);

            // return type
            org.omg.CORBA.Any resultAny = orb.create_any();
            resultAny.type(orb.get_primitive_tc(org.omg.CORBA.TCKind.tk_void));
            org.omg.CORBA.NamedValue result = orb.create_named_value(
                null, resultAny, org.omg.CORBA.ARG_OUT.value);
            // method request
            org.omg.CORBA.Request request
                = obj._create_request(null, method, arg, result);

            // invocation
          try {
                request.invoke();
		Exception e = request.env().exception();
		if(e != null)
		{
			System.out.println(e.getClass().getName());
			if((e.getClass().getName()).equals("org.omg.CORBA.BAD_OPERATION"))
//			if(e.isInstance(new org.omg.CORBA.BAD_OPERATION()))
                 		 throw new org.omg.CORBA.BAD_OPERATION();
	        	else if ((e.getClass().getName()).equals("org.omg.CORBA.MARSHAL"))
		  		 throw new org.omg.CORBA.MARSHAL();
		}
            } catch (org.omg.CORBA.BAD_OPERATION bo) {
		 throw new org.omg.Components.WrongComponentType(
                    "the object to configure doesn't have a writable '" 
                    + config[i].name + "' attribute");
            } catch (org.omg.CORBA.MARSHAL ma) {
                throw new org.omg.Components.WrongComponentType(
                    "the '" + config[i].name + "' attribute doesn't have the expected type.");
            }
        }

    }

}

