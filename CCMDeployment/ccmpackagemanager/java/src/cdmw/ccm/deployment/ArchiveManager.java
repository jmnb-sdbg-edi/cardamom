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
import com.thalesgroup.CdmwXML.Document;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.DOMException;
import com.thalesgroup.CdmwXML.XPathFormatException;



class ArchiveManager implements XMLPersistable {

    private java.util.Map archives;

    private Archive mainArchive;


    public ArchiveManager(Archive mainArchive) {
        // Note: Use a TreeMap instead of a Hashtable because a Hashtable doesn't
        //       guarantee that elements will be saved and loaded in the same
        //       order. This may cause unit tests of CcmPackageManager fail.
        archives = new java.util.TreeMap();
        this.mainArchive = mainArchive;
        if (mainArchive != null) {
            archives.put(mainArchive.getSourceLink(), mainArchive);
        }
    }


    public Archive getMainArchive() {
        return mainArchive;
    }

    public void setMainArchive(Archive a) 
        throws FileAlreadyReferenced
    {
        // mainArchive must have never been set before
        if (mainArchive != null) {
            throw new FileAlreadyReferenced(
                "A main archive as already been set for this package: " + 
                mainArchive.toString() + ".");
        }
        mainArchive = a;
        addArchive(a);
    }
    
    public java.util.Iterator getArchiveIterator() {
        return archives.values().iterator();
    }


    public Archive getArchive(String sourceLink)
        throws FileNotReferenced
    {
        Archive a = (Archive) archives.get(sourceLink);

        if (a == null) {
            throw new FileNotReferenced(sourceLink +
                " is not a referenced archive in this package.");
        }

        return a;
    }


    public void addArchive(Archive a)
        throws FileAlreadyReferenced
    {
        if (archives.containsKey(a.getSourceLink())) {
            // if link already exists, check if existing is the same object
            Archive existingArchive = (Archive) archives.get(a.getSourceLink());
            if (!existingArchive.equals(a)) {
                throw new FileAlreadyReferenced(
                    "An archive " + a.toString() +
                    " is already referenced in this package.");
            }
        } else {
            archives.put(a.getSourceLink(), a);
        }
    }


    public Archive loadArchiveElement(Element elt)
        throws XMLPersistable.LoadException
    {
        String sourceLink = null;
        try {
            // get href attribute
            AttrDescription hrefAttr =
                elt.get_attribute(XMLHelper.toShortArray("href"));
            sourceLink = XMLHelper.toString(hrefAttr.value());
        } catch (DOMException e) {
            // shouldn't happen, xml file must has been validate by dtd
            cdmw.common.Assert.check(false);
        }

        // try to find if Archive is already referenced
        Archive a = (Archive) archives.get(sourceLink);

        if (a == null) {
            // archive not found, create new one
            try {
                a = new Archive(sourceLink);
            } catch (cdmw.common.BadParameterException e) {
                // sourceLink is not valid
                throw new XMLPersistable.LoadException(
                    "Error while loading archive with href=\"" +
                    sourceLink + "\". This URL is not valid.");
            }

            // try to get actualLocation attribute
            // (present only if this is a save file)
            try {
                AttrDescription locAttr =
                    elt.get_attribute(XMLHelper.toShortArray("actualLocation"));
                String location = XMLHelper.toString(locAttr.value());
                a.setActualLocation(new java.io.File(location));
            } catch (DOMException e) {
                // attribute is not present, ignore exception
            } catch (org.omg.Components.Deployment.InstallationFailure e) {
                throw new XMLPersistable.LoadException(
                    "Error while loading archive with href=\"" +
                    sourceLink + "\" :\n" +
                    e.getMessage());
            }
        }

        return a;
    }


    /**
     * Load all Archives which are saved as <link> child elements
     * of the specified element.
     *
     * @param  parentElt  the parent Element of all <link> elements.
     *
     * @throws  XMLPersistable.LoadException
     *          if an error occurs while loading a <link> element.
     */
    public void load(Element parentElt)
        throws XMLPersistable.LoadException
    {
        NodeDescription[] descriptors = null;
        try {
            // archives are saved as <link> elements
            descriptors = parentElt.select_nodes(
                XMLHelper.toShortArray("link"));
        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        for (int i = 0; i < descriptors.length; i++) {
            Element linkElt = ElementHelper.narrow(descriptors[i].reference());
            Archive a = loadArchiveElement(linkElt);
            try {
                addArchive(a);
            } catch (FileAlreadyReferenced e) {
                // shouldn't happen, a is newly created or is the same than
                // the already referenced one, and must not have been added
                cdmw.common.Assert.check(false);
            }

        }
    }


    public static void saveArchiveElement(Archive a,
                                          Document doc,
                                          Element parentElt)
        throws XMLPersistable.SaveException
    {
        // archives are saved as <link> elements
        LinkManager.saveLinkElement(a, doc, parentElt);
    }



    public void save(Document doc,
                     Element parentElt)
        throws XMLPersistable.SaveException
    {
       java.util.Iterator it = archives.values().iterator();
       while (it.hasNext()) {
           Archive a = (Archive) it.next();
           saveArchiveElement(a, doc, parentElt);
        }
    }


}



