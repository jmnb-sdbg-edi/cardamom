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


package cdmw.codegenerator;

/**
 * This class translates CORBA types into XMI.
 */
public class XMITranslator {
    /**
     * Default constructor.
     */
    public XMITranslator() {
    }

    /**
     * This method returns the XMI string for a given TypeRef object.
     *
     * @param typeref The TypeRef object.
     */
    public java.lang.String toXMI(org.objectweb.ccm.IDL3.TypeRef typeref) {
        switch(typeref.getTypeKind()) {
            case org.objectweb.ccm.IDL3.TypeKind._tk_null :
                return "null";
            case org.objectweb.ccm.IDL3.TypeKind._tk_void :
                return "void";
            case org.objectweb.ccm.IDL3.TypeKind._tk_short :
                return "CORBAshort";
            case org.objectweb.ccm.IDL3.TypeKind._tk_ushort :
                return "CORBAunsignedshort";
            case org.objectweb.ccm.IDL3.TypeKind._tk_long :
                return "CORBAlong";
            case org.objectweb.ccm.IDL3.TypeKind._tk_ulong :
                return "CORBAunsignedlong";
            case org.objectweb.ccm.IDL3.TypeKind._tk_longlong :
                return "CORBAlonglong";
            case org.objectweb.ccm.IDL3.TypeKind._tk_ulonglong :
                return "CORBAunsignedlonglong";
            case org.objectweb.ccm.IDL3.TypeKind._tk_float :
                return "CORBAfloat";
            case org.objectweb.ccm.IDL3.TypeKind._tk_double :
                return "CORBAdouble";
            case org.objectweb.ccm.IDL3.TypeKind._tk_longdouble :
                return "CORBAlongdouble";
            case org.objectweb.ccm.IDL3.TypeKind._tk_boolean :
                return "CORBAboolean";
            case org.objectweb.ccm.IDL3.TypeKind._tk_char :
                return "CORBAchar";
            case org.objectweb.ccm.IDL3.TypeKind._tk_wchar :
                return "CORBAwchar";
            case org.objectweb.ccm.IDL3.TypeKind._tk_octet :
                return "CORBAoctet";
            case org.objectweb.ccm.IDL3.TypeKind._tk_any :
                return "CORBAany";
            case org.objectweb.ccm.IDL3.TypeKind._tk_Principal :
                return "CORBAPrincipal";
            case org.objectweb.ccm.IDL3.TypeKind._tk_TypeCode :
                return "CORBATypeCode";
            case org.objectweb.ccm.IDL3.TypeKind._tk_objref :
                return "CORBAObject";

            case org.objectweb.ccm.IDL3.TypeKind._tk_alias :
            case org.objectweb.ccm.IDL3.TypeKind._tk_struct :
            case org.objectweb.ccm.IDL3.TypeKind._tk_union :
            case org.objectweb.ccm.IDL3.TypeKind._tk_enum :
            case org.objectweb.ccm.IDL3.TypeKind._tk_exception :
            case org.objectweb.ccm.IDL3.TypeKind._tk_value_box :
            case org.objectweb.ccm.IDL3.TypeKind._tk_value :
            case org.objectweb.ccm.IDL3.TypeKind._tk_native :
            case org.objectweb.ccm.IDL3.TypeKind._tk_abstract_interface :
            case org.objectweb.ccm.IDL3.TypeKind._tk_local_interface :
            case org.objectweb.ccm.IDL3.TypeKind._tk_interface :
            case org.objectweb.ccm.IDL3.TypeKind._tk_component :
            case org.objectweb.ccm.IDL3.TypeKind._tk_home :
            case org.objectweb.ccm.IDL3.TypeKind._tk_event :
                org.objectweb.ccm.IDL3.Declaration decl = (org.objectweb.ccm.IDL3.Declaration) typeref;
                return getAbsoluteName(decl);

            case org.objectweb.ccm.IDL3.TypeKind._tk_sequence :
                org.objectweb.ccm.IDL3.TypeRefImpl impl= (org.objectweb.ccm.IDL3.TypeRefImpl) typeref;
                java.lang.String str = toXMI(impl.getContentType());
                int length = impl.getLength();
                if (length == 0)
                    return "CORBAsequence<" + str + ">";
                return "CORBAsequence<" + str + ", " + length + ">";

            case org.objectweb.ccm.IDL3.TypeKind._tk_array :
                impl= (org.objectweb.ccm.IDL3.TypeRefImpl) typeref;
                str = toXMI(impl.getContentType());
                return "CORBAarray<" + str + ", " + impl.getLength() + ">";

            case org.objectweb.ccm.IDL3.TypeKind._tk_string :
                impl= (org.objectweb.ccm.IDL3.TypeRefImpl) typeref;
                length = impl.getLength();
                if (length == 0)
                    return "CORBAstring";
                return "CORBAstring<" + impl.getLength() + ">";
            
            case org.objectweb.ccm.IDL3.TypeKind._tk_wstring :
                impl= (org.objectweb.ccm.IDL3.TypeRefImpl) typeref;
                length = impl.getLength();
                if (length == 0)
                    return "CORBAwstring";
                return "CORBAwstring<" + impl.getLength() + ">";
    
            case org.objectweb.ccm.IDL3.TypeKind._tk_fixed :
                impl= (org.objectweb.ccm.IDL3.TypeRefImpl) typeref;
                return "CORBAfixed<" + impl.getDigits() + ", " + impl.getScale() + ">";

            default :
                return "";
        }
    }
    
    /**
     * This method returns the XMI string for a given TypeRef object.
     *
     * @param typeref The TypeRef object.
     */
    public java.lang.String toXMIString(org.objectweb.ccm.IDL3.TypeRef typeref) {
        switch(typeref.getTypeKind()) {
            case org.objectweb.ccm.IDL3.TypeKind._tk_null :
                return "null";
            case org.objectweb.ccm.IDL3.TypeKind._tk_void :
                return "void";
            case org.objectweb.ccm.IDL3.TypeKind._tk_short :
                return "CORBAshort";
            case org.objectweb.ccm.IDL3.TypeKind._tk_ushort :
                return "CORBAunsignedshort";
            case org.objectweb.ccm.IDL3.TypeKind._tk_long :
                return "CORBAlong";
            case org.objectweb.ccm.IDL3.TypeKind._tk_ulong :
                return "CORBAunsignedlong";
            case org.objectweb.ccm.IDL3.TypeKind._tk_longlong :
                return "CORBAlonglong";
            case org.objectweb.ccm.IDL3.TypeKind._tk_ulonglong :
                return "CORBAunsignedlonglong";
            case org.objectweb.ccm.IDL3.TypeKind._tk_float :
                return "CORBAfloat";
            case org.objectweb.ccm.IDL3.TypeKind._tk_double :
                return "CORBAdouble";
            case org.objectweb.ccm.IDL3.TypeKind._tk_longdouble :
                return "CORBAlongdouble";
            case org.objectweb.ccm.IDL3.TypeKind._tk_boolean :
                return "CORBAboolean";
            case org.objectweb.ccm.IDL3.TypeKind._tk_char :
                return "CORBAchar";
            case org.objectweb.ccm.IDL3.TypeKind._tk_wchar :
                return "CORBAwchar";
            case org.objectweb.ccm.IDL3.TypeKind._tk_octet :
                return "CORBAoctet";
            case org.objectweb.ccm.IDL3.TypeKind._tk_any :
                return "CORBAany";
            case org.objectweb.ccm.IDL3.TypeKind._tk_Principal :
                return "CORBAPrincipal";
            case org.objectweb.ccm.IDL3.TypeKind._tk_TypeCode :
                return "CORBATypeCode";
            case org.objectweb.ccm.IDL3.TypeKind._tk_objref :
                return "CORBAObject";
            case org.objectweb.ccm.IDL3.TypeKind._tk_string :
                return "CORBAstring";
            case org.objectweb.ccm.IDL3.TypeKind._tk_wstring :
                return "CORBAwstring";
            case org.objectweb.ccm.IDL3.TypeKind._tk_fixed :
                return "CORBAFixed";
            case org.objectweb.ccm.IDL3.TypeKind._tk_sequence :
                return "CORBASequence";
            case org.objectweb.ccm.IDL3.TypeKind._tk_array :
                return "CORBAArray";

            case org.objectweb.ccm.IDL3.TypeKind._tk_alias :
            case org.objectweb.ccm.IDL3.TypeKind._tk_struct :
            case org.objectweb.ccm.IDL3.TypeKind._tk_union :
            case org.objectweb.ccm.IDL3.TypeKind._tk_enum :
            case org.objectweb.ccm.IDL3.TypeKind._tk_exception :
            case org.objectweb.ccm.IDL3.TypeKind._tk_value_box :
            case org.objectweb.ccm.IDL3.TypeKind._tk_value :
            case org.objectweb.ccm.IDL3.TypeKind._tk_native :
            case org.objectweb.ccm.IDL3.TypeKind._tk_abstract_interface :
            case org.objectweb.ccm.IDL3.TypeKind._tk_local_interface :
            case org.objectweb.ccm.IDL3.TypeKind._tk_interface :
            case org.objectweb.ccm.IDL3.TypeKind._tk_component :
            case org.objectweb.ccm.IDL3.TypeKind._tk_home :
            case org.objectweb.ccm.IDL3.TypeKind._tk_event :
                org.objectweb.ccm.IDL3.Declaration decl = (org.objectweb.ccm.IDL3.Declaration) typeref;
                return getAbsoluteName(decl);

            default :
                return "";
        }
    }

    /**
     * This method returns the absolute name of an IDL declaration.
     *
     * @param decl The IDL declaration.
     */
    public java.lang.String getAbsoluteName(org.objectweb.ccm.IDL3.Declaration decl) {
        return decl.getAbsoluteName();
    }

    /**
     * This method returns a string for an ANY value.
     *
     * @param anyvalue The ANY value.
     * @param typeref The TypeRef object.
     */
    public java.lang.String toString(org.objectweb.ccm.IDL3.AnyValue anyvalue,
                                     org.objectweb.ccm.IDL3.TypeRef typeref) {
                                     
        // TODO : add escaped sequencesto strings, eg '\n' => "\\n"
        if (anyvalue.isInteger()) {
            return java.lang.String.valueOf(anyvalue.getAsInteger());
        } else if (anyvalue.isString()) {
            return "\"" + anyvalue.getAsString() + "\"";
        } else if (anyvalue.isWString()) {
            return "L\"" + anyvalue.getAsWString() + "\"";
        } else if (anyvalue.isChar()) {
            return "\'" + java.lang.String.valueOf(anyvalue.getAsChar()) + "\'";
        } else if (anyvalue.isWChar()) {
            return "L\'" + java.lang.String.valueOf(anyvalue.getAsWChar()) + "\'";
        } else if (anyvalue.isFloating()) {
            return java.lang.String.valueOf(anyvalue.getAsFloating());
        } else if (anyvalue.isFixed()) {
            return anyvalue.getAsFixed();
        } else if (anyvalue.isBoolean()) {
            if (anyvalue.getAsBoolean()) {
                return "TRUE";
            }
            
            return "FALSE";
        } else if (anyvalue.isEnum()) {
            // Build enum member absolute name
            java.lang.String abs_name = 
                ((org.objectweb.ccm.IDL3.EnumDecl) typeref).getParent().getAbsoluteName();
                
            return abs_name + "::" + anyvalue.getAsEnum();
        } else  {
            return "";
        }
    }
}


