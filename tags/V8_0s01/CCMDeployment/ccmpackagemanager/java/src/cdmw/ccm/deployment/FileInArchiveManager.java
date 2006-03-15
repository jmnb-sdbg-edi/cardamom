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


class FileInArchiveManager implements XMLPersistable {

    // separator between archive entry and archive link for hash key
    public static final String ENTRY_SEPARATOR = "!";


    private ArchiveManager archiveManager;

    private java.util.Map filesinarchive;


    public FileInArchiveManager(ArchiveManager archiveManager)
    {
        this.archiveManager = archiveManager;

        // Note: Use a TreeMap instead of a Hashtable because a Hashtable doesn't
        //       guarantee that elements will be saved and loaded in the same
        //       order. This may cause unit tests of CcmPackageManager fail.
        filesinarchive = new java.util.TreeMap();
    }

    public java.util.Iterator getFileInArchiveIterator() {
        return filesinarchive.values().iterator();
    }

    public FileInArchive getFileInArchive(String archiveEntry, String archiveSourceLink)
        throws FileNotReferenced
    {
        // if archiveSourceLink is an empty string, it means that
        // entry is in mainArchive
        if (archiveSourceLink.equals("")) {
            Archive mainArchive = archiveManager.getMainArchive();
            if (mainArchive == null) {
                throw new FileNotReferenced(
                    "This package has no main archive. " + archiveEntry + 
                    " without archive source link is not a referenced" +
                    " fileinarchive in this package.");
            }
            archiveSourceLink = mainArchive.getSourceLink();
        }
        
        
        FileInArchive f = (FileInArchive) filesinarchive.get(archiveEntry +
                                                             ENTRY_SEPARATOR +
                                                             archiveSourceLink);

        if (f == null) {
            throw new FileNotReferenced(archiveEntry + " in " + archiveSourceLink +
                " is not a referenced fileinarchive in this package.");
        }

        return f;
    }

    public void addFileInArchive(FileInArchive f)
        throws FileAlreadyReferenced
    {
        if (filesinarchive.containsKey(f.getArchiveEntry() +
                                       ENTRY_SEPARATOR +
                                       f.getArchive().getSourceLink()))
        {
            // if fileinarchive already exists, check if existing is the same object
            FileInArchive existingFile =
                (FileInArchive) filesinarchive.get(f.getArchiveEntry() +
                                                   ENTRY_SEPARATOR +
                                                   f.getArchive().getSourceLink());
            if (!existingFile.equals(f)) {
                throw new FileAlreadyReferenced(
                    "A fileinarchive " + f.toString() +
                    " is already referenced in this package.");
            }

        } else {
            archiveManager.addArchive(f.getArchive());
            filesinarchive.put(f.getArchiveEntry() +
                               ENTRY_SEPARATOR +
                               f.getArchive().getSourceLink(),
                               f);
        }
    }


    public FileInArchive loadFileInArchiveElement(Element elt)
        throws XMLPersistable.LoadException
    {
        String archiveEntry = null;
        try {
            // get name attribute
            AttrDescription nameAttr =
                elt.get_attribute(XMLHelper.toShortArray("name"));
            archiveEntry = XMLHelper.toString(nameAttr.value());

        } catch (DOMException e) {
            // shouldn't happen, xml file must has been validate by dtd
            cdmw.common.Assert.check(false);
        }


        // try to find the href attribute of a <link> child
        NodeDescription[] children = null;
        try {
            children = elt.select_nodes(
                XMLHelper.toShortArray("link"));
        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        Archive a = null;
        if (children.length == 0) {
            // no link, archive must be mainArchive
            a = archiveManager.getMainArchive();
            if (a == null) {
                throw new XMLPersistable.LoadException(
                    "Error while loading fileinarchive with name=\"" +
                    archiveEntry + "\". It has no link defined, " +
                    "and the package is not in a main archive.");
            }
        } else {
            Element linkElt = ElementHelper.narrow(children[0].reference());
            a = archiveManager.loadArchiveElement(linkElt);
        }

        // try to find if FileInArchive is already referenced
        FileInArchive f = (FileInArchive) filesinarchive.get(archiveEntry +
                                                             ENTRY_SEPARATOR +
                                                             a.getSourceLink());

        if (f == null) {
            f = new FileInArchive(archiveEntry, a);

            // try to get actualLocation attribute
            // (present only if this is a save file)
            try {
                AttrDescription locAttr =
                    elt.get_attribute(XMLHelper.toShortArray("actualLocation"));
                String location = XMLHelper.toString(locAttr.value());
                f.setActualLocation(new java.io.File(location));
            } catch (DOMException e) {
                // attribute is not present, ignore exception
            } catch (org.omg.Components.Deployment.InstallationFailure e) {
                throw new XMLPersistable.LoadException(
                    "Error while loading fileinarchive with name=\"" +
                    archiveEntry + "\" and " + a.toString() + " for archive :\n" +
                    e.getMessage());
            }
        }

        return f;
    }



    public void load(Element elt)
        throws XMLPersistable.LoadException
    {
        NodeDescription[] descriptors = null;
        try {
            descriptors = elt.select_nodes(
                XMLHelper.toShortArray("fileinarchive"));
        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        for (int i = 0; i < descriptors.length; i++) {
            Element fileElt = ElementHelper.narrow(descriptors[i].reference());
            FileInArchive f = loadFileInArchiveElement(fileElt);
            try {
                addFileInArchive(f);
            } catch (FileAlreadyReferenced e) {
                // shouldn't happen, f is newly created or is the same than
                // the already referenced one, and must not have been added
                cdmw.common.Assert.check(false);
            }

        }
    }


    public static void saveFileInArchiveElement(FileInArchive f,
                                                Document doc,
                                                Element parentElt)
        throws XMLPersistable.SaveException
    {
           try {
                // create <fileinarchive> element
                Element fileElt = doc.create_element(
                    XMLHelper.toShortArray("fileinarchive"));
                fileElt.set_attribute(
                    XMLHelper.toShortArray("name"),
                    XMLHelper.toShortArray(f.getArchiveEntry()));
                if (f.isInstalled()) {
                    try {
                        fileElt.set_attribute(
                            XMLHelper.toShortArray("actualLocation"),
                            XMLHelper.toShortArray(f.getActualLocation()));
                    } catch (org.omg.Components.Deployment.InstallationFailure e) {
                        // shouldn't happen, fileinarchive is installed
                        cdmw.common.Assert.check(false);
                    }
                }

                // save archive
                ArchiveManager.saveArchiveElement(f.getArchive(), doc, fileElt);

                // add <fileinarchive> element
                parentElt.append_child(fileElt);

            } catch (DOMException ex) {
                throw new XMLPersistable.SaveException(
                    "Error while saving fileinarchive " + f.toString() + " :\n" +
                    ex.toString());
            }


    }



    public void save(Document doc,
                     Element parentElt)
        throws XMLPersistable.SaveException
    {
       java.util.Iterator it = filesinarchive.values().iterator();
       while (it.hasNext()) {
           FileInArchive f = (FileInArchive) it.next();
           saveFileInArchiveElement(f, doc, parentElt);
        }
    }






}



