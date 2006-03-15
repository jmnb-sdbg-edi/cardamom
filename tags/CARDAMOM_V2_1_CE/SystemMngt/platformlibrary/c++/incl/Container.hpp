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


#ifndef INCL_PLATFORMMNGT_CONTAINER_HPP
#define INCL_PLATFORMMNGT_CONTAINER_HPP

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
* AlreadyExistsException.
*/
class AlreadyExistsException : public Cdmw::Exception
{
  public:
  
    AlreadyExistsException() : Exception (Exception::SAFE, "Parameter invalid")
    {
    }

  protected:

  private:

};



/**
*Purpose:
*<p> Container Template
*Features:
*<p> Thread safe.
*
*/

template <class ELEMENT_DATA> class Container
{

  public:

    /**
    *Purpose: 
    * <p> This structure defines Element composed of name and ELEMENT_DATA 
    *
    */
    struct Element
    {
       /**
       * Element name
       */
       std::string m_name;

       /**
       * Element data
       */
       ELEMENT_DATA m_data;

       /**
       * Purpose:
       * <p> Constructor
       * 
       */ 
       Element()
       {
       }


       /**
       * Purpose:
       * <p> Constructor
       *
       *@param entityName  name of entity
       *@param entityData  data of entity
       * 
       */ 
       Element(const std::string& elementName,
               const ELEMENT_DATA& elementData)
            : m_name(elementName),
              m_data(elementData)
       {
       }           
           

       /**
       * Purpose:
       * <p> Copy constructor.
       *
       */ 
       Element(const Element& rhs)
       {
           m_name = rhs.m_name;
           m_data = rhs.m_data;
       }

       /**
       * Purpose:
       * <p> Destructor
       * 
       */ 
       virtual ~Element()
       {
       }

       /**
       * Purpose:
       * <p> Assignment operator
       * 
       */ 
       Element& operator=(const Element& rhs)
       {
           m_name = rhs.m_name;
           m_data = rhs.m_data;

           return *this;
       }
    };
 
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
    typedef std::list<Element> ElementList;


    /**
    * Purpose:
    * <p> Default constructor.
    */ 
    Container()
    {
    }


    /**
    * Purpose:
    * <p> Copy constructor.
    */ 
    Container(const Container& rhs)
    {
        m_id = rhs.m_id;
        m_elements = rhs.m_elements;
    }


    /**
    * Purpose:
    * <p> Destructor.
    */ 
    virtual ~Container()
    {
    }


    /**
    * Purpose:
    * <p> Assignment operator.
    */ 
    Container& operator=(const Container& rhs)
    {   
        Container temp(rhs);
        swap(temp);

        return *this;
    }


    /**
    * Purpose:
    * <p> atomic swap.
    */ 
    void swap(Container& rhs)
    {
        m_id.swap(rhs.m_id);
        m_elements.swap(rhs.m_elements);
    }


    /**
    * Purpose:
    * <p> Returns the full identifier of this container.
    */ 
    const std::string& id(void) 
    { 
        return m_id; 
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
    *@param elementData element data to be returned
    *
    *@return True if element has been found.
    *
    *@exception Cdmw::OutOfResourcesException
    *
    */ 
    bool findElement(const std::string& name, ELEMENT_DATA& elementData)
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
          
                elementData = it->second;
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
    *@param elementData element data
    *
    *@return True if the element has been added.
    *
    *@exception Cdmw::OutOfResourcesException
    *
    */ 
    bool addElement(const std::string& name, const ELEMENT_DATA& elementData)
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
              m_elements[name] = elementData;
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
    *@param elementData element data
    *
    *@return true if the update is successful or false if name doesn't
    * denote an existing element.
    *
    *@exception Cdmw::OutOfResourcesException
    */ 
    bool updateElement(const std::string& name, const ELEMENT_DATA& elementData)
         throw (Cdmw::OutOfResourcesException)
    {
        CDMW_WRITER_LOCK_GUARD(m_rwLock);

        try
        {
            bool result = false;
            typename Elements::iterator it = m_elements.find(name);

            if (it != m_elements.end())
            {
                m_elements[name] = elementData;
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
    * <p> Removes all elements.
    * 
    */ 
    void removeAllElements()
    {
        CDMW_WRITER_LOCK_GUARD(m_rwLock);

        m_elements.clear();
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
                Element element (it->first, it->second);
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
            Element element (it->first, it->second);
            
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

    /**
    * Purpose:
    * <p> Destroy the Container instance
    * 
    *@return true if the destruction has been correctly performed
    */ 
    static bool destroy(Container* p_container)
    {
        bool result = false;
    
        if (p_container != NULL)
        {
            CDMW_MUTEX_GUARD(M_existingContainers_mutex);
            M_existingContainers.erase(p_container->m_id);
	
            delete p_container;
            result = true;
        }
	
	    return result;
    }




    /**
    * Purpose:
    * <p> Destroy the Container instance
    * 
    *@return true if the destruction has been correctly performed
    */ 
    static bool destroy(const std::string& id)
    {
        bool result = false;
        
        CDMW_MUTEX_GUARD(M_existingContainers_mutex);

        try
        {
          Container* p_container = NULL;

          typename Containers::iterator it = M_existingContainers.find(id);

          if (it != M_existingContainers.end())
          {
              // get the container pointer
              p_container = it->second;
              
              // erase container pointer in map
              M_existingContainers.erase(id);
              
              // release container memory
              delete p_container;
              
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
    * <p> Convert system container name to ident
    * 
    *@return system container ident
    *
    */ 
    static std::string systemContainerNameToId ()
    {
        // set system container ident (only one system on the platform)
        std::string ident = "S/";
        return ident;
    }
 
 
    /**
    * Purpose:
    * <p> Convert application container name to ident
    * 
    *@param  applicationName name of application
    *
    *@return application container ident
    *
    */ 
    static std::string applicationContainerNameToId (const std::string& applicationName)
    {
        // set application container ident
        std::string ident = "A/";
        ident += applicationName;
      
        return ident;
    }
                   
                   
 
    /**
    * Purpose:
    * <p> Convert process container name to ident
    * 
    *@param applicationName name of application
    *@param processName     name of process
    *@param hostName        name of host
    *
    *@return process container ident
    *
    */ 
    static std::string processContainerNameToId (const std::string& applicationName,
                                                 const std::string& processName,
                                                 const std::string& hostName)
    {
        // set process container ident
        std::string ident = "P/";
        ident += applicationName;
        ident += "/";
        ident += processName;
        ident += "/";
        ident += hostName;
      
        return ident;
    }
    
    
    /**
    * Purpose:
    * <p> Creates a new Container instance for system
    * 
    *@return The created instance
    *        Note : the container keeps the ownership of created instance
    *               so the caller cannot get it (using auto ptr)
    *               the created instance must be deleted by calling destroy method
    *
    *@exception AlreadyExistsException if already exists
    *@exception Cdmw::OutOfResourcesException
    */ 
    static Container* createSystemContainer()
            throw (AlreadyExistsException, 
                   OutOfResourcesException)
    {
        // create container
        Container* p_container = create(systemContainerNameToId());
      
        return p_container;
    }
 
 
    /**
    * Purpose:
    * <p> Creates a new Container instance for the application
    * 
    *@param applicationName name of application
    *
    *@return The created instance
    *        Note : the container keeps the ownership of created instance
    *               so the caller cannot get it (using auto ptr)
    *               the created instance must be deleted by calling destroy method
    *
    *@exception AlreadyExistsException if id already exists
    *@exception Cdmw::OutOfResourcesException
    */ 
    static Container* createApplicationContainer(const std::string& applicationName)
            throw (AlreadyExistsException, 
                   OutOfResourcesException)
    {
        // create container
        Container* p_container = create (applicationContainerNameToId(applicationName));
      
        return p_container;
    }
                   
                   
 
    /**
    * Purpose:
    * <p> Creates a new Container instance for the process
    * 
    *@param applicationName name of application
    *@param processName     name of process
    *@param hostName        name of host
    *
    *@return The created instance
    *        Note : the container keeps the ownership of created instance
    *               so the caller cannot get it (using auto ptr)
    *               the created instance must be deleted by calling destroy method
    *
    *@exception AlreadyExistsException if id already exists
    *@exception Cdmw::OutOfResourcesException
    */ 
    static Container* createProcessContainer(const std::string& applicationName,
                                             const std::string& processName,
                                             const std::string& hostName)
            throw (AlreadyExistsException, 
                   OutOfResourcesException)
    {
        // create container
        Container* p_container = create (processContainerNameToId(applicationName,
                                                                  processName,
                                                                  hostName));
      
        return p_container;
    }


    /**
    * Purpose:
    * <p> Indicates whether a Container having the specified id
    *     exists.
    *
    *@exception Cdmw::OutOfResourcesException
    */ 
    static bool existsContainerId(const std::string& id)
            throw (OutOfResourcesException)
    {
        CDMW_MUTEX_GUARD(M_existingContainers_mutex);

        return existsId(id);
    }


    /**
    * Purpose:
    * <p> Returns the Container instance corresponding
    *     to the specified ident
    *
    *@return the Container instance corresponding 
    *        to the specified id or NULL if not found.
    *
    *@exception Cdmw::OutOfResourcesException
    */ 
    static Container* findContainerById(const std::string& id)
            throw (OutOfResourcesException)
    {
        CDMW_MUTEX_GUARD(M_existingContainers_mutex);

        try
        {
          Container* result = NULL;

          typename Containers::iterator it = M_existingContainers.find(id);

          if (it != M_existingContainers.end())
          {
              result = it->second;
          }

          return result;
        }
        catch(const std::bad_alloc &)
        {
            CDMW_THROW(OutOfResourcesException);
        }
    }


protected:

    /**
    * Purpose:
    * <p> Constructs an Container with the supplied id.
    * 
    */ 
    Container(const std::string& id)
      : m_id(id)
    {

    }


private:


    /**
    *Purpose:
    *<p> The type used to manage the existing Container instances.
    */
    typedef std::map<std::string, Container*> Containers;
    
    
    /**
    *Purpose:
    *<p> The type used to manage the ELEMENTs.
    */
    typedef std::map<std::string, ELEMENT_DATA> Elements;



    

    /**
    * Purpose:
    * <p> Indicates whether a Container having the specified id
    *     exists.
    *
    * Unlike exists, this method is not thread safe and must be used with
    * a lock on M_existingContainers_mutex.
    *
    *@param id The identifier of Container
    *
    *@exception Cdmw::OutOfResourcesException
    */ 
    static bool existsId(const std::string& id)
        throw (OutOfResourcesException)
    {
      // NOT THREAD SAFE

        try
        {
            bool result = false;

            typename Containers::iterator it = M_existingContainers.find(id);

            if (it != M_existingContainers.end())
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
    * <p> Creates a new Container instance
    *     with the supplied identifier.
    * 
    * This method is not thread safe and must be used with
    * a lock on M_existingContainers_mutex.
    *
    *@param id The identifier of Container
    *
    *@return The created instance
    *
    *@exception AlreadyExistsException if id already exists
    *@exception Cdmw::OutOfResourcesException
    */ 
    static Container* create(const std::string& id)
        throw (AlreadyExistsException,
               OutOfResourcesException)
    {
      // NOT THREAD SAFE
                
        try
        {
            Container* result = NULL;
            
            // check if container already exist
            if (existsId (id) == true)
            {
              CDMW_THROW(AlreadyExistsException);
            }

            std::auto_ptr<Container> pNewContainer (new Container(id));
    
            // memorize in the existing containers set
            M_existingContainers[id] = pNewContainer.get();
        
            // transfers ownership
            result = pNewContainer.release();

            return result;

        }
        catch(const std::bad_alloc &)
        { 
            CDMW_THROW(OutOfResourcesException);
        }
    }
    
    
    
    
    
    /**
    * The identifier of this Container
    */
    std::string m_id;

    /**
    * The reader/writer lock used to manage concurrent thread safe access to
    * Elements.
    */
    Cdmw::OsSupport::ReaderWriterLock m_rwLock;
    
    /**
    * The elements contained in this container
    */
    Elements m_elements;


    /**
    * The mutex used to manage concurrent thread safe access to
    * M_existingContainers.
    */
    static Cdmw::OsSupport::Mutex M_existingContainers_mutex;

    /**
    * The existing Containers.
    */
    static Containers M_existingContainers;


    

};



} // End of namespace PlatformMngt
} // End of namespace Cdmw

#endif   // INCL_PLATFORMMNGT_CONTAINER_HPP

