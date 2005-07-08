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


#ifndef INCL_UPDATE_ENTITY_HPP 
#define INCL_UPDATE_ENTITY_HPP 

#include <string>


#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/ossupport/OS.hpp"


using namespace Cdmw;
using namespace Cdmw::OsSupport;



/**
*Purpose:
*<p> 
* Abstract EntityToUpdate.
*/
class EntityToUpdate
{

    public:

       /**
        * Purpose:
        * <p> Constructor.
        */ 
        EntityToUpdate(std::string& entityName,
                       std::string& processName) 
            : m_flipFlop (false),
              m_updatingCounter (0),
              m_errorSet (false),
              m_entityName (entityName),
              m_processName (processName)
        { 
            
        }
        
       /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~EntityToUpdate() { }


        /**
         *Purpose:
         *<p> Executes the updating.
         */
        virtual void update() throw() = 0;
        
     protected:
     
        /**
        * flip flop flag for entity updating
        */
        bool m_flipFlop;
        
       /**
        * updating counter
        */
        unsigned long m_updatingCounter;
        
   
        /**
        * error set during updating
        */
        bool m_errorSet;
        
        /**
        * The entity name to set.
        */
        std::string m_entityName;
        
 
        /**
        * The process name.
        */
        std::string m_processName;
     

}; // End class EntityToUpdate



/**
*Purpose:
*<p> 
* SystemEntityToUpdate.
*/
class SystemEntityToUpdate : public EntityToUpdate
{

    public:

        /**
        * Purpose:
        * <p> Constructor.
        */ 
        SystemEntityToUpdate(std::string& entityName,
                             std::string& processName);

        
        /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~SystemEntityToUpdate();


        /**
         *Purpose:
         *<p> Executes the updating.
         */
        virtual void update() throw();
        


}; // End class SystemEntityToUpdate


/**
*Purpose:
*<p> 
* ApplicationEntityToUpdate.
*/
class ApplicationEntityToUpdate : public EntityToUpdate
{

    public:

        /**
        * Purpose:
        * <p> Constructor.
        */ 
        ApplicationEntityToUpdate(std::string& entityName,
                                  std::string& processName);

        
        
        /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~ApplicationEntityToUpdate();


        /**
         *Purpose:
         *<p> Executes the updating.
         */
        virtual void update() throw();


}; // End class ApplicationEntityToUpdate

/**
*Purpose:
*<p> 
* ProcessEntityToUpdate.
*/
class ProcessEntityToUpdate : public EntityToUpdate
{

    public:

        /**
        * Purpose:
        * <p> Constructor.
        */ 
        ProcessEntityToUpdate(std::string& entityName,
                              std::string& processName);
     
        
        /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~ProcessEntityToUpdate();


        /**
         *Purpose:
         *<p> Executes the updating.
         */
        virtual void update() throw();
        
        


}; // End class ProcessEntityToUpdate



/**
*Purpose:
*<p>    This class implement the updating of entity
*
*Features:
*<p> Thread safe, exception safe
*
*
*/
class UpdateEntity : public OsSupport::Thread
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param UpdateEntityable    reference to UpdateEntityable object 
        *                          (can be nil)
        *@param monitoringInterval monitoring interval used to reactivate the thread in msec
        *@param monitoringTimeout  timeout used by UpdateEntityingCall object
        *                          to wait response of UpdateEntityable object during polling
        *
        */ 
        UpdateEntity (EntityToUpdate* p_entityToUpdate,
                      unsigned long updatingInterval);

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~UpdateEntity()
                throw();




        /**
        * Purpose:
        * <p> Ask to the internal thread to stop its job, 
        *     When this function return the internal thread have stopped.
        *
        *
        */ 
        void stop()
            throw (InternalErrorException);
            
            

    protected:


        /**
        * Purpose:
        * <p> Call by the underlying thread system in a new thread
        * 
        */ 
        virtual
        void run()
            throw();
     
        
    private:

        //
        // Copy ctor
        //
        UpdateEntity (const UpdateEntity& rhs)
                throw();




        //
        // Assignement operator
        //
        UpdateEntity& operator=(const UpdateEntity& rhs)
                 throw();


        /**
        * At each iteration the thread test this variable, to
        * see if it must stop it's processing. If true
        * the thread terminates.
        */
        bool m_stopped;    
        
        
        
        /**
        * Mutex protecting stop flag and associated to condition
        */
        OsSupport::Mutex m_mutex;



        /**
        * Condition on which the thread is sleeping
        * if there is no more job.
        */
        OsSupport::Condition m_timerCondition;


        /**
        * updating interval used to reactivate the thread in msec
        */
        unsigned long m_updatingInterval;

          
        /**
        * pointer to EntityToUpdate object
        */
        EntityToUpdate* m_pEntityToUpdate;


}; // End class UpdateEntity 



#endif // INCL_UPDATE_ENTITY_HPP

