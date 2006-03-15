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


#ifndef INCL_PLATFORMMNGT_RUNSET_MANAGER_HPP 
#define INCL_PLATFORMMNGT_RUNSET_MANAGER_HPP 


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "common/Exception.hpp"
#include "SystemMngt/platformlibrary/MessageQueue.hpp"
#include "SystemMngt/platformlibrary/Callback.hpp"
#include "SystemMngt/platformelementlibrary/DataStoreDefinition.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"


#include <set>
#include <list>
#include <map>
#include <string>

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW PlatformMngt
*/
namespace PlatformMngt
{


/**
* RunSetElementNoFoundException.
*/
class RunSetElementNoFoundException : public Cdmw::Exception
{
  public:
  
    RunSetElementNoFoundException() : Exception (
                      Exception::SAFE, "RunSet element not found")
    {
    }

  protected:

  private:

};

/**
* RunSetAckMisuseException.
*/
class RunSetAckMisuseException : public Cdmw::Exception
{
  public:
  
    RunSetAckMisuseException() : Exception (
                      Exception::SAFE, "RunSet element acknowledgement misuse")
    {
    }

  protected:

  private:

};

/**
* RunSetStarterDuplicatedException.
*/
class RunSetStarterDuplicatedException : public Cdmw::Exception
{
  public:
  
    RunSetStarterDuplicatedException() : Exception (
                      Exception::SAFE, "RunSet starter duplicated")
    {
    }

  protected:

  private:

};


/**
* RunSetStarterNotFoundException.
*/
class RunSetStarterNotFoundException : public Cdmw::Exception
{
  public:
  
    RunSetStarterNotFoundException() : Exception (
                      Exception::SAFE, "RunSet starter not found")
    {
    }

  protected:

  private:

};




class RunSetManager;

/**
 * The callback for the completion of the RunSet starter.
 */
class RunSetStarterCompletionCallback : public virtual Callback
{
    

public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param  runSetManager  the manager of RunSets.
     *@param  runSetName     the runset name
     */
    RunSetStarterCompletionCallback (RunSetManager* runSetManager,
                                     const std::string& runSetName);
        

    /**
     *Purpose:
     *<p> Destructor.
     *
     */
    ~RunSetStarterCompletionCallback ();
    
    
    /**
     *Purpose:
     *<p> Callback execution. notifies the RunSetManager,
     *    the starter has been completed.
     */
    void execute() throw();
 
    
private:

    /**
     * The manager of RunSet.
     */
    RunSetManager* m_runSetManager;
    
    /**
     * The RunSet name.
     */
    std::string m_runSetName;

};






/**
*Purpose: RunSet class which manage a runset containing a collection of
*         element names
*<p> 
*
*Features:
*<p> Thread safe
*
*@see  
*
*/
class RunSet
{

    public:

        /**
         *Purpose:
         *<p> The type used to contain the RunSet element names.
         */
        typedef std::set<std::string> RunSetDef;
         
         
         
        /**
        * Purpose:
        * <p> Constructor
        *
        *@exception Cdmw::OutOfResourcesException
        */ 
        RunSet (const CdmwPlatformMngt::RunSet& runSet)
                throw(Cdmw::OutOfResourcesException);



        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~RunSet()
                throw();                

        /**
		* Purpose:
		* <p> Check if element is contained in runset
		* 
		*@param element name of element
		*
		*@return True if element has been found.
        *
        *@exception Cdmw::OutOfResourcesException
		*/ 
		bool contains (const std::string& element)
		        throw (Cdmw::OutOfResourcesException);


        /**
		* Purpose:
		* <p> Get the RunSetDef container
		* 
		*@return RunSetDef container.
		*
		*/ 
		const RunSet::RunSetDef& getDef();

     
        /**
        * Purpose:
        * <p> Set the RunSetDef container
        *
        *@exception Cdmw::OutOfResourcesException
        */ 
        void setDef (const CdmwPlatformMngt::RunSet& runSet)
                throw(Cdmw::OutOfResourcesException);
                
          
        /**
		* Purpose:
		* <p> erase all element name in runSetDef
		* 
		*/ 
		void erase ();
		
		      
        /**
        * Purpose:
        * <p> reset the number of acknowledged elements
        * 
        */ 
        void reset ();
           
           
        /**
        * Purpose:
        * <p> Acknowledge an element
        * 
        *@param element name of element
        *
        *@return True if whole runset has been acknowledged.
        *
        *@exception RunSetElementNoFoundException
        *
        *@exception RunSetAckMisuseException
        */ 
        bool acknowledgeElement (const std::string& element)
		     throw (RunSetElementNoFoundException,
		            RunSetAckMisuseException);
                     


    protected:


    private:
    
        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        RunSet (const RunSet& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        RunSet&
        operator=(const RunSet& rhs)
                throw();
                
         
        /**
         * The number of acknowledged elements.
         */
        int m_nbAckElements;


        /**
         * The mutex used to ack element.
         */
        OsSupport::Mutex m_nbAckElements_mutex;
        
        
        
        /**
         * The RunSetDef containing elements
         */
        RunSet::RunSetDef m_runSetDef;
   

};  // End class RunSet 


class TaskFactory;
class Starter;
class System_impl;


/**
*Purpose: RunSetManager class which manage a collection of RunSets
*<p> 
*
*Features:
*<p> Thread safe
*
*@see  
*
*/
class RunSetManager : public OsSupport::Thread
{

    public:

        /**
         *Purpose:
         *<p> The type used to contain the tasks elements.
         */
        typedef std::list <std::string> ApplicationList;
 
        
        /**
         *Purpose:
         *<p> The type used to contain the RunSet elements.
         */
        typedef std::map <std::string, RunSet*> RunSets;
        
        /**
         *Purpose:
         *<p> The type used to contain the StartedRunSets elements.
         */
        typedef std::map <std::string, Starter*> StartedRunSets;
        
        
        /**
        * Purpose:
        * <p> Constructor
        * 
        *@exception Cdmw::OutOfMemoryException
        *
        */ 
        RunSetManager(System_impl* system)
                throw(OutOfMemoryException);



        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~RunSetManager()
                throw();


        /**
        *Purpose:
        * <p> Actually adds a new runset or modify an existing runset.
        *
        *@param run_set_def the definition of the runset.
        *
        *@exception DuplicateRunSetElement if an element already exists in the
        *           runset or among existing runsets.
        */
        void setRunSet(const CdmwPlatformMngt::RunSetDef& run_set_def)
            throw (CdmwPlatformMngt::DuplicateRunSetElement,
                   Cdmw::OutOfResourcesException);


     
        /**
        *Purpose:
        * <p> Actually removes the specified runset.
        *
        *@param run_set_name the name identifying the runset to remove.
        *
        *@exception RunSetNotFound if run_set_name doesn't denote
        *           an existing runset.
        */
        void removeRunSet(const char* run_set_name)
            throw (CdmwPlatformMngt::RunSetNotFound);


        /**
        *Purpose:
        *<p> Actually removes all runsets.
        *
        */
        void removeAllRunSets();
        
             
        /**
        * Purpose:
        * <p> reset the number of acknowledged elements in all runsets
        * 
        */ 
        void reset();
   
   
   
   
        /**
        * Purpose:
        * <p> Check if element is defined in one of the runsets
        * 
        *@param element name of element
        *
        *@return True if element has been found.
        *
        *@exception Cdmw::OutOfResourcesException
        */ 
        bool isInRunSet (const std::string& element)
		        throw (Cdmw::OutOfResourcesException);
        
                 

        /**
        * Purpose:
        * <p> Acknowledge an element
        * 
        *@param element name of element
        *
        *@param runSetName runset name returned, identifying the 
        *                  acknowledged runset
        *
        *@param p_runSetDef RunSetDef set returned,  
        *                   containing the element and 
        *                   about to be acknowledged
        *
        *@return True if whole runset has been acknowledged.
        *
        *@exception RunSetElementNoFoundException
        *@exception RunSetAckMisuseException
        *@exception Cdmw::OutOfResourcesException
        */ 
        bool acknowledgeElement (const std::string& element,
                                 std::string& runSetName,
                                 RunSet::RunSetDef*& p_runSetDef)
                throw (RunSetElementNoFoundException,
                       RunSetAckMisuseException,
                       OutOfResourcesException);
		               
		
        /**
        * Purpose:
        * <p> Start applications defined in RunSet
        * 
        *@param runSetName runset name identifying the runset
        *
        *@param p_runSetDef RunSetDef set containing elements  
        *
        *@exception Cdmw::OutOfResourcesException
        *@exception Cdmw::InternalErrorException
        *
        */ 
        void startRunSet (const std::string& runSetName,
                          RunSet::RunSetDef*& p_runSetDef)
                throw (OutOfResourcesException,
                       InternalErrorException);               


        /**
        * Purpose:
        * <p> Start applications defined in RunSet
        * 
        *@param runSetName runset name identifying the runset
        *
        *@param application list containing application names to start 
        *
        *@exception Cdmw::OutOfResourcesException
        *@exception Cdmw::InternalErrorException
        *
        */ 
        void startRunSet (const std::string& runSetName,
                          const RunSetManager::ApplicationList& applicationList)
                 throw (OutOfResourcesException,
                       InternalErrorException); 
                       
                       
        /**
         *Purpose:
         *<p> The RunSet starter's activity.
         */
        virtual void run() throw();


        /**
        * Purpose:
        * <p> set RunSet starter completed
        * 
        *@param runSetName runset name identifying the runset
        *
        *@exception Cdmw::RunSetStarterNotFoundException
        *
        */ 
        void addEndedRunSet (const std::string& runSetName)
                throw (RunSetStarterNotFoundException); 
		    
		    
		    
		    
        /**
        * Purpose:
        * <p> Stop all RunSet starter
        */ 
        void stopAllRunSets ()
                throw (); 
		        
		        
	    /**
        * Purpose:
        * <p> Stop RunSetManager
        */ 
        void stop ()
                throw (); 
		        
    protected:


    private:

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        RunSetManager (const RunSetManager& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        RunSetManager&
        operator=(const RunSetManager& rhs)
                throw();


        /**
        * Purpose:
        * <p> Check if element is defined in one of the runsets
        *     this method is not thread safe and must be used with
        *     a lock on m_rwLock.
        * 
        *@param element name of element
        *
        *@return True if element has been found.
        *
        *@exception Cdmw::OutOfResourcesException
        */ 
        bool isElementInRunSet (const std::string& element)
                throw (Cdmw::OutOfResourcesException);
		        
        
        /**
        * Purpose:
        * <p> Add RunSet starter in starter map
        * 
        *@param runSetName runset name identifying the runset
        *@param p_starter runset starter pointer 
        *
        *@exception Cdmw::RunSetStarterDuplicatedException
        *@exception Cdmw::OutOfResourcesException
        *
        */ 
        void addStarter (const std::string& runSetName,
                         Starter* p_starter)
                throw (RunSetStarterDuplicatedException,
                       OutOfResourcesException);      
		        
		        
        /**
        * Purpose:
        * <p> remove RunSet starter in starter map
        * 
        *@param runSetName runset name identifying the runset
        *
        *@return runset starter pointer 
        *
        *@exception Cdmw::RunSetStarterNotFoundException
        *
        */ 
        Starter* removeStarter (const std::string& runSetName)
                throw (RunSetStarterNotFoundException); 
                
                
        /**
        *Purpose:
        *<p> Set the runset record in data store
        *
        *@param run_set_def the definition of the runset.
        *
        */
        void set_record_in_datastore(const CdmwPlatformMngt::RunSetDef& run_set_def);
            
        /**
        *Purpose:
        *<p> Remove the runset record in datastore
        *
        *@param run_set_name the name identifying the runset to remove.
        *
        */
        void remove_record_in_datastore(const char* run_set_name);
		        
	            

        /**
         * The system reponsible for the applications.
         */
        System_impl* m_system;


        /**
        * The reader/writer lock used to manage concurrent thread safe access to
        * RunSet.
        */
        Cdmw::OsSupport::ReaderWriterLock m_rwRunSet_lock;
    
    
        /**
        * The mutex used to manage concurrent thread safe access to
        * Started RunSet.
        */
        Cdmw::OsSupport::Mutex m_startedRunSets_mutex;
    
        /**
         * The map containing all RunSets
         */
        RunSets m_runSets;
        
        /**
         * The map containing all StartedRunSets
         */
        StartedRunSets m_startedRunSets;
        
        /**
         * The mutex protecting the m_stopped member.
         */
        OsSupport::Mutex m_mutex;
        
        /**
         *Purpose:
         *<p> Indicates whether the RunSetManager task has been requested to stop.
         */
        bool m_stopped;

   
       /**
        * The ended runset starter tasks message queue.
        */
        MessageQueue<std::string> m_endedRunSets;
        
       
       /**
        * The task factory.
        */
        TaskFactory* m_taskFactory; 
        
        
      /**
       * Runset datastore
       */
      RunSetDataStore* m_runset_ds;


};  // End class RunSetManager 







}  // End namespace PlatformMngt
}  // End namespace Cdmw



#endif // INCL_PLATFORMMNGT_RUNSET_MANAGER_HPP


