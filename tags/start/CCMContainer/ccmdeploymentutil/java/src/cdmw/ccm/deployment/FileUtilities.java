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


/**
 * Utility class for management of files.
 */
public class FileUtilities {

    
    private static final int COPY_BUFFER_SIZE = 1000;


    /**
     * Checks that a directory is valid.
     * This method tests if the directory exists and is not a file. 
     * If the directory doesn't exists, this method creates it.
     *
     * @param  directoryPath  the path of the directory to be checked.
     *
     * @return  a new <code>File</code> instance corresponding to the directory.
     *
     * @throws  IOException
     *          raised if directoryPath point to an existing file or if
     *          creation of directory failed.
     */
    public static java.io.File checkDirectory(String directoryPath)
        throws java.io.IOException
    {
        java.io.File dir = new java.io.File(directoryPath);
        checkDirectory(dir);
        return dir;    
    }
    
    
    /**
     * Checks that a directory is valid.
     * This method tests if the directory exists and is not a file. 
     * If the directory doesn't exists, this method creates it.
     *
     * @param  directory  the directory to be checked.
     *
     * @throws  IOException
     *          raised if directoryPath point to an existing file or if
     *          creation of directory failed.
     */
    public static void checkDirectory(java.io.File directory)
        throws java.io.IOException        
    {
        if (!directory.exists()) {
            if (!directory.mkdirs()) {
                throw new java.io.IOException("Cannot create directory " + 
                                              directory.toString());
            }
        } else {
            if (!directory.isDirectory()) {
                throw new java.io.IOException(directory.toString() + 
                                              " is not a directory");
            }
        }    
    }
    
    /**
     * Creates a new empty directory in the specified directory, 
     * using the given prefix string to generate its name.
     *
     * @param  prefix  The prefix string to be used in generating the
     *                 file's name; must be at least three characters long
     * @param  directory  The directory in which the directory is to be created, 
     *                    or null if the default temporary directory is to be used.
     *
     * @throws IOException 
     *         raised if the directory could'nt be created.
     * @throws IllegalArgumentException
     *         raised if the prefix argument contains fewer than three characters
     */
    public static java.io.File createTempDir(String prefix, java.io.File directory)
        throws java.io.IOException
    {
        java.io.File tmp = java.io.File.createTempFile(prefix, "", directory);
        tmp.delete();
        tmp.mkdir();
        return tmp;   
    }
    
    /**
     * Creates a new empty directory in the default temporary directory, 
     * using the given prefix string to generate its name.
     *
     * @param  prefix  The prefix string to be used in generating the
     *                 file's name; must be at least three characters long
     *
     * @throws IOException 
     *         raised if the directory could'nt be created.
     * @throws IllegalArgumentException
     *         raised if the prefix argument contains fewer than three characters
     */
    public static java.io.File createTempDir(String prefix) 
        throws java.io.IOException
    {
        return createTempDir(prefix, null);
    }
    
    
    /**
     * Moves a file to a new file.
     *
     * @param  oldFile  the file to be moved.
     * @param  newFile  the file where to move the old file.
     *
     * @throws  IOException
     *          raised if the old file cannot be moved.
     */
    public static void moveFileTo(java.io.File oldFile,
                                  java.io.File newFile)
        throws java.io.IOException
    {
        // try to directly rename oldFile to newFile 
        // (work only if oldFile and newFile are on the same disk)
        if (!oldFile.renameTo(newFile)) {
            
            // check if old file exists and is a file
            if (!oldFile.isFile()) {
                throw new java.io.IOException(
                    oldFile.toString() + " is not a file.");
            }
            
            // check that destination doesn't already exists
            if (newFile.exists()) {
                throw new java.io.IOException(
                    newFile.toString() + " already exists.");
            }
            
            // check that old file can be readed and writed
            if (!oldFile.canRead()) {
                throw new java.io.IOException(
                    "Can't read " + oldFile.toString());
            }
            if (!oldFile.canWrite()) {
                throw new java.io.IOException(
                    "Can't write " + oldFile.toString());
            }
            
            // try to copy oldFile to newFile and to delete oldFile
            copyFile(oldFile, newFile.getAbsolutePath());
            
            if (!oldFile.delete()) {
                throw new java.io.IOException(
                    "Copy from " + oldFile.toString() + " to " +
                    newFile.toString() + " done, but can't delete " + 
                    oldFile.toString());
            }
        }
    }
    
    /**
     * Removes a directory and all what it contains.
     *
     * @param  directory  the directory to be removed.
     *
     * @throws IOException
     *         raised if the directory cannot be removed
     */
    public static void removeDirectoryRecursively(java.io.File directory) 
        throws java.io.IOException
    {
        if (!directory.isDirectory()) {
            throw new java.io.IOException(directory.toString() + " is not a directory.");
        }
        
        java.io.File[] list = directory.listFiles();
        for (int i = 0; i < list.length; i++) {
            if (list[i].isDirectory()) {
                removeDirectoryRecursively(list[i]);
            } else {
                if (!list[i].delete()) {
                    throw new java.io.IOException("Cannot delete file: " + 
                                                  list[i].toString());
                }
            }
        }

        if (!directory.delete()) {
            throw new java.io.IOException("Cannot delete directory: " + 
                                          directory.toString());
        }
    }
    

    /**
     * Downloads a file from an source URL to a local destination.
     *
     * @param  source  the source URL.
     * @param  destination  the path of the destination file.
     *
     * @throws IOException
     *         raised if the download fails.
     * @throws BadParameterException
     *         raised if the source URL is not valid.
     */
    public static void downloadFile(String source, String destination) 
        throws java.io.IOException,
               cdmw.common.BadParameterException
    {
        java.net.URL url;
        
        // convert source into URL
        url = toURL(source);
        
        downloadFile(url, destination);
    }


    /**
     * Downloads a file from an source URL to a local destination.
     *
     * @param  source  the source URL.
     * @param  destination  the path of the destination file.
     *
     * @throws IOException
     *         raised if the download fails.
     * @throws BadParameterException
     *         raised if the source URL is not valid.
     */
    public static void downloadFile(java.net.URL source, String destination)
        throws java.io.IOException,
               cdmw.common.BadParameterException
    {
        java.net.URLConnection conn;
        java.io.InputStream is;

        try {
            conn = source.openConnection();
            conn.setUseCaches(false);
            is = conn.getInputStream();
        } catch (java.io.IOException e) {
            throw new cdmw.common.BadParameterException(
                "Cannot open location " + source.toString());
        }
        
        copyStream(is, destination);
        is.close();
    }
 

    /**
     * Copies a file from an local source to a local destination.
     *
     * @param  source  the path of the source file.
     * @param  destination  the path of the destination file.
     *
     * @throws IOException
     *         raised if the copy fails.
     */
    public static void copyFile(String source, String destination)
        throws java.io.IOException
    {
        java.io.File dest = new java.io.File(destination);
        checkDirectory(dest.getParent());
        java.io.FileInputStream is = new java.io.FileInputStream(source);
        copyStream(is, destination);
        is.close();
    }


    /**
     * Copies a file from an local source to a local destination.
     *
     * @param  source  the source file.
     * @param  destination  the path of the destination file.
     *
     * @throws IOException
     *         raised if the copy fails.
     */
    public static void copyFile(java.io.File source, String destination)
        throws java.io.IOException
    {
        java.io.File dest = new java.io.File(destination);
        checkDirectory(dest.getParent());
        java.io.FileInputStream is = new java.io.FileInputStream(source);
        copyStream(is, destination);
        is.close();
    }
    
    
    /**
     * Copies an InputStream into local destination file.
     *
     * @param  source  the source InputStream.
     * @param  destination  the path of the destination file.
     *
     * @throws IOException
     *         raised if the copy fails.
     */
    public static void copyStream(java.io.InputStream source, String destFile)     
        throws java.io.IOException
    {
        java.io.FileOutputStream os =
            new java.io.FileOutputStream(destFile);

        byte[] buf = new byte[COPY_BUFFER_SIZE];
        int n;
        while ((n = source.read(buf)) != -1) {
            os.write(buf, 0, n);
        }
        
        os.close();        
    }


    /**
     * Convert a location into an URL.
     * The location could be an URL or a absolute or relative path.
     *
     * @param  location  the location for which an URL is required.
     *
     * @throws BadParameterException
     *         raised if the location is not valid
     */
    public static java.net.URL toURL(String location)
        throws cdmw.common.BadParameterException
    {
        if (location == null) {
            throw new cdmw.common.BadParameterException(
                "Location is null.");
        }
        
        java.net.URL url = null;

        try {
            // try direct convertion into URL
            url = new java.net.URL(location);

        } catch (java.net.MalformedURLException e) {
            // if it's not an URL, test is it's a local file
            java.io.File file = new java.io.File(location);
            
            if (!file.canRead()) {
                throw new cdmw.common.BadParameterException(
                    location + " is not a valid URL");
            }

            try {
                // convert file into URL
                url = file.toURL();
            } catch (java.net.MalformedURLException ex) {
                // Shouldn't happen
                cdmw.common.Assert.check(false);
            }
        }
        
        return url;
    }
    

    /**
     * Tests if a file is a zip file
     *
     * @param  filename  the path of the file to be tested.
     *
     * @return  <code>true</code> if the file is a zip,
     *          <code>false</code> otherwise.
     */
    public static boolean isZip(String fileName) {
        try {
            java.util.zip.ZipFile zip = new java.util.zip.ZipFile(fileName);
            zip.close();
        } catch(java.util.zip.ZipException e) {
            return false;
        } catch(java.io.IOException e) {
            return false;
        }
        return true;
    }


    /**
     * Unzips a file from a zip file to a destination file.
     *
     * @param  entry  the <code>ZipEntry</code> corresponding to the file
     *                to be unziped.
     * @param  zip  the zip file into which the file is.
     * @param  destination  the path of the unziped file.
     *
     * @throws ZipException
     *         raised if zip is not a zip file, if the entry is not found
     *         in zip or if an error occurs during extraction.
     * @throws IOException
     *         raised if an error occurs during writing of destination file.
     */
    public static void unzipFileAs(java.util.zip.ZipEntry entry, 
                                  java.util.zip.ZipFile zip,
                                  java.io.File destination)
        throws java.util.zip.ZipException, java.io.IOException
    {
        java.io.InputStream is = zip.getInputStream(entry);
        if (is == null) {
            throw new java.util.zip.ZipException(entry.toString() + 
                                                 " not found in zip file: " +
                                                 zip.toString());
        }
                                        
        copyStream(is, destination.toString());
        is.close();
    }
    
    
    /**
     * Unzips a file from a zip file to a destination file.
     *
     * @param  zipEntry  the entry of the file to be unziped.
     * @param  zip  the path of the zip file into which the file is.
     * @param  destination  the path of the unziped file.
     *
     * @throws ZipException
     *         raised if zip is not a zip file, if the entry is not found
     *         in zip or if an error occurs during extraction.
     * @throws IOException
     *         raised if an error occurs during writing of destination file.
     */
    public static void unzipFileAs(String zipEntry, String zipName, String destinationName)
        throws java.util.zip.ZipException, 
               java.io.IOException
    {
        java.io.File destination = new java.io.File(destinationName);
        java.io.File parent = destination.getParentFile();
        if ((parent != null) && (!parent.exists()) && (!parent.mkdirs())) {
            throw new java.io.IOException("Cannot create directory " + 
                                          parent.toString());
        }

        java.util.zip.ZipEntry entry = new java.util.zip.ZipEntry(zipEntry);
        java.util.zip.ZipFile zip = new java.util.zip.ZipFile(zipName);

        unzipFileAs(entry, zip, destination);
        
        zip.close();
    }


}

