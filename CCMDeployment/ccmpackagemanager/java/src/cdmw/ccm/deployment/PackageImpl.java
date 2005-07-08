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

import org.omg.Components.RemoveFailure;
import org.omg.Components.Deployment.InstallationFailure;
import com.thalesgroup.CdmwDeployment.DELETE_ERROR;

import com.thalesgroup.CdmwDeployment.FileNotReferenced;
import com.thalesgroup.CdmwDeployment.LocationCreationException;

import com.thalesgroup.CdmwXML.Document;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.ElementDescription;
import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.XPathFormatException;


class PackageImpl implements com.thalesgroup.CdmwDeployment.PackageOperations,
                             XMLPersistable
{
    private static final String SAVE_NAME_FOR_ARCHIVES = "archiveList";
    private static final String SAVE_NAME_FOR_FILES_IN_ARCHIVE = "fileinarchiveList";
    private static final String SAVE_NAME_FOR_LINKS = "linkList";
    private static final String SAVE_NAME_FOR_CODEBASES = "codebaseList";
    private static final String SAVE_NAME_FOR_MAIN_ARCHIVE = "mainArchive";


    protected LinkManager linkMgr;
    protected ArchiveManager archiveMgr;
    protected FileInArchiveManager fileinarchiveMgr;
    
    protected java.util.Map usedCodebases;


    public PackageImpl(Archive mainArchive) {
        linkMgr = new LinkManager();
        archiveMgr = new ArchiveManager(mainArchive);
        fileinarchiveMgr = new FileInArchiveManager(archiveMgr);
        // Note: Use a TreeMap instead of a Hashtable because a Hashtable doesn't
        //       guarantee that elements will be saved and loaded in the same
        //       order. This may cause unit tests of CcmPackageManager fail.    
        usedCodebases = new java.util.TreeMap();

    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/Package/get_link_location:1.0
    //
    /***/
    public String get_link_location(String link_href)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        Link l = linkMgr.getLink(link_href);
        String location = l.getActualLocation();

        return PackageManagerImpl.getInstance()
                                 .getLocationFactory()
                                 .filenameToLocation(location);
    }


    //
    // IDL:thalesgroup.com/CdmwDeployment/Package/get_fileinarchive_location:1.0
    //
    /***/
    public String get_fileinarchive_location(String name,
                                             String archive_link)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        FileInArchive f =  fileinarchiveMgr.getFileInArchive(name, archive_link);
        String location = f.getActualLocation();

        return PackageManagerImpl.getInstance()
                                 .getLocationFactory()
                                 .filenameToLocation(location);
    }



    //
    // IDL:thalesgroup.com/CdmwDeployment/Package/get_codebase_location:1.0
    //
    /***/
    public String get_codebase_location(String filename)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        Codebase c = (Codebase) usedCodebases.get(filename);
        if (c == null) {
            throw new FileNotReferenced("Codebase " + filename +
                                        " is not referenced in this package.");
        }

        String location = c.getActualLocation();

        return PackageManagerImpl.getInstance()
                                 .getLocationFactory()
                                 .filenameToLocation(location);
    }



    public LinkManager getLinkManager() {
        return linkMgr;
    }

    public ArchiveManager getArchiveManager() {
        return archiveMgr;
    }

    public FileInArchiveManager getFileInArchiveManager() {
        return fileinarchiveMgr;
    }

    public java.util.Iterator getArchiveIterator() {
        return archiveMgr.getArchiveIterator();
    }

    public java.util.Iterator getFileInArchiveIterator() {
        return fileinarchiveMgr.getFileInArchiveIterator();
    }

    public java.util.Iterator getLinkIterator() {
        return linkMgr.getLinkIterator();
    }

    public java.util.Iterator getUsedCodebaseIterator() {
        return usedCodebases.values().iterator();
    }

    protected void addFileReference(FileReference f)
        throws FileAlreadyReferenced
    {
        if (f instanceof Codebase) {
            Codebase c = (Codebase) f;
            CodebaseManager.getInstance().addCodebase(c);
            usedCodebases.put(c.getFilename(), c);

        } else if (f instanceof Archive) {
            archiveMgr.addArchive((Archive) f);

        } else if (f instanceof Link) {
            linkMgr.addLink((Link) f);

        } else if (f instanceof FileInArchive) {
            fileinarchiveMgr.addFileInArchive((FileInArchive) f);

        } else {
            // FileReference is abstract.
            // Shouldn't happen, except if a new inheritance has been created !!
            cdmw.common.Assert.check(false);
        }
    }


    public void remove() 
        throws RemoveFailure
    {
        try {
        
            // remove archives
            java.util.Iterator it = archiveMgr.getArchiveIterator();
            while (it.hasNext()) {
                Archive a = (Archive) it.next();
                if (a.isInstalled()) {
                    if (!a.delete()) {
                        throw new RemoveFailure(
                            "Cannot delete file " + a.getActualLocation(),
                            DELETE_ERROR.value);
                    }
                }
            }
            
            // remove fileinarchives
            it = fileinarchiveMgr.getFileInArchiveIterator();
            while (it.hasNext()) {
                FileInArchive f = (FileInArchive) it.next();
                if (f.isInstalled()) {
                    if (!f.delete()) {
                        throw new RemoveFailure(
                            "Cannot delete file " + f.getActualLocation(),
                            DELETE_ERROR.value);
                    }
                }
            }
            
            // remove links
            it = linkMgr.getLinkIterator();
            while (it.hasNext()) {
                Link l = (Link) it.next();
                if (l.isInstalled()) {
                    if (!l.delete()) {
                        throw new RemoveFailure(
                            "Cannot delete file " + l.getActualLocation(),
                            DELETE_ERROR.value);
                    }
                }
            }
            
            // don't remove codebases, may be used by other packages
            
        } catch (InstallationFailure e) {
            // shouldn't happen:
            // getActualLocation() is called only when file is installed
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }
        
    }
    



    public void save(com.thalesgroup.CdmwXML.Document doc,
                     com.thalesgroup.CdmwXML.Element elt)
        throws XMLPersistable.SaveException
    {
        try {
            // save archives
            Element archivesElt = doc.create_element(
                XMLHelper.toShortArray(SAVE_NAME_FOR_ARCHIVES));
            archiveMgr.save(doc, archivesElt);
            elt.append_child(archivesElt);

            // save mainArchive
            Archive mainArchive = archiveMgr.getMainArchive();
            if (mainArchive != null) {
                Element mainArchiveElt = doc.create_element(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_MAIN_ARCHIVE));
                ArchiveManager.saveArchiveElement(mainArchive, doc, mainArchiveElt);
                elt.append_child(mainArchiveElt);
            }

            // save filesinarchive
            Element filesinarchiveElt = doc.create_element(
                XMLHelper.toShortArray(SAVE_NAME_FOR_FILES_IN_ARCHIVE));
            fileinarchiveMgr.save(doc, filesinarchiveElt);
            elt.append_child(filesinarchiveElt);

            // save links
            Element linksElt = doc.create_element(
                XMLHelper.toShortArray(SAVE_NAME_FOR_LINKS));
            linkMgr.save(doc, linksElt);
            elt.append_child(linksElt);

            // save used codebases
            Element codebasesElt = doc.create_element(
                XMLHelper.toShortArray(SAVE_NAME_FOR_CODEBASES));
            java.util.Iterator it = usedCodebases.values().iterator();
            while (it.hasNext()) {
                Codebase c = (Codebase) it.next();
                CodebaseManager.saveCodebaseElement(c, doc, codebasesElt);
            }
            elt.append_child(codebasesElt);

        } catch (com.thalesgroup.CdmwXML.DOMException e) {
            throw new XMLPersistable.SaveException(
                "Error while saving a PackageImpl :\n" + e.toString());
        }
    }



    public void load(com.thalesgroup.CdmwXML.Element elt)
        throws XMLPersistable.LoadException
    {
        // load archives
        NodeDescription[] descriptors = null;
        try {
            descriptors = elt.select_nodes(
                XMLHelper.toShortArray(SAVE_NAME_FOR_ARCHIVES));
        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        if (descriptors.length == 0) {
            throw new XMLPersistable.LoadException(
                "No <" + SAVE_NAME_FOR_ARCHIVES + "> element found.");
        }
        Element archivesElt = ElementHelper.narrow(descriptors[0].reference());
        archiveMgr.load(archivesElt);

        // load mainArchive
        try {
            descriptors = elt.select_nodes(
                XMLHelper.toShortArray(SAVE_NAME_FOR_MAIN_ARCHIVE + "/link"));
        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        if (descriptors.length != 0) {
            Element archiveElt = ElementHelper.narrow(descriptors[0].reference());
            Archive mainArchive = archiveMgr.loadArchiveElement(archiveElt);
            try {
                archiveMgr.setMainArchive(mainArchive);
            } catch (FileAlreadyReferenced e) {
                // shouldn't happen, mainArchive is newly created or is the same than
                // the already referenced one, and must not have been added
                cdmw.common.Assert.check(false);
            }
        }

        // load filesinarchive
        try {
            descriptors = elt.select_nodes(
                XMLHelper.toShortArray(SAVE_NAME_FOR_FILES_IN_ARCHIVE));
        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        if (descriptors.length == 0) {
            throw new XMLPersistable.LoadException(
                "No <" + SAVE_NAME_FOR_FILES_IN_ARCHIVE + "> element found.");
        }
        Element filesinarchiveElt = ElementHelper.narrow(descriptors[0].reference());
        fileinarchiveMgr.load(filesinarchiveElt);

        // load links
        try {
            descriptors = elt.select_nodes(
                XMLHelper.toShortArray(SAVE_NAME_FOR_LINKS));
        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        if (descriptors.length == 0) {
            throw new XMLPersistable.LoadException(
                "No <" + SAVE_NAME_FOR_LINKS + "> element found.");
        }
        Element linksElt = ElementHelper.narrow(descriptors[0].reference());
        linkMgr.load(linksElt);

        // load used codebases
        try {
            descriptors = elt.select_nodes(
                XMLHelper.toShortArray(SAVE_NAME_FOR_CODEBASES + "/*"));
        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        for (int i = 0; i < descriptors.length; i++) {
            Element codebaseElt = ElementHelper.narrow(descriptors[i].reference());
            Codebase c = CodebaseManager.getInstance()
                                        .loadCodebaseElement(codebaseElt);
            try {
                CodebaseManager.getInstance().addCodebase(c);
            } catch (FileAlreadyReferenced e) {
                // shouldn't happen, c is newly created or is the same than
                // the already referenced one, and must not have been added
                cdmw.common.Assert.check(false);
            }
            usedCodebases.put(c.getFilename(), c);
        }

    }


    protected void saveFileReference(FileReference f,
                                     Document doc,
                                     Element parentElt)
        throws XMLPersistable.SaveException
    {
        if (f instanceof Codebase) {
            CodebaseManager.saveCodebaseElement((Codebase) f,
                                                doc,
                                                parentElt);
        } else if (f instanceof Archive) {
            ArchiveManager.saveArchiveElement((Archive) f,
                                              doc,
                                              parentElt);
        } else if (f instanceof Link) {
            LinkManager.saveLinkElement((Link) f,
                                        doc,
                                        parentElt);
        } else if (f instanceof FileInArchive) {
            FileInArchiveManager.saveFileInArchiveElement((FileInArchive) f,
                                                          doc,
                                                          parentElt);
        } else {
            // FileReference is abstract.
            // Shouldn't happen, except if a new inheritance has been created !!
            cdmw.common.Assert.check(false);
        }
    }


    // read element, create or retrieve FileReference
    protected FileReference loadFileReference(ElementDescription eltDesc)
        throws XMLPersistable.LoadException
    {
        String referenceType = XMLHelper.toString(eltDesc.name());
        Element elt = ElementHelper.narrow(eltDesc.reference());

        if (referenceType.equals("link")) {
            Link l = linkMgr.loadLinkElement(elt);
            return l;

        } else if (referenceType.equals("codebase")) {
            // codebases are managed by the singleton CodebaseManager
            Codebase c = CodebaseManager.getInstance()
                                        .loadCodebaseElement(elt);
            return c;

        } else if (referenceType.equals("fileinarchive")) {
            FileInArchive f =
                fileinarchiveMgr.loadFileInArchiveElement(elt);
            return f;

        } else {
            // only <link>, <codebase> or <fileinarchive> 
            // are accepted.
            throw new XMLPersistable.LoadException(
                "Element <" + referenceType + 
                "> doesn't correspond to a FileReference.");
        }
    }


}



