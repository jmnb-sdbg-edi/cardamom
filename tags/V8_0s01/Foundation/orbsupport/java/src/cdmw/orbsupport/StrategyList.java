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


package cdmw.orbsupport;

import cdmw.common.BadParameterException;

/**
 * This class defines is used to set ORB dependent
 * capability in an ORB independent way.
 *
 * Features:
 * Thread safe, exception safe
 *
 * @see  ORBSupport
 *
 */
public class StrategyList {

    /**
     * If true, the user has asked for a multi-threaded ORB.
     */
    private boolean hasORBThreaded;

    /**
     * If true, the user has asked for a Threaded POA
     */
    private boolean hasPOAThreadPerRequest;

    /**
     * If true, the user has asked for a Threaded POA
     */
    private boolean hasPOAThreadPerConnection;

    /**
     * If true, the user has asked for a POA with Thread Pool
     */
    private boolean hasPOAThreadPool;

    /**
     * this variable contains the thread number to be used by the ORB.
     */
    private int nbPOAThreads;

    /**
     * If true, the user has asked to use a specific port
     * for the POA
     */
    private boolean hasPOAPortAssigned;


    /**
     * Value of the port to be used for the POA, if
     * hasPOAPortAssigned has been set.
     */
    private int poaPort;

    /**
     * Constructor
     *
     */
    public StrategyList() {

        hasORBThreaded = false;
        hasPOAThreadPerRequest = false;
        hasPOAThreadPerConnection = false;
        hasPOAThreadPool = false;
        nbPOAThreads = 0;
        hasPOAPortAssigned = false;
        poaPort = 0;

    }

    /**
     * Set the ORB shall use multi-threading.
     *
     */
    public void addORBThreaded() {

        this.hasORBThreaded = true;

    }

    /**
     * Set the POA shall use multi-threading. The threads
     *  will be created at the initialisation time. 'nbThreads'
     *  will be created.
     *
     *  This Strategy is incompatible with:
     *   - addPOAThreadPerRequest
     *   - addPOAThreadPerConnection
     *
     *  This Strategy requires:
     *   - ORBInit() call with addORBThreaded Strategy
     *
     * @param nbThreads the size of the thread pool
     *
     * @exception BadParameterException if the nbThreads is equal to zero
     * @exception BadParameterException if the Strategy is incompatible
     *           with a previous one
     */
    public void addPOAThreadPool(int nbThreads)
        throws BadParameterException {

        // Check incompatible Strategy
        if ( hasPOAThreadPerRequest ) {
            throw new BadParameterException("POAThreaded already set");
        }

        if ( hasPOAThreadPerConnection ) {
            throw new BadParameterException("POAThreaded already set");
        }

        // Check already set Strategy
        if ( hasPOAThreadPool ) {
            throw new BadParameterException("POAThreadPool already set");
        }

        // Check Parameters
        if ( nbThreads > 0 ) {
            hasPOAThreadPool = true;
            nbPOAThreads = nbThreads;
        } else {
            throw new BadParameterException("minimum value of nbThreads is 1");
        }

    }

    /**
     * Set the POA shall use multi-threading: one thread per request
     *
     *  This Strategy is incompatible with:
     *   - addPOAThreadPerConnection
     *   - addPOAThreadPool
     *
     *  This Strategy requires:
     *   - ORBInit() call with addORBThreaded Strategy
     *
     * @exception BadParameterException if the Strategy is incompatible
     *           with a previous one
     */
    public void addPOAThreadPerRequest() throws BadParameterException {

        // Check incompatible Strategy
        if ( hasPOAThreadPool ) {
            throw new BadParameterException("POAThreadPool already set");
        }

        if ( hasPOAThreadPerConnection ) {
            throw new BadParameterException("POAThreaded already set");
        }

        // Check already set Strategy
        if ( hasPOAThreadPerRequest ) {
            throw new BadParameterException("POAThreaded already set");
        }

        // Check Parameters
        hasPOAThreadPerRequest = true;

    }

    /**
     * Set the POA shall use multi-threading: one thread per request
     *
     *  This Strategy is incompatible with:
     *   - addPOAThreadPool
     *   - addPOAThreadPerRequest
     *
     *  This Strategy requires:
     *   - ORBInit() call with addORBThreaded Strategy
     *
     * @exception BadParameterException if the Strategy is incompatible
     *           with a previous one
     */
    public void addPOAThreadPerConnection() throws BadParameterException {

        // Check incompatible Strategy
        if ( hasPOAThreadPool ) {
            throw new BadParameterException("POAThreadPool already set");
        }

        if ( hasPOAThreadPerRequest ) {
            throw new BadParameterException("POAThreaded already set");
        }

        // Check already set Strategy
        if ( hasPOAThreadPerConnection ) {
            throw new BadParameterException("POAThreaded already set");
        }

        // Check Parameters
        hasPOAThreadPerConnection = true;

    }

    /**
     * Returns if the Multi-threaded strategy has been
     * choosen for the ORB
     *
     * @return true if the strategy has been set, false elsewhere
     */
     public boolean isORBThreaded() {
         return hasORBThreaded;
     }

    /**
     * Returns true if the Thread Pool strategy has been
     * choosen for the POA
     *
     * @return true if the strategy has been set, false elsewhere
     */
    public boolean isPOAThreadPool() {
        return hasPOAThreadPool;
    }

    /**
     * Returns true if the ThreadPerConnection strategy has been
     * choosen for the POA
     *
     * @return true if the strategy has been set, false elsewhere
     */
    public boolean isPOAThreadPerConnection() {
        return hasPOAThreadPerConnection;
    }

    /**
     * Returns the number of threads of the POA
     *
     * @return threads number of the POA
     */
    public int getNbPOAThreads() {
        return this.nbPOAThreads;
    }

    /**
     * Returns true if the ThreadPerRequest strategy has been
     * choosen for the POA.
     *
     * @return true if the strategy has been set, false elsewhere
     */
    public boolean isPOAThreadPerRequest() {
        return hasPOAThreadPerRequest;
    }


   /**
    * Returns true if the user has asked for a specific
    * port for one POA
    *
    * @deprecated This should not be used by application
    * developpers as it will be removed in the next release.
    *
    * @return true if the strategy has been set, false elsewhere
    */
    protected boolean isPOAPortAssigned() {
        return hasPOAPortAssigned;
    }

    /**
     * Returns the port of the POA
     *
     * @return port of the POA
     */
    protected int getPOAPort() {
        return this.poaPort;
    }

    /**
     * Define the POA must use the specified port for
     * managing incoming CORBA request.
     *
     * @deprecated This should not be used by application
     * developpers as it will be removed in the next release.
     *
     * @param port the port to be used ( 0 < port < 65535 )
     *
     * @exception BadParameterException if the Strategy is incompatible
     *          with a previous one
     * @exception BadParameterException if the port number
     *            is not in the allowed range.
     */
    protected void addPOAPortAssigned(int poaPort)
        throws BadParameterException {


        // Check incompatible Strategy
        // None

        // Check already set Strategy
        if ( hasPOAPortAssigned ) {
            throw new BadParameterException("POAPortAssigned already set");
        }

        // Check Parameters
        if ( ! (poaPort > 0 && poaPort < 65535)  ) {
            throw new
                BadParameterException("The Port is not in the allowed range");
        }

        this.hasPOAPortAssigned = true;
        this.poaPort = poaPort;

    }

    /**
     * Copy the current StrategyList object
     *
     * @return copy of the current object
     */
    public StrategyList copy() {

        StrategyList copy = new StrategyList();

        if ( this.isORBThreaded() ) {
            copy.addORBThreaded();
        }
        if ( this.isPOAThreadPerRequest() ) {
            try {
                copy.addPOAThreadPerRequest();
            } catch(BadParameterException bpe) {}
        }
        if ( this.isPOAThreadPool() ) {
            try {
                copy.addPOAThreadPool( this.getNbPOAThreads() );
            } catch(BadParameterException bpe) {}
        }
        if ( this.isPOAPortAssigned() ) {
            try {
                copy.addPOAPortAssigned( this.getPOAPort() );
            } catch(BadParameterException bpe) {}
        }

        return copy;
    }

}

