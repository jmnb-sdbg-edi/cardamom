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


#ifndef INCL_PLATFORMMNGT_ITERATOR_HPP
#define INCL_PLATFORMMNGT_ITERATOR_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"

#include <list>

namespace Cdmw
{
namespace PlatformMngt
{


class IteratorManager;



/**
*Purpose:
*<p>
* The base class of all iterators.
*/
class BaseIterator
{

    friend class IteratorManager;

    protected:

    /**
    * The ObjectId of this Iterator.
    */
    PortableServer::ObjectId_var m_oid;

    /**
    * The POA in charge of this Iterator.
    */
    PortableServer::POA_var m_POA;

};



/**
*Purpose:
*<p>
* The allocator for CORBA::String typedef iterator element.
*<p>
*/
template <class ELEMENT, class ELEMENT_out> class StringElementAllocator
{
    public:

    /**
    * Purpose:
    * <p>
    * Creates an empty CORBA string.
    */ 
    static void alloc(ELEMENT_out elt)
    {
        elt = CORBA::string_dup("");
    }

};


/**
*Purpose:
*<p>
* The allocator for CORBA struct like iterator element.
*<p>
*/
template <class ELEMENT, class ELEMENT_out> class StructElementAllocator
{
    public:

    /**
    * Purpose:
    * <p>
    * Creates the struct.
    */ 
    static void alloc(ELEMENT_out elt)
    {
        elt = new ELEMENT();
    }

};


/**
*Purpose:
*<p>
* The allocator for CORBA interface like iterator element.
*<p>
*/
template <class ELEMENT, class ELEMENT_out> class InterfaceElementAllocator
{
    public:

    /**
    * Purpose:
    * <p>
    * Creates a nil reference.
    */ 
    static void alloc(ELEMENT_out elt)
    {
        elt = ELEMENT::_nil();
    }

};



/**
*Purpose:
*<p>
* Facility template class for implementing a CosProperty Iterator like
* IDL interface. Used as a delegate.
*
* @param ELEMENT the type of the element
* @param ELEMENT_var the var type of the element
* @param ELEMENT_out the out type of the element
* @param ELEMENT_SEQ the type of the sequence of ELEMENT
* @param ELEMENT_SEQ_var the var type of the sequence of ELEMENT
* @param ELEMENT_SEQ_out the out type of the sequence of ELEMENT
* @param ELEMENT_alloc one of the following allocator class corresponding
* to the ELEMENT kind :
* - StringElementAllocator
* - StructElementAllocator
* - InterfaceElementAllocator
*<p>
*Features:
*<p>
*Thread safe.
*/
template <class ELEMENT,
    class ELEMENT_var,
    class ELEMENT_out,
    class ELEMENT_SEQ,
    class ELEMENT_SEQ_var,
    class ELEMENT_SEQ_out,
    class ELEMENT_alloc > class Iterator : public BaseIterator
{


public:


    /**
    * Purpose:
    * <p>
    * Constructor.
    */
    Iterator(const std::list<ELEMENT_var>& elementList)
        : m_elementList(elementList)
    {
        m_current = m_elementList.begin();
    }
    

    /**
    * Purpose:
    * <p>
    */
    void reset()
        throw(CORBA::SystemException)
    {
        CDMW_MUTEX_GUARD(m_mutex);
        m_current = m_elementList.begin();
    }


    /**
    * Purpose:
    * <p>
    */
    CORBA::Boolean next_one(ELEMENT_out elt)
        throw(CORBA::SystemException)
    {
        CORBA::Boolean ret = false;
        
        CDMW_MUTEX_GUARD(m_mutex);

        try
        {
            ELEMENT_var temp_elt;

            if (m_current != m_elementList.end())
            {
                temp_elt = *m_current;
                ++m_current;
                ret = true;

            }
            else
            {
                // just for not returning an invalid pointer
                ELEMENT_alloc::alloc(temp_elt);
            }

            // transfers ownership
            elt = temp_elt._retn();
            return ret;

        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
        }
    }









    CORBA::Boolean next_n(CORBA::ULong how_many, ELEMENT_SEQ_out elt_list)
        throw(CORBA::SystemException)
    {

        if (how_many == 0)
        {
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAM, CORBA::COMPLETED_NO);
        }

        try
        {
 


            CORBA::Boolean ret = false;
            
            ELEMENT_SEQ_var temp_elt_list = new ELEMENT_SEQ();
            temp_elt_list->length(0);

            CDMW_MUTEX_GUARD(m_mutex);

//gcc specification
#if defined __GNUC__
            size_t nbRemainingElts = std::distance(m_current, m_elementList.end());
#elif defined __IBMCPP__
            size_t nbRemainingElts = std::distance(m_current, m_elementList.end());
//Sun Forte specification
#else
			int len =0;
            std::distance(m_current, m_elementList.end(), len);
            size_t nbRemainingElts = len;
#endif

            if (nbRemainingElts > 0)
            {

                CORBA::ULong len = 0;

                if (nbRemainingElts > how_many)
                {
                    len = how_many;
                }
                else
                {
                    len = nbRemainingElts;
                }

                // sets the actual length of the returned list
                temp_elt_list->length(len);

                for (CORBA::ULong i = 0; i < len; ++i)
                {
                    temp_elt_list[i] = (*m_current).in();
                    ++m_current;
                }

                ret = true;
            }

            // transfers ownership
            elt_list = temp_elt_list._retn();
            return ret;

        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
        }

    }


    /**
    * Purpose:
    * <p>
    */
    void destroy()
        throw(CORBA::SystemException)
    {
        CDMW_ASSERT(!CORBA::is_nil(m_POA.in()));
        m_POA->deactivate_object(m_oid.in());
    }


    /**
    * The destructor.
    */
    virtual ~Iterator() {}


private:

    /**
    * The std iterator to the current element.
    */
    typename std::list<ELEMENT_var>::iterator m_current;


    /**
    * The mutex used to protect concurrent thread safe access to
    * m_current.
    */
    Cdmw::OsSupport::Mutex m_mutex;
    
    /**
    * The list of elements of this Iterator.
    */
    std::list<ELEMENT_var> m_elementList;


}; //End of Iterator



/**
*Purpose:
*<p>
* The IteratorManager.
*/
class IteratorManager
{


public:


    /**
    * Purpose:
    * <p>
    * Initializes the M_POA static member.
    * 
    * @param poa the POA in charge of iterators.
    * @exception BadParameterException if poa is nil.
    */
    static void initialize(PortableServer::POA_ptr poa)
        throw (BadParameterException);


    /**
    * Purpose:
    * <p>
    * Creates the CORBA iterator corresponding to the specified servant and
    * the actual iterator.
    * 
    * @param servant the servant embedding the actual iterator.
    * @param iterator the actual iterator.
    * @return the created CORBA iterator.
    * @exception BadParameterException if servant is NULL or iterator is NULL.
    */
    static CORBA::Object_ptr createIterator(PortableServer::ServantBase* servant,
        BaseIterator* iterator )
        throw (BadParameterException, CORBA::SystemException);


private:

    /**
    * The POA in charge of Iterators.
    * Must have SYSTEM_ID and RETAIN policies thus
    * the rootPOA may be used.
    */
    static PortableServer::POA_ptr M_POA;

};


} // End of namespace PlatformMngt
} // End of namespace Cdmw

#endif

