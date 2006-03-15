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


class LinkManager implements XMLPersistable {


    private java.util.Map links;


    public LinkManager() {
        // Note: Use a TreeMap instead of a Hashtable because a Hashtable doesn't
        //       guarantee that elements will be saved and loaded in the same
        //       order. This may cause unit tests of CcmPackageManager fail.
        links = new java.util.TreeMap();
    }

    public java.util.Iterator getLinkIterator() {
        return links.values().iterator();
    }

    public Link getLink(String sourceLink)
        throws FileNotReferenced
    {
        Link l = (Link) links.get(sourceLink);

        if (l == null) {
            throw new FileNotReferenced(sourceLink +
                " is not a referenced link in this package.");
        }

        return l;
    }


    public void addLink(Link l)
        throws FileAlreadyReferenced
    {
        if (links.containsKey(l.getSourceLink())) {
            // if link already exists, check if existing is the same object
            Link existingLink = (Link) links.get(l.getSourceLink());
            if (!existingLink.equals(l)) {
                throw new FileAlreadyReferenced(
                    "A link " + l.toString() +
                    " is already referenced in this package.");
            }
        } else {
            links.put(l.getSourceLink(), l);
        }
    }


    public Link loadLinkElement(Element elt)
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

        // try to find if Link is already referenced
        Link l = (Link) links.get(sourceLink);

        if (l == null) {
            // link not found, create new one
            try {
                l = new Link(sourceLink);
            } catch (cdmw.common.BadParameterException e) {
                // sourceLink is not valid
                throw new XMLPersistable.LoadException(
                    "Error while loading link with href=\"" +
                    sourceLink + "\". This URL is not valid.");
            }

            // try to get actualLocation attribute
            // (present only if this is a save file)
            try {
                AttrDescription locAttr =
                    elt.get_attribute(XMLHelper.toShortArray("actualLocation"));
                String location = XMLHelper.toString(locAttr.value());
                l.setActualLocation(new java.io.File(location));
            } catch (DOMException e) {
                // attribute is not present, ignore exception
            } catch (org.omg.Components.Deployment.InstallationFailure e) {
                throw new XMLPersistable.LoadException(
                    "Error while loading link with href=\"" +
                    sourceLink + "\" :\n" +
                    e.getMessage());
            }
        }

        return l;
    }


    public void load(com.thalesgroup.CdmwXML.Element elt)
        throws XMLPersistable.LoadException
    {
        NodeDescription[] descriptors = null;
        try {
            descriptors = elt.select_nodes(
                XMLHelper.toShortArray("link"));
        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        for (int i = 0; i < descriptors.length; i++) {
            Element linkElt = ElementHelper.narrow(descriptors[i].reference());
            Link l = loadLinkElement(linkElt);
            try {
                addLink(l);
            } catch (FileAlreadyReferenced e) {
                // shouldn't happen, l is newly created or is the same than
                // the already referenced one, and must not have been added
                cdmw.common.Assert.check(false);
            }

        }
    }


    public static void saveLinkElement(Link l,
                                       Document doc,
                                       Element parentElt)
        throws XMLPersistable.SaveException
    {
        try {
            // create <link> element
            Element linkElt = doc.create_element(
                XMLHelper.toShortArray("link"));
            linkElt.set_attribute(
                XMLHelper.toShortArray("href"),
                XMLHelper.toShortArray(l.getSourceLink()));
            if (l.isInstalled()) {
                try {
                    linkElt.set_attribute(
                        XMLHelper.toShortArray("actualLocation"),
                        XMLHelper.toShortArray(l.getActualLocation()));
                } catch (org.omg.Components.Deployment.InstallationFailure ex) {
                    // shouldn't happen, l is not installed
                    cdmw.common.Assert.check(false);
                }
            }
            // add <link> element
            parentElt.append_child(linkElt);

        } catch (DOMException ex) {
            throw new XMLPersistable.SaveException(
                "Error while saving link " + l.toString() + " :\n" +
                ex.toString());
        }
    }


    public void save(Document doc,
                     Element parentElt)
        throws XMLPersistable.SaveException
    {
       java.util.Iterator it = links.values().iterator();
       while (it.hasNext()) {
           Link l = (Link) it.next();
           saveLinkElement(l, doc, parentElt);
        }
    }




}


