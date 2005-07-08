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


package cdmw.ossupport;

import java.io.File;
import java.io.PrintStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;

import java.util.Vector;

import java.net.InetAddress;
import java.net.UnknownHostException;


/**
 * This class provides several helper tools to execute 
 * some system level operations.
 * All functions of this class are static.
 *
 * Features:
 * Thread safe, exception safe
 *
 * @see ThreadedProcess
 *
 */
public class OS {
    
    private static String classpath;
    
    private static String bootclasspath;
    
    private static String javaCommand;
    
    static {
        
        classpath = System.getProperty("java.class.path");

        bootclasspath = System.getProperty("sun.boot.class.path");

        javaCommand = System.getProperty("java.home") 
            + File.separator + "bin" 
            + File.separator + "java";
        
    }

    /**
     * This functions returned the value of an option passed
     * on the command line with the format: --option=value, or
     * --option. If no value are specified after an option "yes"
     * is returned if the option is present, "no" elsewhere.
     *
     * @param args arguments as received in the main function
     * @param option option to test if it is present on the command line
     *
     * @return the value of the option, "yes" if the option is present
     *         without value, "no" if the option is not present.
     */
    public static String getOptionValue(String[] args, String option) {

        String optionValue = "no";

        // We scan all arguments to find the matching with option
        for(int i=0;i<args.length;i++) {

            String arg = args[i];
            int npos = arg.indexOf(option);

            // We check if in the current argument we have, the option
            if ( npos != -1 ) {

                int posSep = arg.indexOf('=');

                // We have no separator the option is only here so
                //    we return yes, or if the equal is not followed
                //    by something
                if ( posSep == -1 ) {
                    optionValue =  "yes";
                // else we return the value
                } else {
                       optionValue = arg.substring( posSep+1 );
                }
            }

        }

        return optionValue;

    }


    /**
     * The calling thread is blocked during the specified
     * time expressed in milliseconds.
     *
     * @param milli time to sleep expressed in milliseconds
     *
     * @exception InterruptedException
     */
    public static void sleep(int milli) throws InterruptedException {

        Thread.sleep(milli);

    }


    /**
     * Test if a file exists
     *
     * @param path path to the file to test for existence
     *
     * @return true if the file exists, false elsewhere
     */
    public static boolean fileExists(String path) {

        return new File(path).exists();

    }


    /**
     * Delete the specified file.
     *
     * @param path path to the file to be deleted
     *
     * @return true if the file is successfully deleted, false elsewhere
     */
    public static boolean unlink(String path) {

        return new File(path).delete();

    }


    /**
     * Return the absolute path associated to the current
     * working directory and specified path
     *
     * @param path path to the file
     */
    public static String getAbsolutePath(String path) {

        return new File(path).getAbsolutePath();

    }


    /**
     * Blocks the calling thread until the completion of the process
     *
     * @param tp process to be waited for
     *
     * @return exit value of the process
     *
     * @exception InterruptedException
     */
    public static int waitProcess(ThreadedProcess tp)
        throws InterruptedException {

        int exit = tp.waitProcess();
        tp.join();
        return exit;

    }


    /**
     * Create a new process from the specified executable
     *
     * @param command a specified system command.
     * @return The new process as a ThreadedProcess object
     * @exception IOException
     */
    public static ThreadedProcess createProcess(String command)
        throws IOException {
        return startProcess(new ThreadedProcess(command));
    }
    
    /**
     * Create a new process from the specified executable 
     * and the specified environment settings.
     *
     * @param command a specified system command.
     * @param env Environment settings.
     * @return The new process as a ThreadedProcess object
     * @exception IOException
     */
    public static ThreadedProcess createProcess(String command, String[] env)
        throws IOException {
        return startProcess(new ThreadedProcess(command, env));
    }
    
    /**
     * Starts the specified ThreadedProcess.
     *
     * @param process The process to start
     * @throws IOException If 
     */
    private static ThreadedProcess startProcess(ThreadedProcess process) 
        throws IOException {
        
        // start the process
        process.start();
        
        // wait till it's started
        while( !process.isStarted() ) {
            Thread.yield();
        }
        
        // if an exception was thrown, rethrow it
        if (process.getProcess() == null) {
            
            IOException ioe = process.getIOException();
            if (ioe != null) throw ioe;
            else throw new IOException("Unknown reason");
            
        // otherwise redirect the standard output and error 
        // of the created process to the current JVM
        } else {
            
            InputStream in = process.getInputStream();
            InputStream err = process.getErrorStream();

            BufferedReader inReader = 
                new BufferedReader(new InputStreamReader(in));
            BufferedReader errReader = 
                new BufferedReader(new InputStreamReader(err));

            PrintStreamThread outThread = 
                new PrintStreamThread(inReader, System.out);
            PrintStreamThread errThread = 
                new PrintStreamThread(errReader, System.err);

            outThread.start();
            errThread.start();
            
        }
        
        // return the started process
        return process;
        
    }

    /**
     * Create a new process in a new JVM from the specified classname
     * <p/><b>NB:</b> The classname should be in the current classpath.
     *
     * @param classname Name of a Java classname.
     * @return The new process as a ThreadedProcess object
     * @exception IOException
     */
    public static ThreadedProcess createJavaProcess(String classname)
        throws IOException {
        
        String[] nullArray = null;
        return createJavaProcess(classname, nullArray);
        
    }
    
    /**
     * Create a new process in a new JVM from the specified classname
     * with specified environment settings
     * <p/><b>NB:</b> The classname should be in the current classpath.
     *
     * @param classname Name of a Java classname.
     * @param env Environment settings.
     * @return The new process as a ThreadedProcess object
     * @exception IOException
     */
    public static ThreadedProcess 
        createJavaProcess(String classname, String[] env) throws IOException {

        return createJavaProcess(classname, env, null);
    }
    
    /**
     * Create a new process in a new JVM from the specified classname
     * with specified system properties
     * <p/><b>NB:</b> The classname should be in the current classpath.
     *
     * @param classname Name of a Java classname.
     * @param properties System properties to set.
     * @return The new process as a ThreadedProcess object
     * @exception IOException
     */
    public static ThreadedProcess createJavaProcess(
        String classname, 
        Vector properties) throws IOException {

        return createJavaProcess(classname, null, properties);

    }
    
    /**
     * Create a new process in a new JVM from the specified classname
     * with specified environment settings and system properties
     * <p/><b>NB:</b> The classname should be in the current classpath.
     *
     * @param classname Name of a Java classname.
     * @param env Environment settings.
     * @param properties System properties to set.
     * @return The new process as a ThreadedProcess object
     * @exception IOException
     */
    public static ThreadedProcess createJavaProcess(
        String classname, 
        String[] env,
        Vector properties) throws IOException {

        String props = new String("");
        if (properties != null) {
            java.util.Enumeration e = properties.elements();
            while (e.hasMoreElements()) {
                props = props + "-D" + (String) e.nextElement() + " ";
            }
        }
        
        String command = javaCommand + " " + props + 
                         " -Xbootclasspath:" + bootclasspath + 
                         (classname.equals("") ? "": " -cp " + classpath) + 
                         " " + classname;
        return startProcess(new ThreadedProcess(command, env));

    }

    /**
     * Kill a process.
     *
     * @param tp The ThreadedProcess object to be killed
     */
    public static void killProcess(ThreadedProcess tp) {

        if (tp != null) tp.killProcess();
        // tp.destroy();
        // as it is written in the JDK 1.3: "This method is not implemented."

    }

    /**
     * Returns the host name
     *
     * @return  the host name
     */
    public static String getHostname() {

        try {
            return InetAddress.getLocalHost().getHostName();
        }
        catch (UnknownHostException uhe) {
            return "Unknown Host";
        }

    }

    
    
    /**
     * Loads the system library specified by the libraryName argument.
     *
     * This operation should be used instead of the 
     * <code>System.loadLibrary()</code> operation, since the Cdmw
     * jar files are set into the Xbootclasspath. (this is mandatory
     * with Orbacus Java).
     *
     * @param      libraryName   the name of the library.
     * @exception  UnsatisfiedLinkError  if the library cannot be found into the 
     *             java.library.path (which contains the environment library path).
     */
    public static void loadLibrary(String libraryName) 
        throws UnsatisfiedLinkError  
    {
         String fullLibName = System.mapLibraryName(libraryName);

         // Try to find libcdmwjniossupport.so in java.library.path
         //
         // System.loadLibrary() cannot be used, because this class could be 
         // loaded by the bootstrap ClassLoader. The bootstrap ClassLoader
         // only loads libraries from the original "sun.boot.library.path".
         // (dynamically modify "sun.boot.library.path" doesn't work, because
         //  the bootstrap ClassLoader is always initialized with the original
         //  value of "sun.boot.library.path", before the modification)
         //
         java.util.StringTokenizer tokenizer = 
            new java.util.StringTokenizer(System.getProperty("java.library.path"),
                                          java.io.File.pathSeparator);
         while (tokenizer.hasMoreTokens()) {
             java.io.File libfile = new java.io.File(tokenizer.nextToken(), 
					                                 fullLibName);             
             try {
                 System.load(libfile.getAbsolutePath());
                 // library loaded
                 return;
             } catch (UnsatisfiedLinkError e) {
                 // ignore and try with next path.
             }
         }
         
         // library was not found into any path.
         throw new UnsatisfiedLinkError("Cannot find library '" + fullLibName + 
                                        "' into the java.library.path :\n" +
                                        System.getProperty("java.library.path"));
    }
    
    
    // load native library for getTime()
    static {
        loadLibrary("cdmwjniossupport");
        
        // call native operation once to initialise methodID in C code
        // (this could take several milliseconds, influencing the first time measure)
        Timeval foo = getTime();
    }

    /**
     * Returns the number of sec/microsec since 1th of Jan 1970
     *
     * @return  the current date
     */
    public static native Timeval getTime();
  
  
    /**
     * Returns the current formatted date
     * 
     * @param dateStyle The given date formatting style
     * @param timeStyle The given time formatting style
     */
    public static java.text.DateFormat getFmtTime(int dateStyle,int timeStyle) {
    
        return java.text.DateFormat.getDateTimeInstance(dateStyle,timeStyle);
    
    }
    
    /**
     * Thread which prints data coming from a data source 
     * to a PrintStream object.
     */
    private static class PrintStreamThread extends Thread {

        /**
         * Reader which provides data to be read
         */
        private BufferedReader reader;
    
        /**
         * PrintStream object
         */
        private PrintStream out;
    
        /**
         * Constructor
         *
         * @param reader Buffered reader of the data.
         * @param out PrintStream object to print the input data.
         */
        public PrintStreamThread(BufferedReader reader, PrintStream out) {
            this.reader = reader;
            this.out = out;
        }
    
        /**
         * Every line read is sent to the PrintStream object.
         */
        public void run() {
            String line = null;
            try {
                while( (line = reader.readLine()) != null) {
                    out.println(line);
                }
            } catch(IOException ioe) {
                ioe.printStackTrace(out);
            }
        }
    
    }

}

