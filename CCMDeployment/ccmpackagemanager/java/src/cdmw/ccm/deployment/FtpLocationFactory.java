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

import com.thalesgroup.CdmwDeployment.LocationCreationException;

import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.DOMException;
import cdmw.xmlutils.XMLHelper;


public class FtpLocationFactory implements LocationFactory {

    private static final String SAVE_NAME_FOR_FTP_ROOT_DIR = "ftpRootDir";
    private static final String SAVE_NAME_FOR_FTP_LOGIN    = "ftpLogin";
    private static final String SAVE_NAME_FOR_FTP_PASSWORD = "ftpPassword";
    private static final String SAVE_NAME_FOR_FTP_HOST     = "ftpHost";

    private String ftpRootDir;
    private String login;
    private String password;
    private String host;


    public FtpLocationFactory(String ftpRootDirectory, String login, String password, String host) {
        this.ftpRootDir = ftpRootDirectory;
        this.login = login;
        this.password = password;
        this.host = host;
        
        // remove last separator to ftpRootDir if exists
        if (this.ftpRootDir.endsWith(java.io.File.separator)) {
            this.ftpRootDir = this.ftpRootDir.substring(this.ftpRootDir.length() - 1);
        }
        
    }

    public FtpLocationFactory(String ftpRootDirectory, String login, String password) {
        this(ftpRootDirectory, login, password, cdmw.ossupport.OS.getHostname());
    }


    public FtpLocationFactory(String ftpRootDirectory) {
        this(ftpRootDirectory, null, null, cdmw.ossupport.OS.getHostname());
    }

    FtpLocationFactory() {
        this.ftpRootDir = null;
        this.login = null;
        this.password = null;
        this.host = null;
    }


    public String filenameToLocation(String filename)
        throws LocationCreationException
    {
        cdmw.common.Assert.check(filename != null);
        cdmw.common.Assert.check(!filename.equals(""));

        if (!filename.startsWith(ftpRootDir)) {
            throw new LocationCreationException(filename +
                " is not in FTP root directory (" + ftpRootDir + ").");
        }

        // get relative path (filename path without ftpRootDir before)
        String relativePath = 
            filename.substring(ftpRootDir.length(), filename.length());

        // convert all '\' into '/'
        relativePath = relativePath.replace('\\', '/');

        return "ftp://" +
               ((login == null) ? "" : login + ":" + password + "@") +
               host + relativePath;
    }


    public void save(com.thalesgroup.CdmwXML.Document doc,
                     com.thalesgroup.CdmwXML.Element parentElt)
        throws XMLPersistable.SaveException
    {
        try {
            Element elt = doc.create_element(
                XMLHelper.toShortArray(getClass().getName()));

            // save ftpRootDir
            elt.set_attribute(XMLHelper.toShortArray(SAVE_NAME_FOR_FTP_ROOT_DIR),
                              XMLHelper.toShortArray(ftpRootDir));

            if (login != null) {
                // save login
                elt.set_attribute(XMLHelper.toShortArray(SAVE_NAME_FOR_FTP_LOGIN),
                                  XMLHelper.toShortArray(login));

                // save password
                elt.set_attribute(XMLHelper.toShortArray(SAVE_NAME_FOR_FTP_PASSWORD),
                                 XMLHelper.toShortArray(password));
            }

            if (host != null) {
                // save host
                elt.set_attribute(XMLHelper.toShortArray(SAVE_NAME_FOR_FTP_HOST),
                                  XMLHelper.toShortArray(host));
            }

            parentElt.append_child(elt);

        } catch (com.thalesgroup.CdmwXML.DOMException e) {
            throw new XMLPersistable.SaveException(
                "Error while saving FtpLocationFactory :\n" +
                e.toString());
        }

    }

    public void load(com.thalesgroup.CdmwXML.Element elt)
        throws XMLPersistable.LoadException
    {
        System.out.println("  loading FtpLocationFactory...");
        try {
            AttrDescription ftpRootDirAttr =
                elt.get_attribute(XMLHelper.toShortArray(SAVE_NAME_FOR_FTP_ROOT_DIR));
            ftpRootDir = XMLHelper.toString(ftpRootDirAttr.value());

            try {
                AttrDescription ftpLoginAttr =
                    elt.get_attribute(XMLHelper.toShortArray(SAVE_NAME_FOR_FTP_LOGIN));
                login = XMLHelper.toString(ftpLoginAttr.value());
            } catch (DOMException e) {
                // no ftpLogin specified
            }

            if (login != null) {
                AttrDescription ftpPassAttr =
                    elt.get_attribute(XMLHelper.toShortArray(SAVE_NAME_FOR_FTP_PASSWORD));
                // if not found throw DOMException (passord is requiered with login)
                password = XMLHelper.toString(ftpPassAttr.value());
            }

            try {
                AttrDescription ftpHostAttr =
                    elt.get_attribute(XMLHelper.toShortArray(SAVE_NAME_FOR_FTP_HOST));
                host = XMLHelper.toString(ftpHostAttr.value());
           } catch (DOMException e) {
                // no ftpHost specified
                host = cdmw.ossupport.OS.getHostname();
            }

        } catch (com.thalesgroup.CdmwXML.DOMException e) {
            throw new XMLPersistable.LoadException(
                "Error while loading FtpLocationFactory :\n" +
                e.toString());
        }
    }

}



