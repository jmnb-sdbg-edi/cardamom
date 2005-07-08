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


package cdmw.ccm.deployment;

import cdmw.xmlutils.XMLHelper;

import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException;

import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.ElementDescription;

import org.omg.CORBA.Any;
import org.omg.CORBA.TypeCode;
//import org.omg.DynamicAny.*;

public class CpfReader extends XmlReader {

    /**
     * Reference to the ORB instance
     */
    private org.omg.CORBA.ORB orb;

    /**
     * DynAnyFactory reference
     */
    private org.omg.DynamicAny.DynAnyFactory daf;

    /**
     * Interface repository reference
     */
    private org.omg.CORBA.Repository interfaceRep;

    /**
     * Interface Repository id of the supported interface
     */
    private String supportedId;

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
     * @param parser reference to the XML Parser
     * @param cpfFilename name of the properties XML file
     * @param orb reference to the current ORB instance
     * @param id interface repository id of the supported interface
     * @exception ParsingException if the parsed document contains errors
     */
    public CpfReader(XMLParser parser, String cpfFilename,
        org.omg.CORBA.ORB orb, String id) throws ParsingException {

        super(parser, cpfFilename);
        this.orb = orb;
        this.supportedId = id;
        try {
            daf = org.omg.DynamicAny.DynAnyFactoryHelper.narrow(
                orb.resolve_initial_references("DynAnyFactory"));
            interfaceRep = org.omg.CORBA.RepositoryHelper.narrow(
                orb.resolve_initial_references("InterfaceRepository"));
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            throw new org.omg.CORBA.BAD_PARAM(in.getMessage());
        }

    }

    /**
     * Read the properties XML file and returns the corresponding list
     * of ConfigValues (associations between the name of the attribute
     * and their value).
     *
     * @return The list of ConfigValues read in the XML file.
     * @exception ParsingException if the properties XML file
     * contains an error.
     */
    public ConfigValues getConfigValues() throws ParsingException {

        try {
            // System.out.println(">>> cpfreader-getConfigValues");
            
            // get the attributes nodes in the properties XML file
            short[] xpathQuery = XMLHelper.toShortArray("//properties/*");
            com.thalesgroup.CdmwXML.NodeDescription[] nodes
                = doc.select_nodes_from_root(xpathQuery);

            // get the attributes description from the repository interface
            if (interfaceRep == null) {
                throw new org.omg.CORBA.BAD_PARAM(
                    "interface repository not available");
            }
            org.omg.CORBA.Contained obj = interfaceRep.lookup_id(supportedId);
            org.omg.CORBA.InterfaceDef interfaceDef =
                org.omg.CORBA.InterfaceDefHelper.narrow(obj);
            
            if (interfaceDef == null) {
                throw new org.omg.CORBA.BAD_PARAM(
                    supportedId + " is not an interface");
            }
            
            /*
             * The following commented code should be enough with a compliant Interface Repository.
             * But the OpenCCM 0.4 Interface Repository is not.
             * It doesn't return the inherited attributes.
             */
            // org.omg.CORBA.AttributeDescription[] attrs
            //     = interfaceDef.describe_interface().attributes;

            /*
             * Call specific operation for attributes retrieval with OpenCCM 0.4
             */
            org.omg.CORBA.AttributeDescription[] attrs = getAttributes(interfaceDef);                
                
            // create the config values
            ConfigValueImpl[] cv = new ConfigValueImpl[nodes.length];
            for(int i=0; i<nodes.length; i++) {
                Element elem = ElementHelper.narrow(nodes[i].reference());
                String name = getName(elem);
                org.omg.CORBA.AttributeDescription attr = getAttribute(name, attrs);
                // check if attribute is not read-only
                if (attr.mode.value() == org.omg.CORBA.AttributeMode._ATTR_READONLY) {
                    throw new ParsingException("Attribute " + name + 
                        " is readonly. It can't be configured by a .cpf file");
                }
                TypeCode tc = attr.type;
       
                cv[i] = new ConfigValueImpl(name, getValue(elem, tc));
            }

            return new ConfigValues(cv);
        } catch(com.thalesgroup.CdmwXML.DOMException de) {
            throw new ParsingException(de.reason);
        } catch(com.thalesgroup.CdmwXML.XPathFormatException xpfe) {
            throw new ParsingException(xpfe.reason);
        }

    }

    
    /**
     * Returns the list of attributes of an interface (could be a component or a home) 
     * This operation compensates the OpenCCM 0.4 gap.
     * (it doesn't returns the attributes of base components 
     * and supported interfaces)
     *
     * @param interfaceDef the interface definition of an interface, 
     * a component or a home.
     * @return the list of component's attributes
     * @exception org.omg.CORBA.BAD_PARAM if interfaceDef is not
     * the definition of a component's aquivalent interface.
     */
    private org.omg.CORBA.AttributeDescription[] getAttributes(org.omg.CORBA.InterfaceDef interfaceDef)
        throws org.omg.CORBA.BAD_PARAM
    {
        // System.out.println("  ............. getComponentAttributes for " + interfaceDef.id());

        org.omg.CORBA.AttributeDescription[] result = null;
        
        // test if interfaceDef correspond to a component
        if (interfaceDef._is_a(org.omg.CORBA.ComponentIR.ComponentDefHelper.id())) {
            org.omg.CORBA.ComponentIR.ComponentDef componentDef =
                org.omg.CORBA.ComponentIR.ComponentDefHelper.narrow(interfaceDef);
            result = getComponentAttributes(componentDef);
        }
        else
        // test if interfaceDef correspond to a home
        if (interfaceDef._is_a(org.omg.CORBA.ComponentIR.HomeDefHelper.id())) {
            org.omg.CORBA.ComponentIR.HomeDef homeDef =
                org.omg.CORBA.ComponentIR.HomeDefHelper.narrow(interfaceDef);
            result = getHomeAttributes(homeDef);
        }
        else 
            result = getInterfaceAttributes(interfaceDef);          
                
        // System.out.println("  .... found attributes:");
        // for (int j=0; j<result.length; j++) {
        //    System.out.println("     " + result[j].name + ": " + result[j].id);
        // }
        // System.out.println("  ..............................");

        return result;
    }
    
    /**
     * Returns the list of attributes of a component. 
     * This operation compensates the OpenCCM 0.4 gap.
     * (it doesn't returns the attributes of base components 
     * and supported interfaces)
     *
     * @param componentDef the component's definition.
     * @return the list of component's attributes
     */
    private org.omg.CORBA.AttributeDescription[] getComponentAttributes(org.omg.CORBA.ComponentIR.ComponentDef componentDef)
    {        
        // System.out.println("  ...... getComponentAttributes for " + componentDef.id());

        // get attributes of this component
        org.omg.CORBA.AttributeDescription[] componentAttrs = 
            componentDef.describe_interface().attributes;
        
        // get attributes of the base component
        org.omg.CORBA.AttributeDescription[] baseComponentAttrs = 
            new org.omg.CORBA.AttributeDescription[0];
        if (componentDef.base_component() != null) {
            baseComponentAttrs = getComponentAttributes(componentDef.base_component());
            componentAttrs = mergeAttributesArrays(componentAttrs, baseComponentAttrs);
        }
        
        // get attributes of supported interfaces
        org.omg.CORBA.InterfaceDef[] supported_interfaces =
        	componentDef.supported_interfaces();
        for (int i=0; i<supported_interfaces.length; ++i) {
            org.omg.CORBA.AttributeDescription[] interfaceAttrs =
            	getInterfaceAttributes(supported_interfaces[i]);
            if (interfaceAttrs.length > 0) {
                componentAttrs = mergeAttributesArrays(componentAttrs, interfaceAttrs);
            }
        }

        return componentAttrs;
    }
    
    /**
     * Returns the list of attributes of a home. 
     * This operation compensates the OpenCCM 0.4 gap.
     * (it doesn't returns the attributes of base homes) 
     *
     * @param homeDef the home's definition.
     * @return the list of home's attributes
     */
    private org.omg.CORBA.AttributeDescription[] getHomeAttributes(org.omg.CORBA.ComponentIR.HomeDef homeDef)
    {        
        // System.out.println("  ...... getHomeAttributes for " + homeDef.id());

        // get attributes of this home
        org.omg.CORBA.AttributeDescription[] homeAttrs = 
            homeDef.describe_interface().attributes;
        
        // get attributes of the base home
        org.omg.CORBA.AttributeDescription[] baseHomeAttrs = 
            new org.omg.CORBA.AttributeDescription[0];
        if (homeDef.base_home() != null) {
            baseHomeAttrs = getHomeAttributes(homeDef.base_home());
            homeAttrs = mergeAttributesArrays(homeAttrs, baseHomeAttrs);
        }

        return homeAttrs;
    }

    /**
     * Returns the list of attributes of an interface. 
     * This operation compensates the OpenCCM 0.4 gap.
     * (it doesn't returns the attributes of base interfaces) 
     *
     * @param interfaceDef the interface's definition.
     * @return the list of interface's attributes
     */
    private org.omg.CORBA.AttributeDescription[] getInterfaceAttributes(org.omg.CORBA.InterfaceDef interfaceDef)
    {
        // System.out.println("  ...... getInterfaceAttributes for " + interfaceDef.id());
        
        // get attributes of this interface
        org.omg.CORBA.AttributeDescription[] interfaceAttrs = 
            interfaceDef.describe_interface().attributes;
        
        // get attributes of base interfaces
        org.omg.CORBA.InterfaceDef[] base_interfaces =
        	interfaceDef.base_interfaces();
        for (int i=0; i<base_interfaces.length; ++i) {
            org.omg.CORBA.AttributeDescription[] baseInterfaceAttrs =
            	getInterfaceAttributes(base_interfaces[i]);
            if (interfaceAttrs.length > 0) {
                interfaceAttrs = mergeAttributesArrays(interfaceAttrs, baseInterfaceAttrs);
            }
        }
        
        return interfaceAttrs;
    }
    
    /**
     * Copy two AttributeDescription arrays into one array. 
     *
     * @param attrs1 the first array.
     * @param attrs2 the second array.
     * @return an array filled with attrs1 and attrs2.
     */
    private org.omg.CORBA.AttributeDescription[] mergeAttributesArrays(org.omg.CORBA.AttributeDescription[] attrs1,
                                                                       org.omg.CORBA.AttributeDescription[] attrs2)
    {
        org.omg.CORBA.AttributeDescription[] result = 
            new org.omg.CORBA.AttributeDescription[attrs1.length + attrs2.length];
        for (int i=0; i<attrs1.length; ++i) {
            result[i] = attrs1[i];
        }
        for (int i=0; i<attrs2.length; ++i) {
            result[attrs1.length+i] = attrs2[i];
        }
        
        return result;
    }
    
    
    
    /**
     * Find the AttributeDescription with the specified name.
     *
     * @param name name of the attribute to be found
     * @param attrs description of all the available attributes
     * @return The AttributeDescription of the requested attribute
     * @exception com.thalesgroup.CdmwXML.DOMException if the name
     * of the attribute does not exist in the interface
     */
    private org.omg.CORBA.AttributeDescription getAttribute(String name,
        org.omg.CORBA.AttributeDescription[] attrs)
        throws com.thalesgroup.CdmwXML.DOMException 
    {

        // System.out.println("  ....... Test if attribute " + name + " is defined in " + supportedId);           
            
        for(int i=0; i<attrs.length; i++) {
            if (attrs[i].name.equals(name)) {
                return attrs[i];
            }
        }
        throw new com.thalesgroup.CdmwXML.DOMException((short)0,
            name + " is not an attribute from the interface " + supportedId);

    }

    /**
     * Returns the value of the given DOM Element with its TypeCode
     * as an Any object.
     *
     * @param elem DOM Element from which the value must be extracted
     * @param tc TypeCode of the corresponding object
     * in the interface repository
     * @return The value of the element.
     * @exception com.thalesgroup.CdmwXML.DOMException
     * if an error occurs in the XML file
     */
    private Any getValue(Element elem, TypeCode tc)
        throws com.thalesgroup.CdmwXML.DOMException {

        String propertyType = getPropertyType(elem);
        org.omg.DynamicAny.DynAny dynAny = null;
        try {
            dynAny = daf.create_dyn_any_from_type_code(tc);
        } catch(org.omg.DynamicAny.DynAnyFactoryPackage.InconsistentTypeCode itc) {
            throw new org.omg.CORBA.BAD_PARAM("Attempt to create a DynAny "
                + "object with an inconsistent TypeCode");
        }
        try {
            while (tc.kind().value() == org.omg.CORBA.TCKind._tk_alias) {
                // this is an alias, retrieve the content type
                tc = tc.content_type();
            }
        } catch(org.omg.CORBA.TypeCodePackage.BadKind bk) {
            // should never happen because the method is called
            // only if the TCKind is an alias
            throw new org.omg.CORBA.BAD_PARAM("Attempt to invoke "
                + "a method on an inappropriate kind of TypeCode object");
        }

        if (propertyType.equals("simple")) {
            if (tc.kind().value() == org.omg.CORBA.TCKind._tk_enum) {
                return getEnumValue(dynAny, elem);
            } else {
                return getSimpleValue(dynAny, elem);
            }
        } else if (propertyType.equals("sequence")) {
            return getSequenceValue(dynAny, elem, tc);
        } else if (propertyType.equals("struct")) {
            return getStructValue(dynAny, elem, tc);
        } else if (propertyType.equals("valuetype")) {
            return getValuetypeValue(dynAny, elem, tc);
        } else {
            // we should never be here if the XML file is validated
            throw new com.thalesgroup.CdmwXML.DOMException((short)0,
                "property type " + propertyType + " is not supported");
        }

    }

    /**
     * Returns the value of a simple element as an Any object.
     *
     * @param dynAny Instance of a DynAny object.
     * @param elem Current element in the XML file.
     * @return Value of the property node.
     * @exception com.thalesgroup.CdmwXML.DOMException
     * if an error occurs during reading the data.
     */
    private Any getSimpleValue(org.omg.DynamicAny.DynAny dynAny, Element elem)
        throws com.thalesgroup.CdmwXML.DOMException {

        try {
            String type = getType(elem);
            String value = getChildValue(elem).trim();

            if (type.equals("boolean")) {
                dynAny.insert_boolean(Boolean.valueOf(value).booleanValue());
            } else if (type.equals("char")) {
                dynAny.insert_char(value.charAt(0));
            } else if (type.equals("double")) {
                dynAny.insert_double(Double.parseDouble(value));
            } else if (type.equals("float")) {
                dynAny.insert_float(Float.parseFloat(value));
            } else if (type.equals("short")) {
                dynAny.insert_short(Short.parseShort(value));
            } else if (type.equals("long")) {
                dynAny.insert_long(Integer.parseInt(value));
            } else if (type.equals("objref")) {
                org.omg.CORBA.Object obj = orb.string_to_object(value);
                if (obj == null) {
                    throw new com.thalesgroup.CdmwXML.DOMException((short)0,
                        value + ": invalid object reference");
                }
                dynAny.insert_reference(obj);
            } else if (type.equals("octet")) {
                dynAny.insert_octet(Byte.parseByte(value));
            } else if (type.equals("string")) {
                dynAny.insert_string(value);
            } else if (type.equals("ulong")) {
                dynAny.insert_ulong(Integer.parseInt(value));
            } else if (type.equals("ushort")) {
                dynAny.insert_ushort(Short.parseShort(value));
            } else {
                // we should never be here if the XML file is validated
                throw new com.thalesgroup.CdmwXML.DOMException((short)0,
                    "simple type " + type + " is not supported");
            }
        } catch(NumberFormatException nfe) {
            throw new com.thalesgroup.CdmwXML.DOMException(
                (short)0, nfe.getMessage());
        } catch(org.omg.DynamicAny.DynAnyPackage.TypeMismatch tm) {
            throw new com.thalesgroup.CdmwXML.DOMException((short)0,
                "argument type mismatch for attribute type, found: "
                + getType(elem) +", required: "
                + typeNames.get(dynAny.type().kind()));
        } catch(org.omg.DynamicAny.DynAnyPackage.InvalidValue iv) {
            // should never happen
            throw new org.omg.CORBA.BAD_PARAM("method called on a DynAny "
                + "whose current component itself has components");
        }
        return dynAny.to_any();

    }
    
    /**
     * Returns the value of an enum element as an Any object
     *
     * @param dynAny Instance of a DynAny object.
     * @param elem Current element in the XML file.
     * @return Value of the enum element.
     * @exception com.thalesgroup.CdmwXML.DOMException
     * if an error occurs during reading the data.
     */
    private Any getEnumValue(org.omg.DynamicAny.DynAny dynAny, Element elem)
        throws com.thalesgroup.CdmwXML.DOMException {

        org.omg.DynamicAny.DynEnum dynEnum =
            org.omg.DynamicAny.DynEnumHelper.narrow(dynAny);
        String value = getChildValue(elem).trim();
        try {
            // by default, we use 'set_as_string' and not 'set_as_ulong'
            dynEnum.set_as_string(value);
        } catch(org.omg.DynamicAny.DynAnyPackage.InvalidValue iv) {
            throw new com.thalesgroup.CdmwXML.DOMException((short)0,
                "value contains a string that is not a valid IDL identifier" 
                + " for the corresponding enumerated type");
        }
        return dynEnum.to_any();
    
    }

    /**
     * Returns the value of a sequence element as an Any object
     *
     * @param dynAny Instance of a DynAny object.
     * @param elem Current element in the XML file.
     * @param tc TypeCode of the current element.
     * @return Value of the sequence element.
     * @exception com.thalesgroup.CdmwXML.DOMException
     * if an error occurs during reading the data.
     */
    private Any getSequenceValue(
        org.omg.DynamicAny.DynAny dynAny, Element elem, TypeCode tc)
        throws com.thalesgroup.CdmwXML.DOMException {

        org.omg.DynamicAny.DynSequence dynSequence =
            org.omg.DynamicAny.DynSequenceHelper.narrow(dynAny);
        Element[] elems = getChildProperties(elem);
        Any[] elements = new Any[elems.length];
        try {
            for(int i=0; i<elems.length; i++) {
                elements[i] = getValue(elems[i], tc.content_type());
            }
        } catch(org.omg.CORBA.TypeCodePackage.BadKind bk) {
            // should never happen because the property type is a sequence
            throw new org.omg.CORBA.BAD_PARAM("Attempt to invoke "
                + "a method on an inappropriate kind of TypeCode object");
        }
        try {
            dynSequence.set_elements(elements);
        } catch(org.omg.DynamicAny.DynAnyPackage.TypeMismatch tm) {
            throw new com.thalesgroup.CdmwXML.DOMException((short)0,
                "one or more elements have a TypeCode which is not "
                + "equivalent to the element TypeCode of the sequence");
        } catch(org.omg.DynamicAny.DynAnyPackage.InvalidValue iv) {
            throw new com.thalesgroup.CdmwXML.DOMException((short)0,
                "the length of the elements exceeds the bound "
                + "of the bounded sequence");
        }
        return dynSequence.to_any();

    }

    /**
     * Returns the value of a struct element as an Any object.
     *
     * @param dynAny Instance of a DynAny object.
     * @param elem Current element in the XML file.
     * @param tc TypeCode of the current element.
     * @return Value of the struct element.
     * @exception com.thalesgroup.CdmwXML.DOMException
     * if an error occurs during reading the data.
     */
    private Any getStructValue(
        org.omg.DynamicAny.DynAny dynAny, Element elem, TypeCode tc)
        throws com.thalesgroup.CdmwXML.DOMException {

        org.omg.DynamicAny.DynStruct dynStruct =
            org.omg.DynamicAny.DynStructHelper.narrow(dynAny);
        Element[] elems = getChildProperties(elem);
        org.omg.DynamicAny.NameValuePair[] members
            = new org.omg.DynamicAny.NameValuePair[elems.length];
        for(int i=0; i<elems.length; i++) {
            String name = getName(elems[i]);
            TypeCode type = getMemberType(tc, name);
            members[i] = new org.omg.DynamicAny.NameValuePair(
                name, getValue(elems[i], type));
        }
        try {
            dynStruct.set_members(members);
        } catch(org.omg.DynamicAny.DynAnyPackage.TypeMismatch tm) {
            throw new com.thalesgroup.CdmwXML.DOMException((short)0,
                "the member names do not match the corresponding member name" +
                " in the Struct's TypeCode and they are not empty strings");
        } catch(org.omg.DynamicAny.DynAnyPackage.InvalidValue iv) {
            throw new com.thalesgroup.CdmwXML.DOMException((short)0,
                "members have a number of elements that disagrees with the " +
                "the number of members as indicated by the Struct's TypeCode");
        }
        return dynStruct.to_any();

    }

    /**
     * Returns the value of a valuetype element as an Any object.
     *
     * @param dynAny Instance of a DynAny object.
     * @param elem Current element in the XML file.
     * @param tc TypeCode of the current element.
     * @return Value of the valuetype element.
     * @exception com.thalesgroup.CdmwXML.DOMException
     * if an error occurs during reading the data.
     */
    private Any getValuetypeValue(
        org.omg.DynamicAny.DynAny dynAny, Element elem, TypeCode tc)
        throws com.thalesgroup.CdmwXML.DOMException {

        //org.omg.DynamicAny.DynStruct dynStruct =
        //    org.omg.DynamicAny.DynStructHelper.narrow(dynAny);


        org.omg.CORBA.Contained c = null;
        try {
            c = interfaceRep.lookup_id(tc.id());
        } catch(org.omg.CORBA.TypeCodePackage.BadKind bk) {
            // should never happen because this method is called
            // only when the property type is a valuetype
            throw new org.omg.CORBA.BAD_PARAM("Attempt to invoke "
                + "a method on an inappropriate kind of TypeCode object");
        }
        org.omg.CORBA.ValueDef valueDef
            = org.omg.CORBA.ValueDefHelper.narrow(c);
        org.omg.CORBA.ValueMember[] valueMembers
            = valueDef.describe_value().members;
        org.omg.DynamicAny.DynValue dynValue =
            org.omg.DynamicAny.DynValueHelper.narrow(dynAny);
        Element[] elems = getChildProperties(elem);
        org.omg.DynamicAny.NameValuePair[] members
            = new org.omg.DynamicAny.NameValuePair[elems.length];
        for(int i=0; i<elems.length; i++) {
            String name = getName(elems[i]);
            TypeCode type = getValueMemberType(name, valueMembers);
            members[i] = new org.omg.DynamicAny.NameValuePair(
                name, getValue(elems[i], type));
        }
        try {
            dynValue.set_members((org.omg.DynamicAny.NameValuePair[]) members);
            //dynStruct.set_members((org.omg.DynamicAny.NameValuePair[]) members);
        } catch(org.omg.DynamicAny.DynAnyPackage.TypeMismatch tm) {
            throw new com.thalesgroup.CdmwXML.DOMException((short)0,
                "the member names do not match the corresponding member name" +
                " in the ValueType's TypeCode and they are not empty strings");
        } catch(org.omg.DynamicAny.DynAnyPackage.InvalidValue iv) {
            throw new com.thalesgroup.CdmwXML.DOMException((short)0,
                "values have a number of elements that disagrees with the " +
                "number of members as indicated by the ValueType's TypeCode");
        }
        return dynValue.to_any();
        //return dynStruct.to_any();

    }

    /**
     * Returns the TypeCode of the member of the struct
     * whose name is given in parameter.
     *
     * @param tc TypeCode of the struct
     * @param name Name of the member to retrieve
     * @return The TypeCode of the member to retrieve
     * @exception com.thalesgroup.CdmwXML.DOMException
     * if the wanted member does not exist in the struct
     */
    private TypeCode getMemberType(TypeCode tc, String name)
        throws com.thalesgroup.CdmwXML.DOMException {

        try {
            for(int i=0; i<tc.member_count(); i++) {
                if (tc.member_name(i).equals(name)) {
                    return tc.member_type(i);
                }
            }
        } catch(org.omg.CORBA.TypeCodePackage.BadKind bk) {
            // should never happen because this method is called
            // only when the property type is a struct
            throw new org.omg.CORBA.BAD_PARAM("Attempt to invoke a method on" +
                " an inappropriate kind of TypeCode object");
        } catch(org.omg.CORBA.TypeCodePackage.Bounds b) {
            // should never happen because the for statement is limited
            // by the member count of the struct
            throw new org.omg.CORBA.BAD_PARAM(
                "Attempt to reach a struct member with an index equal to " +
                "or greater than the number of members constituting the type");
        }
        throw new com.thalesgroup.CdmwXML.DOMException((short)0,
            name + " is not a valid member of the struct");

    }

    /**
     * Returns the TypeCode of the member of the valuetype
     * whose name is given in parameter.
     *
     * @param name Name of the member of the valuetype to retrieve
     * @param valueMembers Description of the members of the valuetype
     * @return The TypeCode of the member to retrieve
     * @exception com.thalesgroup.CdmwXML.DOMException
     * if the member to retrieve does not exist in the valuetype
     */
    private TypeCode getValueMemberType(
        String name, org.omg.CORBA.ValueMember[] valueMembers)
        throws com.thalesgroup.CdmwXML.DOMException {

        for(int i=0; i<valueMembers.length; i++) {
            if (valueMembers[i].name.equals(name)) {
                return valueMembers[i].type;
            }
        }
        throw new com.thalesgroup.CdmwXML.DOMException((short)0,
            name + " is not a valid member of the valuetype");

    }

    /**
     * Returns the value of a simple element.
     *
     * @param elem Reference to an DOM element.
     * @return The value of the simple element.
     * An empty string is returned by default.
     */
    private String getChildValue(Element elem) {

        String result = "";
        Element valueElem = getChildElement(elem, "value");
        if (valueElem != null) {
            com.thalesgroup.CdmwXML.TextDescription[] texts
                = valueElem.get_texts();
            for (int i=0; i<texts.length; i++) {
                result += XMLHelper.toString(texts[i].data());
            }
        }
        return result;

    }

    /**
     * Returns the type of an element
     *
     * @param elem Reference to an DOM element.
     * @exception com.thalesgroup.CdmwXML.DOMException
     * if the element has no attribute with this name.
     * @return The type of an element.
     */
    private String getType(Element elem)
        throws com.thalesgroup.CdmwXML.DOMException {

        com.thalesgroup.CdmwXML.AttrDescription typeAttrDesc
            = elem.get_attribute(XMLHelper.toShortArray("type"));
        return XMLHelper.toString(typeAttrDesc.value());

    }

    /**
     * Returns the name of an element
     *
     * @param elem Reference to a DOM element.
     * @return The name of the element. By default an empty string is returned.
     */
    private String getName(Element elem) {

        com.thalesgroup.CdmwXML.AttrDescription[] attrs
            = elem.get_attributes();
        for(int i=0; i<attrs.length; i++) {
            if (XMLHelper.toString(attrs[i].name()).equals("name")) {
                return XMLHelper.toString(attrs[i].value());
            }
        }
        return "";

    }

    /**
     * Returns the type of the property
     *
     * @param elem Reference to a DOM element.
     * @return The type of the property.
     */
    private String getPropertyType(Element elem) {
        return XMLHelper.toString(
            ((ElementDescription) elem.get_description()).name());
    }

    /**
     * Returns the child elements of the element passed as parameter
     * except the description element if it is present.
     *
     * @param elem Reference to a DOM element.
     * @return An array containing the child elements except
     * the description one.
     * An array with a length of 0 is returned when there is no child element.
     */
    private Element[] getChildProperties(Element elem) {

        ElementDescription[] elemDescs = elem.get_child_elements();
        Element[] elems = null;
        if (elemDescs.length > 0) {
            if (XMLHelper.toString(elemDescs[0].name()).equals("description")) {
                elems = new Element[elemDescs.length-1];
                for(int i=0; i<elems.length; i++) {
                    elems[i] = ElementHelper.narrow(elemDescs[i+1].reference());
                }
            } else {
                elems = new Element[elemDescs.length];
                for(int i=0; i<elems.length; i++) {
                    elems[i] = ElementHelper.narrow(elemDescs[i].reference());
                }
            }
        } else {
            elems = new Element[0];
        }
        return elems;

    }

    /**
     * Returns the child element whose name is given in parameter.
     *
     * @param elem Reference to a DOM element.
     * @param name Name of the element to retrieve
     * @return The element with the wanted name if it exists, null otherwise.
     */
    private Element getChildElement(Element elem, String name) {

        ElementDescription[] elems = elem.get_child_elements();
        for (int i=0; i<elems.length; i++) {
            if (XMLHelper.toString(elems[i].name()).equals(name)) {
                return ElementHelper.narrow(elems[i].reference());
            }
        }
        return null;

    }

}

