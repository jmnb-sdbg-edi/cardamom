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


#include "platformsystemlibrary/RunSetManager.hpp"
#include "platformsystemlibrary/ApplicationRun.hpp"
#include "platformsystemlibrary/System_impl.hpp"
#include "platformsystemlibrary/SystemLogMessageId.hpp"

#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/TaskStarter.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"

#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"

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

// ----------------------------------------------------------------------
// RunSetStarterCompletionCallback class.
// ----------------------------------------------------------------------
RunSetStarterCompletionCallback::RunSetStarterCompletionCallback (
                                     RunSetManager* runSetManager,
                                     const std::string& runSetName)
    : Callback()
{
    m_runSetManager = runSetManager;
    m_runSetName = runSetName;
}


RunSetStarterCompletionCallback::~RunSetStarterCompletionCallback ()
{
}


void RunSetStarterCompletionCallback::execute()
  throw()
{
    // RunSet starter is terminated, so call RunSetManager to destroy it
    m_runSetManager->addEndedRunSet(m_runSetName);
}




// ----------------------------------------------------------------------
// RunSet class.
// ----------------------------------------------------------------------

//
// Constructor
//
RunSet::RunSet (const CdmwPlatformMngt::RunSet& runSet)
    throw(Cdmw::OutOfResourcesException)
{
    m_nbAckElements = 0;
    
    for (CORBA::ULong i=0 ; i < runSet.length() ; i++)
    {
        // store element name in runSetDef
        #if CDMW_ORB_VDR == orbacus
        m_runSetDef.insert(runSet[i]);
        #else
        m_runSetDef.insert((runSet[i]).in());
        #endif
    }
}


//
// Destructor
//
RunSet::~RunSet()
    throw()
{
}



//
// Check if element is contained in runset
//
bool RunSet::contains (const std::string& element)
    throw (Cdmw::OutOfResourcesException)
{
    try
    {
        bool result = false;

        RunSet::RunSetDef::iterator it = m_runSetDef.find(element);

        if (it != m_runSetDef.end())
        {
            result = true;
        }

        return result;
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(Cdmw::OutOfResourcesException);
    }
}


//
// Get the RunSetDef container
//
const RunSet::RunSetDef& RunSet::getDef()
{
    return m_runSetDef;
}


//
// Set the RunSetDef container
//
void RunSet::setDef (const CdmwPlatformMngt::RunSet& runSet)
    throw(Cdmw::OutOfResourcesException)
{
    CDMW_MUTEX_GUARD(m_nbAckElements_mutex);
    
    m_nbAckElements = 0;
    
    if (m_runSetDef.size() != 0)
    {
      // erase all element name in runSetDef if not already done
      m_runSetDef.clear();
    }
    
    for (CORBA::ULong i=0 ; i < runSet.length() ; i++)
    {
        // store input element name in runSetDef
        #if CDMW_ORB_VDR == orbacus
        m_runSetDef.insert(runSet[i]);
        #else
        m_runSetDef.insert((runSet[i]).in());
        #endif
    }
}
    

//
// erase all element name in runSetDef
//
void RunSet::erase ()
{
    
    CDMW_MUTEX_GUARD(m_nbAckElements_mutex);
    
    m_nbAckElements = 0;
    
    // erase all element name in runSetDef
    m_runSetDef.clear();
}

//
// reset the number of acknowledged elements
//
void RunSet::reset ()
{
    CDMW_MUTEX_GUARD(m_nbAckElements_mutex);
    
    m_nbAckElements = 0;
}
           

//           
// Acknowledge an element
//
bool RunSet::acknowledgeElement (const std::string& element)
    throw (RunSetElementNoFoundException,
		   RunSetAckMisuseException)
{
    bool result = false;
    
    CDMW_MUTEX_GUARD(m_nbAckElements_mutex);
    
    // check if element exists in runset
    if (contains (element))
    {
        int nbElements = m_runSetDef.size();
        
        if (m_nbAckElements >= nbElements)
        {
            CDMW_THROW (RunSetAckMisuseException);
        }
        else
        {
            m_nbAckElements ++;
            
            // if all elements have been acknowledged
            if (m_nbAckElements >= nbElements)
            {
                result = true;
            }
        }
        
        return result;
    }
    else
    {
        CDMW_THROW (RunSetElementNoFoundException);
    }
}
  




// ----------------------------------------------------------------------
// RunSetManager class.
// ----------------------------------------------------------------------

//
// Constructor
//
RunSetManager::RunSetManager(System_impl* system)
    throw(OutOfMemoryException)
{
    // Set the internal data
    m_stopped = false;
    m_system = system;
    m_taskFactory = NULL;
        
    // create application run task factory   
    m_taskFactory = RunApplicationTaskFactory::createFactory (m_system);
    
    try
    {
        Cdmw::CommonSvcs::DataStore::DataStoreBase* ds =
           Cdmw::CommonSvcs::DataStore::DataStoreRegistry::Get_data_store(
               Configuration::Get_runset_datastore_id());
        m_runset_ds = 
           dynamic_cast<Cdmw::PlatformMngt::RunSetDataStore*>(ds);
    }
    catch (const NotExistException& e)
    {
        CDMW_ASSERT(false);
    }
}



//
// Destructor
//
RunSetManager::~RunSetManager()        
     throw()
{
    // Delete the internal data
    if (m_taskFactory != NULL)
    {
        delete m_taskFactory;
    }   
}


 

//
// Adds a new runset or modify an existing runset.
//
void RunSetManager::setRunSet(const CdmwPlatformMngt::RunSetDef& run_set_def)
     throw (CdmwPlatformMngt::DuplicateRunSetElement,
            Cdmw::OutOfResourcesException)
{
    try
    {
        CDMW_WRITER_LOCK_GUARD(m_rwRunSet_lock);
        
        // search name of runset in map container
        std::string name = run_set_def.set_name.in();
        RunSets::iterator it = m_runSets.find(name);

        bool found = false;
        
        // if runset found, erase its runSetDef
        if (it != m_runSets.end())
        {
            found = true;
            (it->second)->erase();
        }
        
        // check element is not duplicated in all defined runSetDefs
        for (CORBA::ULong i=0 ; i < run_set_def.run_set.length() ; i++)
        {
            #if CDMW_ORB_VDR == orbacus
            std::string element = run_set_def.run_set[i];
            #else
            std::string element = (run_set_def.run_set[i]).in();
            #endif
            if (isElementInRunSet (element))
            {
                throw CdmwPlatformMngt::DuplicateRunSetElement(); 
            }
        }
        
        
        // if runset has been found, modify it with new runSetDef
        if (found)
        {
            (it->second)->setDef (run_set_def.run_set);
        }
        // else add new runset
        else
        {
            // construct a new runset and insert it in map
            std::auto_ptr <RunSet> runSet (new RunSet (run_set_def.run_set));
            m_runSets[name] = runSet.get();
            runSet.release();
        }
        
        // sto runset in datastore
        set_record_in_datastore(run_set_def);
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(Cdmw::OutOfResourcesException);
    }
}
     


//
// Actually removes the specified runset.
//
void RunSetManager::removeRunSet(const char* run_set_name)
     throw (CdmwPlatformMngt::RunSetNotFound)
{
    // remove runset in map container
    
    CDMW_WRITER_LOCK_GUARD(m_rwRunSet_lock);

    // search name of runset in map container
    RunSets::iterator it = m_runSets.find(run_set_name);
  
    // if runset found, erase and delete it
    if (it != m_runSets.end())
    {
        RunSet* p_runSet = it->second;
        m_runSets.erase(it);
        delete p_runSet;
    }
    else
    {
        throw CdmwPlatformMngt::RunSetNotFound();
    }
    
    // remove runset in datastore
    remove_record_in_datastore(run_set_name);
}


//
// Actually removes all runsets.
//
void RunSetManager::removeAllRunSets()
{
    CDMW_WRITER_LOCK_GUARD(m_rwRunSet_lock);
    
    RunSets::iterator it;

    for (it = m_runSets.begin(); it != m_runSets.end(); it++)
    {
      // remove runset in datastore
      remove_record_in_datastore((it->first).c_str());
    }
    
    m_runSets.clear();
}

// 
// Set the runset record in data store
//
void RunSetManager::set_record_in_datastore(const CdmwPlatformMngt::RunSetDef& run_set_def)
{
    if (Configuration::Is_primary())
    {        
        // name of runset
        std::string name = run_set_def.set_name.in();
 
        if (m_runset_ds->contains (name))
        {
            m_runset_ds->update(name, run_set_def.run_set);
        }
        else
        {
            m_runset_ds->insert(name, run_set_def.run_set);
        }
    }
}

//
// Remove the runset record in datastore
//
void RunSetManager::remove_record_in_datastore(const char* run_set_name)
{
   // inhibition if backup
   if (Configuration::Is_primary())
   {
       try
       {
           m_runset_ds->remove(run_set_name);
       }
       catch (Common::NotFoundException&)
       {
       }
   }
}

        
        
//            
// reset the number of acknowledged elements in all runsets
//
void RunSetManager::reset ()
{
    CDMW_READER_LOCK_GUARD(m_rwRunSet_lock);
    
    RunSets::iterator it;

    for (it = m_runSets.begin(); it != m_runSets.end(); it++)
    {
      (it->second)->reset();
    }
}   
   
//   
// Check if element is defined in one of the runsets
//
bool RunSetManager::isInRunSet (const std::string& element)
     throw (Cdmw::OutOfResourcesException)
{
    CDMW_READER_LOCK_GUARD(m_rwRunSet_lock);
    
    return isElementInRunSet (element);
}


   
//   
// Check if element is defined in one of the runsets
// No thread safe
//
bool RunSetManager::isElementInRunSet (const std::string& element)
     throw (Cdmw::OutOfResourcesException)
{
    bool result = false;
    
    try
    {
        RunSets::iterator it;

        for (it = m_runSets.begin(); it != m_runSets.end() && !result; it++)
        {
            if ((it->second)->contains (element))
            {
                result = true;
            }
        }
        
        return result;
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(Cdmw::OutOfResourcesException);
    }
}        
                 

//
// Acknowledge an element
//
bool RunSetManager::acknowledgeElement (const std::string& element,
                                        std::string& runSetName,
		                                RunSet::RunSetDef*& p_runSetDef)
     throw (RunSetElementNoFoundException,
		    RunSetAckMisuseException,
		    OutOfResourcesException)
{
    bool result = false;
    bool found = false;
    
    try
    {
        RunSets::iterator it;
        RunSet* p_runSet = NULL;
        
        // set to NULL the RunSetDef returned pointer 
        // (runset not aknowledged)
        p_runSetDef = NULL;
        
        CDMW_READER_LOCK_GUARD(m_rwRunSet_lock);

        // scan runset map to find element
        for (it = m_runSets.begin(); it != m_runSets.end() && !found; it++)
        {
            p_runSet = it->second;
            
            // if runset contains element
            if (p_runSet->contains (element))
            {
                // acknowledge the runset
                result = p_runSet->acknowledgeElement (element);
                found = true;
                
                // if all elements acknowledged
                if (result)
                {
                    // return RunSet name
                    runSetName = it->first;
                    
                    // return RunSetDef content to the caller
                    std::auto_ptr <RunSet::RunSetDef> runSetDef (
                                 new RunSet::RunSetDef(p_runSet->getDef()));
                    
                    p_runSetDef = runSetDef.release();
                }
            }
        }
        
        // if element has not been found, throw exception
        if (!found)
        {
            CDMW_THROW (RunSetElementNoFoundException);
        }
        
        return result;
        
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(Cdmw::OutOfResourcesException);
    }
}


//
// Start applications defined in RunSet
//
void RunSetManager::startRunSet (const std::string& runSetName,
                                 RunSet::RunSetDef*& p_runSetDef)
     throw (OutOfResourcesException,
            InternalErrorException)
{
    try
    {
        // Set the list of applications to be run concurently            
        RunSetManager::ApplicationList appList;
        RunSet::RunSetDef::iterator applicationIt;

        for (applicationIt = p_runSetDef->begin() ;
             applicationIt != p_runSetDef->end() ;
             applicationIt++)
        {
            // insert application name in list
            appList.push_back (*applicationIt);
        }
         
        // start RunSet with application list
        startRunSet (runSetName, appList);

    }
    catch (const std::bad_alloc&)
    {
        CDMW_THROW(OutOfResourcesException);
    }
}


//
// Start applications defined in RunSet
//
void RunSetManager::startRunSet (const std::string& runSetName,
                                 const RunSetManager::ApplicationList& applicationList)
     throw (OutOfResourcesException,
            InternalErrorException)
{
    try
    {            
        // Create the application starter
        std::auto_ptr <RunSetStarterCompletionCallback> completionCallback (
                     new RunSetStarterCompletionCallback (this, runSetName));

        Starter* p_starter = new Starter (m_taskFactory);
        std::auto_ptr <Starter> starter (p_starter);
             
        p_starter->setCompletionCallback (completionCallback.release());
        
        // add starter in StartedRunSets map
        addStarter (runSetName, p_starter);
                
        // set tasks to start and start the starter thread
        p_starter->addTasksToStart (applicationList);
        
        try
        {
            p_starter->start();
        }
        catch (...)
        {
            // start not ok, so remove starter from map
            removeStarter (runSetName);
            throw;
        }
        
        // no exception : release auto_ptr
        starter.release();
    }
    catch (const std::bad_alloc&)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch (const OutOfMemoryException&)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch (...)
    {
        CDMW_THROW(InternalErrorException);
    }
}


//
// Add RunSet starter in starter map
//
void RunSetManager::addStarter (const std::string& runSetName,
                                Starter* p_starter)
     throw (RunSetStarterDuplicatedException,
            OutOfResourcesException)
{
    CDMW_MUTEX_GUARD(m_startedRunSets_mutex);

    try
    {
        StartedRunSets::iterator it = m_startedRunSets.find(runSetName);

        if (it == m_startedRunSets.end())
        {
           // add the starter
           m_startedRunSets[runSetName] = p_starter;
        }
        else
        {
           CDMW_THROW(RunSetStarterDuplicatedException);
        }
    }
    catch (const std::bad_alloc&)
    {
        CDMW_THROW(OutOfResourcesException);
    }
}


//
// Remove RunSet starter in starter map
//
Starter*  RunSetManager::removeStarter (const std::string& runSetName)
     throw (RunSetStarterNotFoundException)
{
    Starter* p_starter = NULL;
    
    CDMW_MUTEX_GUARD(m_startedRunSets_mutex);

    StartedRunSets::iterator it = m_startedRunSets.find(runSetName);

    if (it == m_startedRunSets.end())
    {
        CDMW_THROW(RunSetStarterNotFoundException);
    }
    else
    {
        p_starter = it->second;
        m_startedRunSets.erase (it);
    }
        
    return p_starter;
}


//
// The RunSet starter's activity, waiting to destroy the terminated starter
//
void RunSetManager::run()
  throw()
{
    bool stopped = false;

    try
    {
        while(!stopped)
        {
            // get the name of terminated RunSet from message queue
            std::auto_ptr<std::string> runSetId(m_endedRunSets.pop());

            m_mutex.lock();
            stopped = m_stopped;
            m_mutex.unlock();

            // if stop is requested, break the loop
            if (stopped)
            {
                break;
            }

            // remove starter from the map, then stop and destroy it
            try
            {
                Starter* p_starter = removeStarter(*runSetId);
                if (p_starter != NULL)
                {
                    p_starter->stop();
                    delete p_starter;
                }
            }
            catch (...)
            {
                LogMngr::logMessage(FTL, "Unexpected exception in while loop of RunSetManager");
            }

        }
    }
    
    catch(const MessageQueueDestroyedException&)
    {
        // Nothing to do
    }
    catch(...)
    {
        LogMngr::logMessage(FTL, "Unexpected exception in RunSetManager");
    }
}



//
// set RunSet starter completed
//
void RunSetManager::addEndedRunSet (const std::string& runSetName)
         throw (RunSetStarterNotFoundException)
{
    try
    {
        std::auto_ptr<std::string> runSetId(new std::string(runSetName));

        // push RunSet name into message queue
        m_endedRunSets.push(runSetId.get());

        runSetId.release();

    }
    catch(const MessageQueueDestroyedException&)
    {
        // The RunSetManager has been stopped
        // Nothing to do
    }
    catch(...)
    {
        LogMngr::logMessage(FTL, MSG_ID_RUNSETMANAGER_MEMORY); 
        stop();
    }
}
                 

//
// Stop all RunSet starter
// 
void RunSetManager::stopAllRunSets ()
          throw ()
{
    CDMW_MUTEX_GUARD(m_startedRunSets_mutex);
    
    // remove all starters from the map, then stop and destroy them
    StartedRunSets::iterator it;

    for (it = m_startedRunSets.begin() ; it != m_startedRunSets.end() ; it++)
    {
        Starter* p_starter = it->second;
        if (p_starter != NULL)
        {
            p_starter->stop();
            delete p_starter;
        }
    }
    
    // destroy all elements in started RunSet map
    m_startedRunSets.clear();
}
		        
		       
//		        
// Stop RunSetManager
// 
void RunSetManager::stop()
          throw ()
{    
    bool stop;

    m_mutex.lock();

    // check if RunSetManager thread is running
    stop = (get_status() != OsSupport::THREAD_CREATED) && (!m_stopped);
    m_stopped = true;

    m_mutex.unlock();

    if (stop)
    {
        // Destroys the message queue in order to release the RunSetManager thread
        m_endedRunSets.destroy();

        // Waits for the ending of the RunSetManager thread
        this->join();
    }
} 		
 



}  // End namespace PlatformMngt
}  // End namespace Cdmw




