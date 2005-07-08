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


package cdmw.codegenerator;

/**
 * This class takes a template file, visits the IR and generates a file.
 */
public class XMIPrinter
    extends org.objectweb.ccm.visitorIDL3.common.Printer {

    protected XMITranslator translator_;
    protected org.objectweb.ccm.util.Table stereotypes_;
    protected org.objectweb.ccm.util.Table types_;
    protected org.objectweb.ccm.util.Vector external_refs_;
    protected org.objectweb.ccm.util.Table generalizations_;
    protected org.objectweb.ccm.util.Table dependencies_tree_;
    protected org.objectweb.ccm.util.Table associations_;
    protected org.objectweb.ccm.util.Table assoc_ccm_manages_;
    protected org.objectweb.ccm.util.Table assoc_union_members_;
    protected org.objectweb.ccm.util.Table assoc_typedefs_;
    protected org.objectweb.ccm.util.Table assoc_anon_arrays_;
    protected org.objectweb.ccm.util.Table constants_;
    protected long id_counter_;
    protected java.lang.String root_id_;

    private int depth_ = 0;

    /**
     * Constructor.
     *
     * @param gen The generator to use.
     * @param indent The identation to use.
     *
     * @exception fr.lifl.cim.jidlscript.lang.JISError
     */
    public XMIPrinter(org.objectweb.ccm.visitorIDL3.common.Generator gen,
                      java.lang.String indent)
        throws fr.lifl.cim.jidlscript.lang.JISError {

        super(gen, '$', '#', indent);

        // Init internal state.
        translator_ = new XMITranslator();
        id_counter_ = java.lang.System.currentTimeMillis();
        external_refs_ = new org.objectweb.ccm.util.Vector();
        dependencies_tree_ = new org.objectweb.ccm.util.Table();
        generalizations_ = new org.objectweb.ccm.util.Table();
        associations_ = new org.objectweb.ccm.util.Table();
        assoc_ccm_manages_ = new org.objectweb.ccm.util.Table();
        assoc_union_members_ = new org.objectweb.ccm.util.Table();
        assoc_typedefs_ = new org.objectweb.ccm.util.Table();
        assoc_anon_arrays_ = new org.objectweb.ccm.util.Table();
        constants_ = new org.objectweb.ccm.util.Table();
        initStereotypes();
        initTypes();

        // Set the "self" alias in root scope on this object.
        super.setAlias("self", this);
        
        // Load template
        super.loadFile("xmi.template");
    }

    /**
     * This method prints a string to the standard error 
     * output stream with indents.
     *
     * @param msg The string to print out.
     */
   private void log(java.lang.String msg) {
        java.lang.String tab = "";
        
        for (int i = 0; i < depth_; i++) {
            tab = tab + "	  ";
        }

        System.err.println(tab + msg);
    }

    /**
     * This methid increments the depth of indents.
     */
    private void incDepth() {
        depth_++;
    }

    /**
     * Decrement depth for indents. 
     */
    private void decDepth() {
        depth_--;
    }

    /**
     * This method initializes the stereotypes.
     */
    protected void initStereotypes() {
        stereotypes_ = new org.objectweb.ccm.util.Table();

        // packages
        stereotypes_.put("CORBAModule", "Package");

        // classes
        stereotypes_.put("CORBAAlias", "Class");
        stereotypes_.put("CORBAAnonymousArray", "Class");
        stereotypes_.put("CORBAAnonymousSequence", "Class");
        stereotypes_.put("CORBAArray", "Class");
        stereotypes_.put("CORBABoxedValue", "Class");
        stereotypes_.put("CORBAConstants", "Class");
        stereotypes_.put("CORBAEnum", "Class");
        stereotypes_.put("CORBAException", "Class");
        stereotypes_.put("CORBAInterface", "Class");
        stereotypes_.put("CORBASequence", "Class");
        stereotypes_.put("CORBAStruct", "Class");
        stereotypes_.put("CORBATypedef", "Class");
        stereotypes_.put("CORBAUnion", "Class");
        stereotypes_.put("CORBAValue", "Class");
        stereotypes_.put("CORBAValueFactory", "Class");
        stereotypes_.put("CORBAValueSupports", "Class");

        // attributes
        stereotypes_.put("CORBAConstant", "Attribute");

        // datatypes
        stereotypes_.put("null", "DataType");
        stereotypes_.put("void", "DataType");
        stereotypes_.put("CORBAFixed", "DataType");
        stereotypes_.put("CORBAObject", "DataType");
        stereotypes_.put("CORBAany", "DataType");
        stereotypes_.put("CORBAboolean", "DataType");
        stereotypes_.put("CORBAchar", "DataType");
        stereotypes_.put("CORBAdouble", "DataType");
        stereotypes_.put("CORBAfloat", "DataType");
        stereotypes_.put("CORBAlong", "DataType");
        stereotypes_.put("CORBAlongdouble", "DataType");
        stereotypes_.put("CORBAlonglong", "DataType");
        stereotypes_.put("CORBAoctet", "DataType");
        stereotypes_.put("CORBAshort", "DataType");
        stereotypes_.put("CORBAstring", "DataType");
        stereotypes_.put("CORBAunsignedlong", "DataType");
        stereotypes_.put("CORBAunsignedlonglong", "DataType");
        stereotypes_.put("CORBAunsignedshort", "DataType");
        stereotypes_.put("CORBAwchar", "DataType");
        stereotypes_.put("CORBAwstring", "DataType");

        // CCM Specific

        // classes
        stereotypes_.put("CCMComponent", "Class");
        stereotypes_.put("CCMHome", "Class");
        stereotypes_.put("CCMEvent", "Class");
        stereotypes_.put("CCMPrimaryKey", "Class");

        // operations
        stereotypes_.put("CCMHomeFactory", "Class");
        stereotypes_.put("CCMHomeFinder", "Class");

        // associations
        stereotypes_.put("CCMManages", "Association");
        stereotypes_.put("CCMProvides", "Association");
        stereotypes_.put("CCMUses", "Association");
        stereotypes_.put("CCMPublishes", "Association");
        stereotypes_.put("CCMConsumes", "Association");
        stereotypes_.put("CCMEmits", "Association");

        // 
        stereotypes_.put("readonly", "null");
        stereotypes_.put("readonlyEnd", "null");

        //
        stereotypes_.put("CORBATypeCode", "DataType");
        stereotypes_.put("CORBAPrincipal", "DataType");
    }

    /**
     * This method creates a new xmi.id for each stereotype.
     */
    protected void initTypes() {
        types_ = new org.objectweb.ccm.util.Table();

        java.util.Enumeration keys = stereotypes_.keys();

        while (keys.hasMoreElements()) {
            types_.put(keys.nextElement(), newId());
        }
    }

    /**
     * This method returns an xmi.id for a declaration
     * considered as an external reference.
     *
     * @param decl The declaration to visit.
     */
    protected java.lang.String getExternalId(
        org.objectweb.ccm.IDL3.Declaration decl) {
        
        java.lang.String id = newId(decl);

        org.objectweb.ccm.IDL3.Declaration[] depend = null;

        // Get the first non module scope that contains the target declaration.
        // The reason is that only modules get have their content generated partially.
        // For example, we can't generate only one operation in an interface ; the interface
        // must be fully generated. But we can generate only one interface in a module as modules
        // can be reopened. So, dependency check is done on the first non module scope.
        org.objectweb.ccm.IDL3.Declaration decl2 = decl;
        while ((decl2.getParent().getDeclKind() != org.objectweb.ccm.IDL3.DeclarationKind._dk_module) &&
               (decl2.getParent().getDeclKind() != org.objectweb.ccm.IDL3.DeclarationKind._dk_repository)) {
            
            decl2 = decl2.getParent();
        }

        if (decl == decl2) {
            external_refs_.add(decl);
        } else if (!types_.containsKey(decl2.getId())) {
            external_refs_.add(decl2);
            newId(decl2);
        } else {
            return id;
        }

        depend = decl2.getDependencies();

        for (int i = 0; i < depend.length; i++) {
            if (!types_.containsKey(depend[i].getId())) {
                getExternalId(depend[i]);
            }
        }

        return id;
    }

    /**
     * This method returns a reference to the XMI translator.
     */
    public XMITranslator getXMITranslator() {
        return translator_;
    }

    /**
     * Modify strings in the template to :
     * <ul>
     *   <li>increase indents,
     *   <li>decrease indents,
     *   <li>add carriage return.
     * </ul>
     *
     * @param str The string to modify.
     */
    public java.lang.String modifyCurrentLine(java.lang.String str) {
        int l = str.length();

        if (l == 0) {
            return "";
        }

        if ((l > 2) && (str.charAt(0) == '<') &&
            ((str.charAt(1) == '!') || (str.charAt(1) == '?'))) {
            
            return str + "\\n";
        }

        if ((l > 2) &&
            (str.charAt(l - 1) == '>') &&
            (str.charAt(l - 2) != '/') &&
            (str.indexOf("</") == -1)) {
            
            return str + "\\n$self.inc()$";
        }

        if ((l > 2) && (str.charAt(0) == '<') &&
            (str.charAt(1) == '/')) {
            
            return "$self.dec()$" + str + "\\n";
        }

        if ((str.charAt(0) != getCallChar()) ||
            (str.charAt(l - 1) != getCallChar())) {
            
            return str+"\\n";
        }

        return str;
    }

    /**
     * This method generates a new xmi.id.
     */
    public java.lang.String newId() {
        id_counter_++;

        return "xmi." + java.lang.Long.toString(id_counter_);
    }

    /**
     * This method creates a new xmi.id only if the 
     * Declartion is a new one and stores the newly 
     * created xmi.id in a hashtable.
     */
    public java.lang.String newId(org.objectweb.ccm.IDL3.Declaration decl) {
        java.lang.String res = (java.lang.String) types_.get(decl.getId());

        if (res != null) {
            return res;
        }

        res = newId();
        types_.put(decl.getId(), res);

        return res;
    }

    /**
     * This method generates a new root xmi.id.
     */
    public java.lang.String newRootId() {
        root_id_ = newId();

        return root_id_;
    }

    /**
     * This method returns the xmi.id of the given stereotype name.
     *
     * @param stereo_name The name of the stereotype.
     */
    public java.lang.String getXMIId(java.lang.String stereo_name) {
        java.lang.String res = (java.lang.String) types_.get(stereo_name);

        if (res == null) {
            res = "";
        }

        return res;
    }

    /**
     * This method returns the xmi.id of the given type.
     *
     * @param type The TypeRef object.
     */
    public java.lang.String getXMIId(org.objectweb.ccm.IDL3.TypeRef type) {
        if (type.isDeclaration()) {
            org.objectweb.ccm.IDL3.Declaration decl =
                (org.objectweb.ccm.IDL3.Declaration) type;

            java.lang.String res = getXMIId(decl.getId());

            if (res.equals("")) {
                res = getExternalId(decl);
            }

            return res;
        }

        return getXMIId(getXMITranslator().toXMIString(type));
    }

    /**
     * This method returns 'true' if the CORBA value 
     * is abstract and 'false' otherwise.
     *
     * @param value The ValueType object.
     */
    public java.lang.String isAbstract(org.objectweb.ccm.IDL3.ValueDecl value) {
        if (value.isAbstract()) {
            return "true";
        } else {
            return "false";
        }
    }

    /**
     * This method returns 'in' if the parameter mode is PARAM_IN,
     * 'out' if PARAM_OUT and 'inout' otherwise.
     *
     * @param mode The parameter kind.
     */
    public java.lang.String toStringMode(int mode) {
        if (mode == org.objectweb.ccm.IDL3.OperationDecl.PARAM_IN) {
            return "in";
        } else if (mode == org.objectweb.ccm.IDL3.OperationDecl.PARAM_OUT) {
            return "out";
        } else {
            return "inout";
        }
    }

    /**
     * This method generates the XMI mapping for a stereotype.
     */
    public java.lang.String corbaStereotypes() {
        java.util.Enumeration keys = stereotypes_.keys();
        java.util.Enumeration values = stereotypes_.elements();

        java.lang.String[] aliases = { "name", "base_class" };
        java.lang.Object[] objs = new java.lang.Object[aliases.length];

        while (keys.hasMoreElements()) {
            objs[0] = keys.nextElement();
            objs[1] = values.nextElement();
            super.map("STEREOTYPE", aliases, objs);
        }

        return "";
    }

    /**
     * This method inserts XMI generalizable element tags.
     *
     * @param gen_id The generalization xmi.id.
     */
    public java.lang.String insertGeneralizableElement(java.lang.String gen_id) {
        java.lang.String[] aliases = { "gen_id" };
        java.lang.Object[] objs = { gen_id };

        super.map("GENERALIZABLE_ELEMENT", aliases, objs);

        return "";
    }

    /**
     * This method generates XMI mapping for all the generalizations found.
     */
    public java.lang.String generalizations() {
        java.util.Enumeration keys = generalizations_.keys();
        java.util.Enumeration values = generalizations_.elements();

        java.lang.String[] aliases = { "gen_id", "child_id", "parent_id" };
        java.lang.Object[] objs = new java.lang.Object[3];

        while (keys.hasMoreElements()) {
            objs[1] = getXMIId((java.lang.String) keys.nextElement());
            org.objectweb.ccm.util.Vector[] elems = 
                (org.objectweb.ccm.util.Vector[]) values.nextElement();

            for (int i = 0; i < elems.length; i++) {
                objs[0] = elems[i].get(0);
                objs[2] = elems[i].get(1);
                super.map("GENERALIZATION", aliases, objs);
            }
        }

        return "";
    }

    /**
     * This method generates XMI mapping for all the associations found.
     */
    public java.lang.String associations() {
        java.util.Enumeration keys = associations_.keys();
        java.util.Enumeration values = associations_.elements();

        java.lang.String[] aliases = { "assoc_id", "src_elem_ref", "dest_elem_ref", "name", 
            "stereotype", "visibility", "multiplicity_src", "multiplicity_dest" };
        java.lang.Object[] objs = new java.lang.Object[aliases.length];

        while (keys.hasMoreElements()) {
            objs[4] = "";
            keys.nextElement();
            org.objectweb.ccm.util.Vector[] elems = 
                (org.objectweb.ccm.util.Vector[]) values.nextElement();

            for (int i = 0; i < elems.length; i++) {
                for (int j = 0; j < elems[i].size(); j++) {
                    objs[j] = elems[i].get(j);
                }

                super.map("ASSOCIATION", aliases, objs);
            }
        }

        return "";
    }

    /**
     * This method generates XMI mapping for all the associations found.
     */
    public java.lang.String associations_ccm_manages() {
        java.util.Enumeration keys = assoc_ccm_manages_.keys();
        java.util.Enumeration values = assoc_ccm_manages_.elements();

        java.lang.String[] aliases = { "assoc_id", "home_ref", "component_ref", "pk_ref" };
        java.lang.Object[] objs = new java.lang.Object[aliases.length];

        // Init objs
        for (int i = 0; i < objs.length; i++) {
            objs[i] = "";
        }

        while (keys.hasMoreElements()) {
            keys.nextElement();
            org.objectweb.ccm.util.Vector[] elems = 
                (org.objectweb.ccm.util.Vector[]) values.nextElement();

            for (int i = 0; i < elems.length; i++) {
                for (int j = 0; j < elems[i].size(); j++) {
                    objs[j] = elems[i].get(j);
                }

                if (elems[i].size() == 3) {
                    super.map("ASSOCIATION_CCM_MANAGES", aliases, objs);
                } else {
                    super.map("ASSOCIATION_CCM_MANAGES_WITH_PK", aliases, objs);
                }
            }
        }

        return "";
    }

    /**
     * This method generates XMI mapping for all the associations found.
     */
    public java.lang.String associations_union_members() {
        java.util.Enumeration keys = assoc_union_members_.keys();
        java.util.Enumeration values = assoc_union_members_.elements();

        java.lang.String[] aliases = { "assoc_id", "name", "src_elem_ref", 
            "dest_elem_ref", "idl_order", "label" };
        java.lang.Object[] objs = new java.lang.Object[aliases.length];

        while (keys.hasMoreElements()) {
            keys.nextElement();
            org.objectweb.ccm.util.Vector[] elems = 
                (org.objectweb.ccm.util.Vector[]) values.nextElement();

            for (int i = 0; i < elems.length; i++) {
                for (int j = 0; j < elems[i].size(); j++) {
                    objs[j] = elems[i].get(j);
                }

                super.map("ASSOCIATION_UNION_MEMBER", aliases, objs);
            }
        }

        return "";
    }

    /**
     * This method generates XMI mapping for all the associations found.
     */
    public java.lang.String associations_typedefs() {
        java.util.Enumeration keys = assoc_typedefs_.keys();
        java.util.Enumeration values = assoc_typedefs_.elements();

        java.lang.String[] aliases = { "assoc_id", "src_elem_ref", 
            "dest_elem_ref", "obj", "multiplicity" };
        java.lang.Object[] objs = new java.lang.Object[aliases.length];

        while (keys.hasMoreElements()) {
            keys.nextElement();
            org.objectweb.ccm.util.Vector[] elems = 
                (org.objectweb.ccm.util.Vector[]) values.nextElement();

            for (int i = 0; i < elems.length; i++) {
                for (int j = 0; j < elems[i].size() - 1; j++) {
                    objs[j] = elems[i].get(j);
                }

                if (((java.lang.String) 
                    elems[i].get(elems[i].size() - 1)).equals("CORBASequence")) {
        
                    super.map("ASSOCIATION_SEQUENCE", aliases, objs);
                } else {
                    super.map("ASSOCIATION_ARRAY", aliases, objs);
                }
            }
        }

        return "";
    }

    /**
     * This method generates XMI mapping for all the associations found.
     */
    public java.lang.String associations_anon_arrays() {
        java.util.Enumeration keys = assoc_anon_arrays_.keys();
        java.util.Enumeration values = assoc_anon_arrays_.elements();

        java.lang.String[] aliases = { "assoc_id", "src_elem_ref", 
            "dest_elem_ref", "obj" };
        java.lang.Object[] objs = new java.lang.Object[aliases.length];

        while (keys.hasMoreElements()) {
            keys.nextElement();
            org.objectweb.ccm.util.Vector[] elems = 
                (org.objectweb.ccm.util.Vector[]) values.nextElement();

            for (int i = 0; i < elems.length; i++) {
                for (int j = 0; j < elems[i].size(); j++) {
                    objs[j] = elems[i].get(j);
                }

                super.map("ASSOCIATION_ANONYMOUS_ARRAY", aliases, objs);
            }
        }

        return "";
    }

    /**
     * This method returns either the namespace or 
     * the owner xmi.id for a given element.
     *
     * @param decl The declaration to visit.
     */
    public java.lang.String getScopeId(org.objectweb.ccm.IDL3.Declaration decl) {
        java.lang.String id = "";
        id = getXMIId(decl.getParent().getId());

        if (decl.getParent().getDeclKind() == 
            org.objectweb.ccm.IDL3.DeclarationKind._dk_repository) {

            return root_id_;
        } else {
            return id;
        }
    }

    /**
     * This method maps "obj" to the module.
     *
     * @param decl The declaration to visit.
     */
    public java.lang.String visitModule(org.objectweb.ccm.IDL3.Declaration decl) {
        if (decl.getDeclKind() != org.objectweb.ccm.IDL3.DeclarationKind._dk_module) {
            java.lang.String msg =
            "error in class org.objectweb.ccm.visitorIDL3.XMIPrinter\n" +
            "   operation : visitModule\n" +
            "   message : root visited object must be a module";
            throw new Error(msg);
        }

        java.lang.String[] aliases = { "obj" };
        java.lang.Object[] objs = { decl };
        super.map("MODULE", aliases, objs);

        return "";
    }

    /**
     * This method visits all the contained elements of a given scope.
     *
     * @param scope The scope to visit.
     */
    public java.lang.String contents(org.objectweb.ccm.IDL3.Scope scope) {
        org.objectweb.ccm.IDL3.Declaration[] decls = null;

        decls = scope.getContents(true, org.objectweb.ccm.IDL3.DeclarationKind._dk_all);

        for (int i = 0; i < decls.length; i++) {
            visitContained(decls[i], "");
        }

        return "";
    }

    /**
     * This method finds the corresponding mapping for a contained element and runs it.
     *
     * @param contained The declaration to visit.
     * @param extension The string id extension.
     */
    public java.lang.String visitContained(org.objectweb.ccm.IDL3.Declaration contained,
                                           java.lang.String extension) {
    
        java.lang.String id =
            org.objectweb.ccm.IDL3.DeclarationKind.toString(
                contained.getDeclKind()).toUpperCase() +  extension;

        java.lang.String[] aliases = { "obj" };
        java.lang.Object[] objs = { contained };
        super.map(id, aliases, objs);

        return "";
    }

    /**
     * This method gets the base component and 
     * the supported interfaces of a given component.
     *
     * @param component The component to visit.
     */
    public java.lang.String baseComponent(
        org.objectweb.ccm.IDL3.ComponentDecl component) {
        
        org.objectweb.ccm.IDL3.InterfaceDecl[] supportedInterfaces = null;
        supportedInterfaces = 
            ((org.objectweb.ccm.IDL3.ComponentDecl) component).getSupportedInterfaces();

        java.lang.String gen_id = "";
        org.objectweb.ccm.util.Vector gen_bases = new org.objectweb.ccm.util.Vector();

        for (int i = 0; i < supportedInterfaces.length; i++) {
            gen_id = newId();
            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(gen_id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) supportedInterfaces[i]));
            gen_bases.add(ids);

            insertGeneralizableElement(gen_id);
        }

        org.objectweb.ccm.IDL3.ComponentDecl baseComponent = null;
        baseComponent = component.getBaseComponent();

        if (baseComponent != null) {
            gen_id = newId();

            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(gen_id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) baseComponent));
            gen_bases.add(ids);

            insertGeneralizableElement(gen_id);
        }

        generalizations_.put(component.getId(), 
            (org.objectweb.ccm.util.Vector[]) 
                gen_bases.toArray(new org.objectweb.ccm.util.Vector[0]));

        return "";
    }

    /**
     * This method gets the base home and 
     * the supported interfaces of a given home.
     *
     * @param home The home to visit.
     */
    public java.lang.String baseHome(org.objectweb.ccm.IDL3.HomeDecl home) {
        org.objectweb.ccm.IDL3.InterfaceDecl[] supportedInterfaces = null;
        supportedInterfaces = 
            ((org.objectweb.ccm.IDL3.HomeDecl) home).getSupportedInterfaces();

        java.lang.String gen_id = "";
        org.objectweb.ccm.util.Vector gen_bases = new org.objectweb.ccm.util.Vector();

        for (int i = 0; i < supportedInterfaces.length; i++) {
            gen_id = newId();
            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(gen_id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) supportedInterfaces[i]));
            gen_bases.add(ids);

            insertGeneralizableElement(gen_id);
        }

        org.objectweb.ccm.IDL3.HomeDecl baseHome = null;
        baseHome = home.getBaseHome();

        if (baseHome != null) {
            gen_id = newId();
            
            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(gen_id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) baseHome));
            gen_bases.add(ids);

            insertGeneralizableElement(gen_id);
        }

        generalizations_.put(home.getId(), 
            (org.objectweb.ccm.util.Vector[]) 
                gen_bases.toArray(new org.objectweb.ccm.util.Vector[0]));

        return "";
    }

    /**
     * This component visits the managed component and the primary key.
     *
     * @param home The home to visit.
     */
    public java.lang.String managedComponent(org.objectweb.ccm.IDL3.HomeDecl home) {
        org.objectweb.ccm.IDL3.ComponentDecl component = null;
        component = home.getManagedComponent();
        org.objectweb.ccm.IDL3.ValueDecl pk = null;
        pk = home.getPrimaryKey();

        // Association
        java.lang.String assoc_id = "";
        org.objectweb.ccm.util.Vector assoc_ends = new org.objectweb.ccm.util.Vector();
            
        assoc_id = newId();

        org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
        ids.add(assoc_id);
        ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) home));
        ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) component));
        
        if (pk != null) {
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) pk));
        }
        assoc_ends.add(ids);

        assoc_ccm_manages_.put(home.getId(),
            (org.objectweb.ccm.util.Vector[]) 
                assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));

        return "";
    }


    /**
     * This method gets the base interfaces of a given interface.
     *
     * @param itf The interface to visit.
     */
    public java.lang.String baseInterfaces(org.objectweb.ccm.IDL3.InterfaceDecl itf) {
        org.objectweb.ccm.IDL3.InterfaceDecl[] bases = null;
        bases = itf.getBaseInterfaces();

        // Generalizable element
        java.lang.String gen_id = "";
        org.objectweb.ccm.util.Vector gen_bases = new org.objectweb.ccm.util.Vector();

        for (int i = 0; i < bases.length; i++) {
            gen_id = newId();
            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(gen_id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) bases[i]));
            gen_bases.add(ids);
            
            insertGeneralizableElement(gen_id);
        }

        generalizations_.put(itf.getId(),
            (org.objectweb.ccm.util.Vector[]) 
                gen_bases.toArray(new org.objectweb.ccm.util.Vector[0]));

        return "";
    }

    /**
     * This method defines a new association when a facet is visited.
     *
     * @param _provides The facet to visit.
     */
    public java.lang.String provides(org.objectweb.ccm.IDL3.ProvidesDecl _provides) {
        org.objectweb.ccm.IDL3.TypeRef type = null;
        type = _provides.getType();

        if (type != null) {
            // Association
            java.lang.String assoc_id = "";
            org.objectweb.ccm.util.Vector assoc_ends = new org.objectweb.ccm.util.Vector();
            
            assoc_id = newId();

            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(assoc_id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) _provides.getParent()));
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) type));
            ids.add(_provides.getName());
            ids.add("stereotype='" + getXMIId("CCMProvides") + "'");
            ids.add("public");
            ids.add("1");
            ids.add("1");
            assoc_ends.add(ids);

            associations_.put(_provides.getId(),
                (org.objectweb.ccm.util.Vector[]) 
                    assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));
        }

        return "";
    }

    /**
     * This method defines a new association when a receptacle is visited.
     *
     * @param _uses The receptacle to visit.
     */
    public java.lang.String uses(org.objectweb.ccm.IDL3.UsesDecl _uses) {
        org.objectweb.ccm.IDL3.TypeRef type = null;
        type = _uses.getType();

        if (type != null) {
            // Association
            java.lang.String assoc_id = "";
            org.objectweb.ccm.util.Vector assoc_ends = new org.objectweb.ccm.util.Vector();
            
            assoc_id = newId();

            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(assoc_id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) _uses.getParent()));
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) type));
            ids.add(_uses.getName());
            ids.add("stereotype='" + getXMIId("CCMUses") + "'");
            ids.add("public");
            ids.add("1");
            ids.add(_uses.isMultiple() ? "*" : "1");
            assoc_ends.add(ids);

            associations_.put(_uses.getId(),
                (org.objectweb.ccm.util.Vector[]) 
                    assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));
        }

        return "";
    }

    /**
     * This method defines a new association when an event is visited.
     *
     * @param _publishes The event to visit.
     */
    public java.lang.String publishes(org.objectweb.ccm.IDL3.PublishesDecl _publishes) {
        org.objectweb.ccm.IDL3.TypeRef type = null;
        type = _publishes.getType();

        if (type != null) {
            // Association
            java.lang.String assoc_id = "";
            org.objectweb.ccm.util.Vector assoc_ends = new org.objectweb.ccm.util.Vector();
            
            assoc_id = newId();

            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(assoc_id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) _publishes.getParent()));
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) type));
            ids.add(_publishes.getName());
            ids.add("stereotype='" + getXMIId("CCMPublishes") + "'");
            ids.add("public");
            ids.add("1");
            ids.add("1");
            assoc_ends.add(ids);

            associations_.put(_publishes.getId(),
                (org.objectweb.ccm.util.Vector[]) 
                    assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));
        }

        return "";
    }

    /**
     * This method defines a new association when an event is visited.
     *
     * @param _consumes The event to visit.
     */
    public java.lang.String consumes(org.objectweb.ccm.IDL3.ConsumesDecl _consumes) {
        org.objectweb.ccm.IDL3.TypeRef type = null;
        type = _consumes.getType();

        if (type != null) {
            // Association
            java.lang.String assoc_id = "";
            org.objectweb.ccm.util.Vector assoc_ends = new org.objectweb.ccm.util.Vector();
            
            assoc_id = newId();

            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(assoc_id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) _consumes.getParent()));
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) type));
            ids.add(_consumes.getName());
            ids.add("stereotype='" + getXMIId("CCMConsumes") + "'");
            ids.add("public");
            ids.add("1");
            ids.add("1");
            assoc_ends.add(ids);

            associations_.put(_consumes.getId(),
                (org.objectweb.ccm.util.Vector[]) 
                    assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));
        }

        return "";
    }

    /**
     * This method defines a new association when an event is visited.
     *
     * @param _emits The event to visit.
     */
    public java.lang.String emits(org.objectweb.ccm.IDL3.EmitsDecl _emits) {
        org.objectweb.ccm.IDL3.TypeRef type = null;
        type = _emits.getType();

        if (type != null) {
            // Association
            java.lang.String assoc_id = "";
            org.objectweb.ccm.util.Vector assoc_ends = new org.objectweb.ccm.util.Vector();
            
            assoc_id = newId();

            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(assoc_id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) _emits.getParent()));
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) type));
            ids.add(_emits.getName());
            ids.add("stereotype='" + getXMIId("CCMEmits") + "'");
            ids.add("public");
            ids.add("1");
            ids.add("1");
            assoc_ends.add(ids);

            associations_.put(_emits.getId(),
                (org.objectweb.ccm.util.Vector[])
                    assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));
        }

        return "";
    }

    /**
     * This method gets the exceptions raised by an operation.
     *
     * @param op The operation to visit.
     */
    public java.lang.String operationExceptions(org.objectweb.ccm.IDL3.OperationDecl op) {
        org.objectweb.ccm.IDL3.ExceptionDecl[] excs = op.getExceptions();

        java.lang.String[] aliases = {"exception"};
        java.lang.Object[] objs = new java.lang.Object[1];

        for (int i = 0; i < excs.length; i++) {
            objs[0] = excs[i].getAbsoluteName().substring(2);
            super.map("OPERATION_EXCEPTION", aliases, objs);
        }
          
        return "";
    }

    /**
     * This method gets the parameters of an operation.
     *
     * @param op The operation to visit.
     */
    public java.lang.String operationParams(org.objectweb.ccm.IDL3.OperationDecl op) {
        org.objectweb.ccm.IDL3.ParameterList params = op.getParameters();
        org.objectweb.ccm.IDL3.TypeRef[] types = params.getParamTypes();		
        java.lang.String[] names = params.getParamNames();
        int[] modes = params.getParamModes();

        java.lang.String[] aliases = {"name", "kind", "operation", "type"};
        java.lang.Object[] objs = new java.lang.Object[4];
        objs[2] = getXMIId(op.getId());

        for (int i = 0; i < names.length; i++) {
            objs[0] = names[i];
            objs[1] = toStringMode(modes[i]);
            objs[3] = getXMIId(types[i]);

            super.map("OPERATION_PARAMETER", aliases, objs);
        }
          
        return "";
    }

    /**
     * This method maps an IDL attribute either to a UML attribute or to a UML association.
     *
     * @param attrib The attribute to visit.
     */
    public java.lang.String attribute(org.objectweb.ccm.IDL3.AttributeDecl attrib) {
        org.objectweb.ccm.IDL3.TypeRef attrib_type = attrib.getType();
        java.lang.String id = "";
        id = newId(attrib);

        if (attrib_type.isDeclaration()) {
            // Association
            org.objectweb.ccm.util.Vector assoc_ends = new org.objectweb.ccm.util.Vector();

            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) attrib.getParent()));
            ids.add(getXMIId(attrib_type));
            ids.add(attrib.getName());
            ids.add(attrib.isReadonly() ? ("stereotype='" + getXMIId("readonlyEnd") + "'") : "");
            ids.add("public");
            ids.add("1");
            ids.add("1");
            assoc_ends.add(ids);

            associations_.put(attrib.getId(),
                (org.objectweb.ccm.util.Vector[]) 
                    assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));
        } else {
            java.lang.String[] aliases = { "attrib_id", "name", "visibility", 
                "multiplicity", "stereotype", "scope_id", "type" };
            java.lang.Object[] objs = new java.lang.Object[aliases.length];

            objs[0] = id;
            objs[1] = attrib.getName();
            objs[2] = "public";
            objs[3] = "";
            objs[4] = attrib.isReadonly() ? ("stereotype='" + getXMIId("readonly") + "'") : "";
            objs[5] = getScopeId(attrib);
            objs[6] = getXMIId(attrib_type);

            super.map("ATTRIBUTE_BASIC_TYPES", aliases, objs);
        }

        return "";
    }

    /**
     * This method generates XMI mappings for the enum members.
     *
     * @param enum The enum to visit.
     */
    public java.lang.String enumMembers(org.objectweb.ccm.IDL3.EnumDecl enum_) {
        java.lang.String[] names = enum_.getMembers();

        java.lang.String[] aliases = {"name", "owner_id", "idl_order"};
        java.lang.Object[] objs = new java.lang.Object[aliases.length];

        objs[1] = getXMIId(enum_.getId());

        for (int i = 0; i < names.length; i++)
        {
            objs[0] = names[i];
            objs[2] = java.lang.Integer.toString(i);
            super.map("ENUM_MEMBER", aliases, objs);
        }

        return "";
    }

    /**
     * This method generates XMI mappings for struct members.
     *
     * @param struct_id The xmi.id of the struct to visit.
     * @param members The struct members.
     */
    public java.lang.String structMembers(java.lang.String struct_id,
                                          org.objectweb.ccm.IDL3.StructMembers members) {
                                          
        org.objectweb.ccm.IDL3.TypeRef[] types = members.getMemberTypes();
        java.lang.String[] names = members.getMemberNames();

        org.objectweb.ccm.util.Vector assoc_ends = new org.objectweb.ccm.util.Vector();
        org.objectweb.ccm.util.Vector anon_arrays_assoc_ends = new org.objectweb.ccm.util.Vector();
        java.lang.String id = "";

        for (int i = 0; i < names.length; i++) {
            boolean is_inner = false;
            boolean is_inner_array = false;
            
            // Check for inner struct, union or enum members
            if ((types[i].getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_struct) ||
                (types[i].getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_union) ||
                (types[i].getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_enum)) {

                org.objectweb.ccm.IDL3.Declaration decl = null;
                decl = (org.objectweb.ccm.IDL3.Declaration) types[i];

                if (decl.getParent().getId().equals(struct_id)) {
                    is_inner = true;
                } else {
                    is_inner = false;
                }
            } else if ((types[i].getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_array) &&
                       !types[i].isDeclaration()) {

                is_inner_array = true;
            } else {
                is_inner = false;
            }

            id = newId();
            
            if (is_inner || is_inner_array) {
                switch (types[i].getTypeKind()) {
                    case org.objectweb.ccm.IDL3.TypeKind._tk_struct :
                        {
                            java.lang.String[] aliases = { "obj" };
                            java.lang.Object[] objs = { (org.objectweb.ccm.IDL3.StructDecl) types[i] };
                            super.map("STRUCT", aliases, objs);
                        }
                        break;
                    case org.objectweb.ccm.IDL3.TypeKind._tk_union :
                        {
                            java.lang.String[] aliases = { "obj" };
                            java.lang.Object[] objs = { (org.objectweb.ccm.IDL3.UnionDecl) types[i] };
                            super.map("UNION", aliases, objs);
                        }
                        break;
                    case org.objectweb.ccm.IDL3.TypeKind._tk_enum :
                        {
                            java.lang.String[] aliases = { "obj" };
                            java.lang.Object[] objs = { (org.objectweb.ccm.IDL3.EnumDecl) types[i] };
                            super.map("ENUM", aliases, objs);
                        }
                        break;
                    case org.objectweb.ccm.IDL3.TypeKind._tk_array :
                        {
                            java.lang.String[] aliases = { "array_id", "scope_id" };
                            java.lang.Object[] objs = { id, getXMIId(struct_id) };
                            super.map("ANONYMOUS_ARRAY", aliases, objs);
                        }
                        break;
                    default :
                        break;
                }
            }

            if (types[i].isDeclaration()) {
                // Association
                org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
                ids.add(id);
                ids.add(getXMIId(struct_id));
                ids.add(getXMIId(types[i]));
                ids.add(names[i]);
                ids.add("");
                ids.add("public");
                ids.add("1");
                ids.add("1");
                assoc_ends.add(ids);
            } else if (is_inner_array) {
                // Associations
                org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
                ids.add(newId());
                ids.add(getXMIId(struct_id));
                ids.add(id);
                ids.add(names[i]);
                ids.add("");
                ids.add("public");
                ids.add("1..1");
                ids.add("1..1");
                assoc_ends.add(ids);
                
                org.objectweb.ccm.util.Vector ids2 = new org.objectweb.ccm.util.Vector();
                org.objectweb.ccm.IDL3.TypeRefImpl impl = (org.objectweb.ccm.IDL3.TypeRefImpl) types[i];
                ids2.add(newId());
                ids2.add(id);

                org.objectweb.ccm.IDL3.TypeRefImpl impl2 = impl;
                while (impl2.getContentType().getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_array) {
                    impl2 = (org.objectweb.ccm.IDL3.TypeRefImpl) impl2.getContentType();
                }
                ids2.add(getXMIId(impl2.getContentType()));
                
                ids2.add(impl);
                anon_arrays_assoc_ends.add(ids2);
            } else {
                // Attribute
                java.lang.String[] aliases = { "attrib_id", "name", "visibility", 
                    "multiplicity", "stereotype", "scope_id", "type" };
                java.lang.Object[] objs = new java.lang.Object[aliases.length];

                objs[0] = id;
                objs[1] = names[i];
                objs[2] = "public";
                objs[3] = "multiplicity='1..1'";
                objs[4] = "";
                objs[5] = getXMIId(struct_id);
                objs[6] = getXMIId(types[i]);

                super.map("ATTRIBUTE_BASIC_TYPES", aliases, objs);
            }
        }

        associations_.put(struct_id,
            (org.objectweb.ccm.util.Vector[]) 
                assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));

        if (!anon_arrays_assoc_ends.isEmpty()) {
            assoc_anon_arrays_.put(struct_id,
                (org.objectweb.ccm.util.Vector[]) 
                    anon_arrays_assoc_ends.toArray(
                        new org.objectweb.ccm.util.Vector[0]));
        }

        return "";
    }

    /**
     * This method generates XMI mappings for union members.
     *
     * @param union The union to visit.
     */
    public java.lang.String unionMembers(org.objectweb.ccm.IDL3.UnionDecl union) {
        boolean is_inner = false;

        // Discriminator
        org.objectweb.ccm.IDL3.TypeRef discriminator = union.getType();

        java.lang.String owner_id = "";
        owner_id = getXMIId((org.objectweb.ccm.IDL3.TypeRef) union);

        java.lang.String id = "";
        id = newId();

        if (discriminator.isDeclaration()) {
            // Association
            org.objectweb.ccm.util.Vector assoc_ends = new org.objectweb.ccm.util.Vector();

            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) union));
            ids.add(getXMIId(discriminator));
            ids.add(getXMITranslator().toXMI(discriminator) + "_switch");
            ids.add("");
            ids.add("public");
            ids.add("1");
            ids.add("1");
            assoc_ends.add(ids);

            associations_.put(union.getId(),
                (org.objectweb.ccm.util.Vector[]) 
                    assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));
        } else {
            java.lang.String[] aliases = { "attrib_id", "name", "visibility", 
                "multiplicity", "stereotype", "scope_id", "type" };
            java.lang.Object[] objs = new java.lang.Object[aliases.length];

            objs[0] = id;
            objs[1] = getXMITranslator().toXMI(discriminator) + "_switch";
            objs[2] = "public";
            objs[3] = "multiplicity='1..1'";
            objs[4] = "";
            objs[5] = owner_id;
            objs[6] = getXMIId(discriminator);

            super.map("ATTRIBUTE_BASIC_TYPES", aliases, objs);
        }

        // Other members
        org.objectweb.ccm.IDL3.TypeRef[] types = union.getMembers().getMemberTypes();
        org.objectweb.ccm.IDL3.AnyValue[] labels = union.getMembers().getMemberValues();
        java.lang.String[] names = union.getMembers().getMemberNames();
        int default_index = union.getDefaultIndex();

        java.lang.String[] aliases = { "attrib_id", "name", "scope_id", 
            "type", "idl_order", "label" };
        java.lang.Object[] objs = new java.lang.Object[aliases.length];

        org.objectweb.ccm.util.Vector assoc_ends = new org.objectweb.ccm.util.Vector();

        for (int i = 0; i < names.length; i++) {
            // Check for inner struct, union or enum members
            if ((types[i].getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_struct) ||
                (types[i].getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_union) ||
                (types[i].getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_enum)) {

                org.objectweb.ccm.IDL3.Declaration decl = null;
                decl = (org.objectweb.ccm.IDL3.Declaration) types[i];

                if (decl.getParent().getId().equals(union.getId())) {
                    is_inner = true;
                } else {
                    is_inner = false;
                }
            } else {
                is_inner = false;
            }

            if (is_inner) {
                switch (types[i].getTypeKind()) {
                    case org.objectweb.ccm.IDL3.TypeKind._tk_struct :
                        {
                            java.lang.String[] aliases_ = { "obj" };
                            java.lang.Object[] objs_ = { (org.objectweb.ccm.IDL3.StructDecl) types[i] };
                            super.map("STRUCT", aliases_, objs_);
                        }
                        break;
                    case org.objectweb.ccm.IDL3.TypeKind._tk_union :
                        {
                            java.lang.String[] aliases_ = { "obj" };
                            java.lang.Object[] objs_ = { (org.objectweb.ccm.IDL3.UnionDecl) types[i] };
                            super.map("UNION", aliases_, objs_);
                        }
                        break;
                    case org.objectweb.ccm.IDL3.TypeKind._tk_enum :
                        {
                            java.lang.String[] aliases_ = { "obj" };
                            java.lang.Object[] objs_ = { (org.objectweb.ccm.IDL3.EnumDecl) types[i] };
                            super.map("ENUM", aliases_, objs_);
                        }
                        break;
                    default :
                        break;
                }
            }

            id = newId();
            
            if (types[i].isDeclaration()) {
                // Association
                org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
                ids.add(id);
                ids.add(names[i]);
                ids.add(owner_id);
                ids.add(getXMIId(types[i]));
                ids.add(java.lang.Integer.toString(i));
                if (default_index == i) {
                    ids.add("default");
                } else {
                    ids.add(getXMITranslator().toString(labels[i], discriminator));
                }
                assoc_ends.add(ids);
            } else {
                objs[0] = id;
                objs[1] = names[i];
                objs[2] = owner_id;
                objs[3] = getXMIId(types[i]);
                objs[4] = java.lang.Integer.toString(i);
                if (default_index == i) {
                    objs[5] = "default";
                } else {
                    objs[5] = getXMITranslator().toString(labels[i], discriminator);
                }
                super.map("ATTRIBUTE_UNION_MEMBER", aliases, objs);
            }
        }

        assoc_union_members_.put(union.getId(),
            (org.objectweb.ccm.util.Vector[]) 
                assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));

        return "";
    }

    /**
     * This method gets the base values of an eventtype or a CORBA value.
     *
     * @param value The value type to visit.
     */
    public java.lang.String baseValues(org.objectweb.ccm.IDL3.ValueDecl value) {
        org.objectweb.ccm.IDL3.ValueDecl[] bases = null;
        bases = value.getAbstractValues();

        // Generalizable element
        java.lang.String gen_id = "";
        org.objectweb.ccm.util.Vector gen_bases = new org.objectweb.ccm.util.Vector();

        for (int i = 0; i < bases.length; i++) {
            gen_id = newId();
            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(gen_id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) bases[i]));
            gen_bases.add(ids);
            
            insertGeneralizableElement(gen_id);
        }

        org.objectweb.ccm.IDL3.ValueDecl base = value.getBaseValue();
          
        if (base != null) {
            gen_id = newId();
            
            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(gen_id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) base));
            gen_bases.add(ids);
              
            insertGeneralizableElement(gen_id);
        }

        generalizations_.put(value.getId(),
            (org.objectweb.ccm.util.Vector[]) 
                gen_bases.toArray(new org.objectweb.ccm.util.Vector[0]));

        return "";
    }

    /**
     * This method generates XMI mappings for a value type factory.
     *
     * @param value The value type to visit.
     */
    public java.lang.String initializer(org.objectweb.ccm.IDL3.ValueDecl value) {
        org.objectweb.ccm.IDL3.Initializer[] initializers = value.getInitializers();

        if (initializers.length == 0) {
            return "";
        }

        java.lang.String[] aliases = { "init", "owner_id" };
        java.lang.Object[] objs = { initializers[0], 
            getXMIId((org.objectweb.ccm.IDL3.TypeRef) value) };
          
        super.map("INITIALIZER", aliases, objs);

        return "";
    }

    /**
     * This method gets the value type factory parameters.
     *
     * @param initializerId The value type factory xmi.id.
     * @param params The list of parameters.
     */
    public java.lang.String
    initializerParams(java.lang.String initializerId, 
                      org.objectweb.ccm.IDL3.ParameterList params) {
                      
        org.objectweb.ccm.IDL3.TypeRef[] types = params.getParamTypes();
        java.lang.String[] names = params.getParamNames();
        int[] modes = params.getParamModes();

        java.lang.String[] aliases = {"name", "kind", "operation", "type"};
        java.lang.Object[] objs = new java.lang.Object[4];
        objs[2] = initializerId;

        for (int i = 0; i < names.length; i++) {
            objs[0] = names[i];
            objs[1] = toStringMode(modes[i]);
            objs[3] = getXMIId(types[i]);

            super.map("OPERATION_PARAMETER", aliases, objs);
        }
          
        return "";
    }

    /**
     * This method maps a value member either to a UML attribute or to a UML association.
     *
     * @param member The value type member to visit.
     */
    public java.lang.String valueMember(org.objectweb.ccm.IDL3.ValueMemberDecl member) {
        org.objectweb.ccm.IDL3.TypeRef member_type = member.getType();
        java.lang.String id = "";
        id = newId(member);

        if (member_type.isDeclaration()) {
            // Association
            org.objectweb.ccm.util.Vector assoc_ends = new org.objectweb.ccm.util.Vector();

            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(id);
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) member.getParent()));
            ids.add(getXMIId(member_type));
            ids.add(member.getName());
            ids.add("");
            ids.add(member.isPrivate() ? "private" : "public");
            ids.add("1");
            ids.add("1");
            assoc_ends.add(ids);

            associations_.put(member.getId(),
                (org.objectweb.ccm.util.Vector[]) 
                    assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));
        } else {
            java.lang.String[] aliases = { "attrib_id", "name", "visibility", 
                "multiplicity", "stereotype", "scope_id", "type" };
            java.lang.Object[] objs = new java.lang.Object[aliases.length];

            objs[0] = id;
            objs[1] = member.getName();
            objs[2] = member.isPrivate() ? "private" : "public";
            objs[3] = "multiplicity='1..1'";
            objs[4] = "";
            objs[5] = getScopeId(member);
            objs[6] = getXMIId(member_type);

            super.map("ATTRIBUTE_BASIC_TYPES", aliases, objs);
        }

        return "";
    }

    /**
     * This method creates a UML generalization for the boxed value.
     *
     * @param valuebox The boxed value to visit.
     */
    public java.lang.String boxedType(org.objectweb.ccm.IDL3.ValueBoxDecl valuebox) {
        java.lang.String gen_id = "";
        gen_id = newId();

        org.objectweb.ccm.util.Vector gen_bases = new org.objectweb.ccm.util.Vector();
        org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
        ids.add(gen_id);
        ids.add(getXMIId(valuebox.getType()));
        gen_bases.add(ids);

        insertGeneralizableElement(gen_id);

        return "";
    }

    /**
     * This method maps a constant to an attribute when defined within 
     * the scope of an IDL interface or to a class containing an attribute
     * when defined within a module scope.
     *
     * @param _constant The constant to visit.
     */
    public java.lang.String
    constant(org.objectweb.ccm.IDL3.ConstantDecl _constant)
    {
        org.objectweb.ccm.IDL3.TypeRef type = null;
        type = _constant.getType();
        org.objectweb.ccm.IDL3.AnyValue value = null;
        value = _constant.getAnyValue();

        java.lang.String id = "";
        id = newId();

        java.lang.String initvalue = "";
        if (value.isInteger()) {
            initvalue = java.lang.Long.toString(value.getAsInteger());
        } else if (value.isString()) {
            initvalue = value.getAsString();
        } else if (value.isWString()) {
            initvalue = value.getAsWString();
        } else if (value.isChar()) {
            initvalue = java.lang.String.valueOf(value.getAsChar());
        } else if (value.isWChar()) {
            initvalue = java.lang.String.valueOf(value.getAsWChar());
        } else if (value.isFixed()) {
            initvalue = value.getAsFixed();
        } else if (value.isFloating()) {
            initvalue = java.lang.Double.toString(value.getAsFloating());
        } else if (value.isBoolean()) {
            initvalue = java.lang.String.valueOf(value.getAsBoolean());
        } else if (value.isEnum()) {
            initvalue = value.getAsEnum();
        }

        if ((_constant.getParent().getDeclKind() == org.objectweb.ccm.IDL3.DeclarationKind._dk_module) ||
            (_constant.getParent().getDeclKind() == org.objectweb.ccm.IDL3.DeclarationKind._dk_repository)) {
            
            org.objectweb.ccm.util.Vector v = new org.objectweb.ccm.util.Vector();

            org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
            ids.add(id);
            ids.add(_constant.getName());
            ids.add("public");
            ids.add(getXMIId((org.objectweb.ccm.IDL3.TypeRef) _constant.getParent()));
            ids.add(getXMIId(type));
            ids.add(initvalue);
            v.add(ids);

            constants_.put(_constant.getId(),
                (org.objectweb.ccm.util.Vector[]) 
                    v.toArray(new org.objectweb.ccm.util.Vector[0]));
        } else {
            java.lang.String[] aliases = { "attrib_id", "name", "visibility", 
                "scope_id", "type", "init_value" };
            java.lang.Object[] objs = new java.lang.Object[aliases.length];

            objs[0] = id;
            objs[1] = _constant.getName();
            objs[2] = "public";
            objs[3] = getXMIId((org.objectweb.ccm.IDL3.TypeRef) _constant.getParent());
            objs[4] = getXMIId(type);
            objs[5] = initvalue;
            
            super.map("ATTRIBUTE_CONSTANT", aliases, objs);
        }

        return "";
    }

    /**
     * This method generates XMI mappings for the class CORBAConstants.
     * 
     * @param scope_id The xmi.id of the scope to visit.
     */
    public java.lang.String corbaConstants(java.lang.String scope_id) {
        if (constants_.size() > 0) {
            java.lang.String[] aliases = { "scope_id" };
            java.lang.Object[] objs = { scope_id };

            super.map("CONSTANT_IN_MODULE_SCOPE", aliases, objs);
        }

        return "";
    }

    /**
     * This method generates attributes of the class CORBAConstants.
     *
     * @param owner_id The xmi.id of the container.
     */
    public java.lang.String constants_in_module_scope(java.lang.String owner_id) {
        java.util.Enumeration keys = constants_.keys();
        java.util.Enumeration values = constants_.elements();

        java.lang.String[] aliases = { "attrib_id", "name", "visibility", 
            "scope_id", "type", "init_value" };
        java.lang.Object[] objs = new java.lang.Object[aliases.length];

        while (keys.hasMoreElements()) {
            keys.nextElement();
            org.objectweb.ccm.util.Vector[] elems = 
                (org.objectweb.ccm.util.Vector[]) values.nextElement();

            for (int i = 0; i < elems.length; i++) {
                for (int j = 0; j < elems[i].size(); j++) {
                    objs[j] = elems[i].get(j);
                }
                objs[3] = owner_id;

                super.map("ATTRIBUTE_CONSTANT", aliases, objs);
            }
        }

        return "";
    }

    /**
     * This method visits the external references.
     *
     * @param vect The list of declarations to visit.
     */
    public java.lang.String visit(org.objectweb.ccm.util.Vector vect) {
        org.objectweb.ccm.IDL3.Declaration[] decls = (org.objectweb.ccm.IDL3.Declaration[])
            vect.toArray(new org.objectweb.ccm.IDL3.Declaration[0]);

        for (int i = 0; i < decls.length; i++) {
            if (decls[i].getDeclKind() == org.objectweb.ccm.IDL3.DeclarationKind._dk_module)  {
                java.lang.String[] aliases = { "obj", "vect" };
                java.lang.Object[] objs = { decls[i], dependencies_tree_.get(decls[i]) };
                super.map("MODULE_EXT_REFS", aliases, objs);
            } else {
                visitContained(decls[i], "");
            }
        }

        return "";
    }

    /**
     * This method generates XMI mappings for the external references.
     */
    public java.lang.String externalTypes() {
        // Build the dependencies tree
        org.objectweb.ccm.IDL3.Declaration[] depend = null;
        org.objectweb.ccm.IDL3.Scope parent = null;
        org.objectweb.ccm.util.Vector childs = null;
        org.objectweb.ccm.IDL3.Declaration obj1 = null;
        org.objectweb.ccm.util.Vector root_dependencies = new org.objectweb.ccm.util.Vector();

        depend = (org.objectweb.ccm.IDL3.Declaration[]) 
            external_refs_.toArray(new org.objectweb.ccm.IDL3.Declaration[0]);

        for (int i = 0; i < depend.length; i++) {
            obj1	= depend[i];
            parent = obj1.getParent();

            while (parent.getDeclKind() != 
                org.objectweb.ccm.IDL3.DeclarationKind._dk_repository) {
                
                if (dependencies_tree_.containsKey(parent)) {
                    childs = (org.objectweb.ccm.util.Vector) 
                        dependencies_tree_.get(parent);
                    if (childs.indexOf(obj1) == -1) {
                        childs.add(obj1);
                    }
                } else {
                    childs = new org.objectweb.ccm.util.Vector();
                    childs.add(obj1);
                    dependencies_tree_.put(parent, childs);
                }

                obj1 = parent;
                parent = obj1.getParent();
            }

            if (root_dependencies.indexOf(obj1) == -1) {
                root_dependencies.add(obj1);
            }
        }

        visit(root_dependencies);

        return "";
    }

    /**
     * This method creates a new UML class for the anonymous type if any,
     * creates an UML class that specializes the type.
     *
     * @param _alias The alias to visit.
     */
    public java.lang.String
    alias(org.objectweb.ccm.IDL3.AliasDecl _alias) {
        java.lang.String anon_id = newId(_alias);
        java.lang.String gen_id = newId();
        java.lang.String name = _alias.getName();
        java.lang.String scope_id = getScopeId(_alias);
        java.lang.String type_id = _alias.getId();

        org.objectweb.ccm.util.Vector ids = new org.objectweb.ccm.util.Vector();
        org.objectweb.ccm.util.Vector gen_bases = new org.objectweb.ccm.util.Vector();
        org.objectweb.ccm.util.Vector assoc_ends = new org.objectweb.ccm.util.Vector();
        ids.add(gen_id);

        // Typedefed anonymous template types
        org.objectweb.ccm.IDL3.TypeRef type = _alias.getType();

        org.objectweb.ccm.IDL3.TypeRefImpl impl = null;

        if (type.getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_sequence) {
            java.lang.String[] aliases = { "seq_id", "name", "scope_id", "type_id" };
            java.lang.Object[] objs = { anon_id, name, scope_id, type_id };

            super.map("TYPEDEF_SEQUENCE", aliases, objs);

            impl = (org.objectweb.ccm.IDL3.TypeRefImpl) type;
            org.objectweb.ccm.IDL3.TypeRef implType = impl.getContentType();
            ids.add(anon_id);
            ids.add(getXMIId(implType));
            ids.add(impl);
            if (implType.getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_value_box) {
                ids.add("0..1");
            } else {
                ids.add("1..1");
            }
            ids.add("CORBASequence");

            // Add an association
            assoc_ends.add(ids);
            assoc_typedefs_.put(type_id,
               (org.objectweb.ccm.util.Vector[]) 
                   assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));
        } else if (type.getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_array) {
            java.lang.String[] aliases = { "array_id", "name", "scope_id", "type_id" };
            java.lang.Object[] objs = { anon_id, name, scope_id, type_id };

            super.map("TYPEDEF_ARRAY", aliases, objs);

            impl = (org.objectweb.ccm.IDL3.TypeRefImpl) type;
            while (impl.getContentType().getTypeKind() == 
                org.objectweb.ccm.IDL3.TypeKind._tk_array) {
                
                impl = (org.objectweb.ccm.IDL3.TypeRefImpl) impl.getContentType();
            }

            ids.add(anon_id);

            if (impl.getContentType().getTypeKind() !=
               org.objectweb.ccm.IDL3.TypeKind._tk_sequence) {
   
               ids.add(getXMIId(impl.getContentType()));
            } else {
               impl = (org.objectweb.ccm.IDL3.TypeRefImpl) impl.getContentType();

               java.lang.String anon_seq_id = newId();

               java.lang.String[] aliases_ = { "seq_id", "scope_id" };
               java.lang.Object[] objs_ = { anon_seq_id, scope_id };

               super.map("ANONYMOUS_SEQUENCE", aliases_, objs_);

               org.objectweb.ccm.util.Vector ids2 = new org.objectweb.ccm.util.Vector();
               org.objectweb.ccm.IDL3.TypeRef implType = impl.getContentType();

               ids2.add(newId());
               ids2.add(anon_seq_id);
               ids2.add(getXMIId(implType));
               ids2.add(impl);
               if (implType.getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_value_box) {
                  ids2.add("0..1");
               } else {
                  ids2.add("1..1");
               }
               ids2.add("CORBASequence");

               // Add an association
               assoc_ends.add(ids2);
               assoc_typedefs_.put(type_id,
                  (org.objectweb.ccm.util.Vector[]) 
                     assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));

               ids.add(anon_seq_id);
            }

            ids.add(type);
            ids.add("1..1");
            ids.add("CORBAArray");

            // Add association
            assoc_ends.add(ids);
            assoc_typedefs_.put(type_id,
               (org.objectweb.ccm.util.Vector[]) 
                    assoc_ends.toArray(new org.objectweb.ccm.util.Vector[0]));
        } else if (type.getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_fixed) {
            impl = (org.objectweb.ccm.IDL3.TypeRefImpl) type;
            java.lang.String digits = java.lang.String.valueOf(impl.getDigits());
            java.lang.String scale = java.lang.String.valueOf(impl.getScale());
                     
            java.lang.String[] aliases = { "fixed_id", "name", "scope_id", "type_id", "digits", "scale" };
            java.lang.Object[] objs = { anon_id, name, scope_id, type_id, digits, scale };

            super.map("TYPEDEF_FIXED", aliases, objs);
            
            ids.add(getXMIId("CORBAFixed"));

            // Add generalization
            gen_bases.add(ids);
            generalizations_.put(type_id, 
                (org.objectweb.ccm.util.Vector[]) 
                    gen_bases.toArray(new org.objectweb.ccm.util.Vector[0]));
        } else {
            java.lang.String[] aliases = { "alias_id", "name", "scope_id", "gen_id", "type_id" };
            java.lang.Object[] objs = { anon_id, name, scope_id, gen_id, type_id };

            super.map("TYPEDEF", aliases, objs);

            ids.add(getXMIId(type));

            // add generalization
            gen_bases.add(ids);
            generalizations_.put(type_id, 
                (org.objectweb.ccm.util.Vector[])
                    gen_bases.toArray(new org.objectweb.ccm.util.Vector[0]));
        }

        return "";
    }

    /**
     * This method generates XMI mappings for an attribute of a qualified association.
     *
     * @param kind The kind of the association.
     * @param impl The object to visit.
     */
    public java.lang.String
    qualifierAttribute(java.lang.String kind, 
                       org.objectweb.ccm.IDL3.TypeRefImpl impl) {
         
         java.lang.String[] aliases = { "name", "value" };
         java.lang.Object[] objs = new java.lang.Object[aliases.length];

         if (kind.equals("CORBASequence")) {
              objs[0] = "index";
              int upperBound = impl.getLength();
              objs[1] = (upperBound == 0) ? "*" : java.lang.String.valueOf(upperBound);

              super.map("ATTRIBUTE_ASSOCIATION_QUALIFIER", aliases, objs);
         } else {
              org.objectweb.ccm.util.Vector arrayDims = new org.objectweb.ccm.util.Vector();
              arrayDims.add(new java.lang.Integer(impl.getLength()));

              while (impl.getContentType().getTypeKind() == org.objectweb.ccm.IDL3.TypeKind._tk_array) {
                    impl = (org.objectweb.ccm.IDL3.TypeRefImpl) impl.getContentType();
                    arrayDims.add(new java.lang.Integer(impl.getLength()));
              }

              for (int i = 0; i < arrayDims.size(); i++) {
                    objs[0] = "index" + java.lang.String.valueOf(i);
                    objs[1] = java.lang.String.valueOf(arrayDims.get(arrayDims.size() - 1 - i));
                    
                    super.map("ATTRIBUTE_ASSOCIATION_QUALIFIER", aliases, objs);
              }
         }

         return "";
    }

    /**
     * This method returns the basename of a filename.
     *
     * @param filename The file name.
     */
    public java.lang.String
    printShortFileName(java.lang.String filename) {
        java.io.File f = new java.io.File(filename);

        return f.getName();
    }
}


