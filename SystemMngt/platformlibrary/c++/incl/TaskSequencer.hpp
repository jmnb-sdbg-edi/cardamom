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


#ifndef INCL_PLATFORMMNGT_TASKSEQUENCER_HPP 
#define INCL_PLATFORMMNGT_TASKSEQUENCER_HPP 

#include "SystemMngt/platformlibrary/TaskStarter.hpp"
#include "Foundation/osthreads/Condition.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


class SynchronisableTask;
class SynchronisableTaskFactory;
class CompletionResumer;

/**
*Purpose:
* The concurrent tasks sequencer.
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
class Sequencer : public Starter
{

    friend class SynchronisableTask;
    friend class CompletionResumer;


    public:


        /**
        * Purpose:
        * <p> Constructor.
        */ 
        Sequencer(SynchronisableTaskFactory* taskFactory);


        /**
        * Purpose:
        * <p> Destructor.
        */ 
        ~Sequencer() throw();


        /**
         *Purpose:
         *<p> Sets the suspension callback.
         * This method takes ownership of the callback.
         *
         *@param callback the callback.
         *
         * @exception BadParameterException if the callback is NULL.
         */
        void setSuspensionCallback(Callback* callback)
            throw (BadParameterException);


        /**
         *Purpose:
         *<p> Stops the sequencer.
         */
        void stop() throw();


        /**
         *Purpose:
         *<p> Resumes the sequencer.
         *
         * @exception IncompatibleStatusException if the sequencer is
         * not suspended.
         */
        void resume() throw(IncompatibleStatusException);



    protected:


        /**
         *Purpose:
         *<p> The sequencer's activity.
         */
        virtual void run() throw();


    private:

        /**
        * Purpose:
        * <p>
        * Copy constructor is not allowed.
        */ 
        Sequencer(const Sequencer& rhs);


        /**
        * Purpose:
        * <p>
        * Assignment operator is not allowed.
        */ 
        Sequencer& operator=(const Sequencer& rhs);


        /**
         *Purpose:
         *<p> Signals the suspension to the sequencer.
         * Used only by SynchronisableTask.
         */
        void suspend() throw();


    private:

        /**
         * Indicates whether the sequencer is suspended.
         * Protected by the m_mutex base member.
         */
        bool m_suspended;

        /**
         * Condition for the resumption of the sequencer.
         * Based on the m_mutex base member.
         */
        OsSupport::Condition m_resumption_condition;
 
        /**
        * The associated task factory.
        */
        SynchronisableTaskFactory* m_taskFactory;

        /**
         * The callback executed when the sequencer is suspended.
         */
        Callback* m_suspensionCallback;
                
        /**
        * The associated completion resumer.
        */
        CompletionResumer* m_completionResumer;


}; // End class Sequencer 



/**
*Purpose:
* The observer of synchronisable task completion.
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
class SynchronisableTaskObserver : public TaskObserver
{

    public:


        /**
        * Purpose:
        * <p> Constructor.
        */ 
        SynchronisableTaskObserver(Starter* starter,
            SynchronisableTaskFactory* taskFactory);


        /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~SynchronisableTaskObserver() throw();


    protected:

        /**
         *Purpose:
         *<p> The extension point of the run.
         * Adds the successors of the specified task to the starter.
         */
        virtual bool runExtensionPoint(const std::string& taskId);


    private:

        /**
        * The associated task factory.
        */
        SynchronisableTaskFactory* m_taskFactory;


}; // End class SynchronisableTaskObserver 





/**
*Purpose:
*<p>    This class implement the CompletionResumer thread
*       to call the starter doComplete method
*
*Features:
*<p> Thread safe, exception safe
*
*
*/
class CompletionResumer : public OsSupport::Thread
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param p_sequencer   ptr to sequencer  
        *
        */ 
        CompletionResumer (Sequencer* p_sequencer);



        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~CompletionResumer()
                throw();
                

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
        CompletionResumer (const CompletionResumer& rhs)
                throw();


        //
        // Assignement operator
        //
        CompletionResumer& operator=(const CompletionResumer& rhs)
                 throw();


        /**
        * Sequencer object pointer
        */
        Sequencer* m_sequencer;
        

}; // End class CompletionResumer 


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_TASKSEQUENCER_HPP

