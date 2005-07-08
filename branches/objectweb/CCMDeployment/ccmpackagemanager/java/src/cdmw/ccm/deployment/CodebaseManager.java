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

import com.thalesgroup.CdmwDeployment.FileNotReferenced;

import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Document;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.DOMException;
import com.thalesgroup.CdmwXML.XPathFormatException;


class CodebaseManager implements XMLPersistable {

    private static CodebaseManager instance;

    private java.util.Map codebases;



    private CodebaseManager() {
        // Note: Use a TreeMap instead of a Hashtable because a Hashtable doesn't
        //       guarantee that elements will be saved and loaded in the same
        //       order. This may cause unit tests of CcmPackageManager fail.
        this.codebases = new java.util.TreeMap();
    }

    public static CodebaseManager getInstance() {
        if (instance == null) {
            instance = new CodebaseManager();
        }
        return instance;
    }


    public Codebase getCodebase(String name)
        throws FileNotReferenced
    {
        Codebase c = (Codebase) codebases.get(name);

        if (c == null) {
            throw new FileNotReferenced(name +
                " is not a referenced codebase.");
        }

        return c;
    }


    void addCodebase(Codebase c)
        throws FileAlreadyReferenced
    {
        if (codebases.containsKey(c.getFilename())) {
            // if codebase already exists, check if existing is the same object
            Codebase existingCodebase = (Codebase) codebases.get(c.getFilename());
            if (!existingCodebase.equals(c)) {
                throw new FileAlreadyReferenced(
                    "A codebase " + existingCodebase.toString() +
                    " is already referenced.");
            }
        } else {
            // add codebase
            codebases.put(c.getFilename(), c);
        }
    }


    public Codebase loadCodebaseElement(Element elt)
        throws XMLPersistable.LoadException
    {
        String filename = null;
        String sourceLink = null;
        try {
            // get name attribute
            AttrDescription nameAttr =
                elt.get_attribute(XMLHelper.toShortArray("filename"));
            filename = XMLHelper.toString(nameAttr.value());

            // get href attribute
            AttrDescription hrefAttr =
                elt.get_attribute(XMLHelper.toShortArray("href"));
            sourceLink = XMLHelper.toString(hrefAttr.value());
        } catch (DOMException e) {
            // shouldn't happen, xml file must has been validate by dtd
            cdmw.common.Assert.check(false);
        }

        // try to find if Codebase is already referenced
        Codebase c = (Codebase) codebases.get(filename);

        if (c == null) {
            // codebase not found, create new one
            try {
                c = new Codebase(filename, sourceLink);
            } catch (cdmw.common.BadParameterException e) {
                // sourceLink is not valid
                throw new XMLPersistable.LoadException(
                    "Error while loading codebase with fileame=\"" +
                    filename + "\" and href=\"" +
                    sourceLink + "\". This URL is not valid.");
            }

            // try to get actualLocation attribute
            // (present only if this is a save file)
            try {
                AttrDescription locAttr =
                    elt.get_attribute(XMLHelper.toShortArray("actualLocation"));
                String location = XMLHelper.toString(locAttr.value());
                c.setActualLocation(new java.io.File(location));
            } catch (DOMException e) {
                // attribute is not present, ignore exception
            } catch (org.omg.Components.Deployment.InstallationFailure e) {
                throw new XMLPersistable.LoadException(
                    "Error while loading codebase with filename=\"" +
                    filename + "\" and href=\"" + sourceLink + "\" :\n"
                    + e.getMessage());
            }

        } else {
            // check if existing Codebase has same sourceLink
            if (!c.getSourceLink().equals(sourceLink)) {
                throw new XMLPersistable.LoadException(
                    "Conflict between existing Codebase: " + c.toString() +
                    " and the loading codebase with same filename but with href=\"" +
                    sourceLink + "\".");
            }
        }

        return c;
    }


    public void load(Element elt)
        throws XMLPersistable.LoadException
    {
        NodeDescription[] descriptors = null;
        try {
            descriptors = elt.select_nodes(
                XMLHelper.toShortArray("codebase"));
        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        for (int i = 0; i < descriptors.length; i++) {
            Element codebaseElt = ElementHelper.narrow(descriptors[i].reference());
            Codebase c = loadCodebaseElement(codebaseElt);
            try {
                addCodebase(c);
            } catch (FileAlreadyReferenced e) {
                // shouldn't happen, c is newly created or is the same than
                // the already referenced one, and must not have been added
                cdmw.common.Assert.check(false);
            }

        }
    }


    public static void saveCodebaseElement(Codebase c,
                                           Document doc,
                                           Element parentElt)
        throws XMLPersistable.SaveException
    {
        try {
            // create <codebase> element
            Element codebaseElt = doc.create_element(
                XMLHelper.toShortArray("codebase"));
            codebaseElt.set_attribute(
                XMLHelper.toShortArray("filename"),
                XMLHelper.toShortArray(c.getFilename()));
            codebaseElt.set_attribute(
                XMLHelper.toShortArray("href"),
                XMLHelper.toShortArray(c.getSourceLink()));
            if (c.isInstalled()) {
                try {
                    codebaseElt.set_attribute(
                        XMLHelper.toShortArray("actualLocation"),
                        XMLHelper.toShortArray(c.getActualLocation()));
                } catch (org.omg.Components.Deployment.InstallationFailure e) {
                    // shouldn't happen, c is installed
                    cdmw.common.Assert.check(false);
                }
            }
            // add <link> element
            parentElt.append_child(codebaseElt);

        } catch (DOMException ex) {
            throw new XMLPersistable.SaveException(
                "Error while saving codebase " + c.toString() + " :\n" +
                ex.toString());
        }
    }


    public void save(Document doc,
                     Element parentElt)
        throws XMLPersistable.SaveException
    {
       java.util.Iterator it = codebases.values().iterator();
       while (it.hasNext()) {
           Codebase c = (Codebase) it.next();
           saveCodebaseElement(c, doc, parentElt);
        }
    }

}



