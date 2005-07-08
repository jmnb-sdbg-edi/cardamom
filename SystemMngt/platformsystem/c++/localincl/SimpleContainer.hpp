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


#ifndef INCL_PLATFORMMNGT_SIMPLE_CONTAINER_HPP
#define INCL_PLATFORMMNGT_SIMPLE_CONTAINER_HPP

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include "Foundation/common/Exception.hpp"

#include <memory>
#include <string>
#include <list>
#include <map>



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
*Purpose:
*<p> SimpleContainer Template
*Features:
*<p> Thread safe.
*
*/

template <class ELEMENT> class SimpleContainer
{

  public:

 
    /**
    *Purpose:
    *<p> The types used for retrieving the list of element names of a given
    *    container in a thread safe manner.
    */
    typedef std::list<std::string> ElementNameList;
    
    /**
    *Purpose:
    *<p> The types used for retrieving the list of elements of a given
    *    container in a thread safe manner.
    */
    typedef std::list<ELEMENT> ElementList;


    /**
    * Purpose:
    * <p> Default constructor.
    */ 
    SimpleContainer()
    {
    }


    /**
    * Purpose:
    * <p> Copy constructor.
    */ 
    SimpleContainer(const SimpleContainer& rhs)
    {
        m_elements = rhs.m_elements;
    }


    /**
    * Purpose:
    * <p> Destructor.
    */ 
    virtual ~SimpleContainer()
    {
    }


    /**
    * Purpose:
    * <p> Assignment operator.
    */ 
    SimpleContainer& operator=(const SimpleContainer& rhs)
    {   
        SimpleContainer temp(rhs);
        swap(temp);

        return *this;
    }


    /**
    * Purpose:
    * <p> atomic swap.
    */ 
    void swap(SimpleContainer& rhs)
    {
        m_elements.swap(rhs.m_elements);
    }




    /**
    * Purpose:
    * <p> Indicates whether this container is empty or not.
    */ 
    bool isEmpty(void)
    {
        bool result = false;

        CDMW_READER_LOCK_GUARD(m_rwLock);

        if (m_elements.size() == 0)
        {
            result = true;
        }

        return result;
    }
    
    
    
    
    /**
    * Purpose:
    * <p> Returns the number of elements in the container
    */ 
    size_t getNumberOfElements(void)
    {
        CDMW_READER_LOCK_GUARD(m_rwLock);

        return m_elements.size();
    }
    
    
    
    
    /**
    * Purpose:
    * <p> Indicates whether an element exists with the specified name.
    * 
    *@param  name name specifying the element
    *
    *@return true if the element exists
    *
    *@exception Cdmw::OutOfResourcesException
    */ 
    bool existsElement(const std::string& name)
         throw (Cdmw::OutOfResourcesException)
    {
        CDMW_READER_LOCK_GUARD(m_rwLock);

        try
        {
            bool result = false;

            typename Elements::iterator it = m_elements.find(name);

            if (it != m_elements.end())
            {
                result = true;
            }

            return result;
        }
        catch(const std::bad_alloc &)
        {
            CDMW_THROW(OutOfResourcesException);
        }
    }


    
        
     
    /**
    * Purpose:
    * <p> Find an existing element
    * 
    *@param name        name specifying the element
    *@param element     element data to be returned
    *
    *@return True if element has been found.
    *
    *@exception Cdmw::OutOfResourcesException
    *
    */ 
    bool findElement(const std::string& name, ELEMENT& element)
         throw (Cdmw::OutOfResourcesException)
    {
        CDMW_READER_LOCK_GUARD(m_rwLock);

        try
        {
            bool result = false;
            typename Elements::iterator it = m_elements.find(name);

            if (it != m_elements.end())
            {
                // the element is found, return its data
                result = true;
          
                element = it->second;
            }
        
            return result;
        }
        catch(const std::bad_alloc &)
        {
            CDMW_THROW(OutOfResourcesException);
        }
    }   
    
    
    /**
    * Purpose:
    * <p> Adds a new element
    * 
    *@param name        name specifying the element
    *@param element     element data
    *
    *@return True if the element has been added.
    *
    *@exception Cdmw::OutOfResourcesException
    *
    */ 
    bool addElement(const std::string& name, const ELEMENT& element)
         throw (Cdmw::OutOfResourcesException)
    {
        CDMW_WRITER_LOCK_GUARD(m_rwLock);

        try
        {
            bool result = false;
 
            typename Elements::iterator it = m_elements.find(name);

            if (it == m_elements.end())
            {
              // add the element
              m_elements[name] = element;
              result = true;
            }

            return result;
        }
        catch(const std::bad_alloc &)
        {
            CDMW_THROW(OutOfResourcesException);
        }
    }


    /**
    * Purpose:
    * <p> Updates an existing element.
    * 
    *@param name        name specifying the element
    *@param element     element data
    *
    *@return true if the update is successful or false if name doesn't
    * denote an existing element.
    *
    *@exception Cdmw::OutOfResourcesException
    */ 
    bool updateElement(const std::string& name, const ELEMENT& element)
         throw (Cdmw::OutOfResourcesException)
    {
        CDMW_WRITER_LOCK_GUARD(m_rwLock);

        try
        {
            bool result = false;
            typename Elements::iterator it = m_elements.find(name);

            if (it != m_elements.end())
            {
                m_elements[name] = element;
                result = true;
            }

            return result;
        }
        catch(const std::bad_alloc &)
        {
            CDMW_THROW(OutOfResourcesException);
        }
    }


    /**
    * Purpose:
    * <p> Removes an existing element.
    * 
    *@param name The name specifying the binding
    *
    *@return     True if the removal has been performed.
    *            False if the name doesn't exist.
    */ 
    bool removeElement(const std::string& name)
    {
        bool result = false;

        CDMW_WRITER_LOCK_GUARD(m_rwLock);

        int count = m_elements.erase(name);

        if (count != 0)
        {
            result = true;
        }

        return result;
    }


    /**
    * Purpose:
    * <p> Returns the list of elements of this container.
    *
    *@param list of elements
    *
    *@exception Cdmw::OutOfResourcesException
    *
    */ 
    void list(ElementList& elementList)
        throw (Cdmw::OutOfResourcesException)
    {
        CDMW_READER_LOCK_GUARD(m_rwLock);

        try
        {
            typename Elements::iterator it;

            for (it = m_elements.begin(); it != m_elements.end(); it++)
            {
                ELEMENT element (it->second);
                elementList.push_back(element);
            }
        }
        catch(const std::bad_alloc &)
        {
            CDMW_THROW(OutOfResourcesException);
        }
    }
    
    
    /**
    * Purpose:
    * <p> Returns the list of elements of this container limited to the input number
    *     of elements. The rest of list of elements of the container is returned also
    *
    *@param how_many          max number of requested elements
    *@param elementList       list of elements returned, limited to max request number
    *@param restOfElementList rest of list of elements is returned
    *
    *@exception Cdmw::OutOfResourcesException
    *
    */
    void list(unsigned long how_many, 
              ElementList& elementList, ElementList& restOfElementList)
         throw (OutOfMemoryException)
    {
    	CDMW_READER_LOCK_GUARD(m_rwLock);
    	
        size_t len = m_elements.size();
                
        // set length of requested sequence and rest of sequence
        size_t requested_len = len;
        size_t rest_len = 0;
                
        if (len > how_many)
        {
            requested_len = how_many;
            rest_len = len - how_many;
        }
                

        typename Elements::iterator it;
        unsigned long i_len = 0;

        for (it = m_elements.begin(); it != m_elements.end(); it++)
        {
        	// first fill the element list until the max number of elements reached
        	// then fill the rest of list
            ELEMENT element (it->second);
            
            if (i_len < requested_len)
            {
                elementList.push_back(element);
            }
            else
            {
            	restOfElementList.push_back(element);
            }
            
            i_len++;
        }
    }







    /**
    * Purpose:
    * <p> Returns the list of element names of this container.
    *
    *@param list of element names
    *
    *@exception Cdmw::OutOfResourcesException
    *
    */ 
    void listName(ElementNameList& elementNameList)
        throw (Cdmw::OutOfResourcesException)
    {
        CDMW_READER_LOCK_GUARD(m_rwLock);

        try
        {
            typename Elements::iterator it;

            for (it = m_elements.begin(); it != m_elements.end(); it++)
            {
                elementNameList.push_back(it->first);
            }
        }
        catch(const std::bad_alloc &)
        {
            CDMW_THROW(OutOfResourcesException);
        }
    }
    
    
    
    /**
    * Purpose:
    * <p> Returns the list of element names of this container limited to the input number
    *     of elements. The rest of list of element names of the container is returned also
    *
    *@param how_many              max number of requested element names
    *@param elementNameList       list of element names returned, limited to max request number
    *@param restOfElementNameList rest of list of element names is returned
    *
    *@exception Cdmw::OutOfResourcesException
    *
    */
    void listName(unsigned long how_many, 
                  ElementNameList& elementNameList, 
                  ElementNameList& restOfElementNameList)
         throw (OutOfMemoryException)
    {
    	CDMW_READER_LOCK_GUARD(m_rwLock);
    	
        size_t len = m_elements.size();
                
        // set length of requested sequence and rest of sequence
        size_t requested_len = len;
        size_t rest_len = 0;
                
        if (len > how_many)
        {
            requested_len = how_many;
            rest_len = len - how_many;
        }
                

        typename Elements::iterator it;
        unsigned long i_len = 0;

        for (it = m_elements.begin(); it != m_elements.end(); it++)
        {
        	// first fill the element list until the max number of elements reached
        	// then fill the rest of list
            if (i_len < requested_len)
            {
                elementNameList.push_back(it->first);
            }
            else
            {
            	restOfElementNameList.push_back(it->first);
            }
            
            i_len++;
        }
    }

    


protected:



private:


  
    /**
    *Purpose:
    *<p> The type used to manage the ELEMENTs.
    */
    typedef std::map<std::string, ELEMENT> Elements;



    


    /**
    * The reader/writer lock used to manage concurrent thread safe access to
    * Elements.
    */
    Cdmw::OsSupport::ReaderWriterLock m_rwLock;
    
    /**
    * The elements contained in this container
    */
    Elements m_elements;

};



} // End of namespace PlatformMngt
} // End of namespace Cdmw

#endif   // INCL_PLATFORMMNGT_SIMPLE_CONTAINER_HPP

