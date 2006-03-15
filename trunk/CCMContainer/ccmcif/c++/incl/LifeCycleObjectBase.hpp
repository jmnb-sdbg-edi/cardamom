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


#ifndef INCL_CCM_CIF_LIFECYCLE_OBJECT_BASE_HPP 
#define INCL_CCM_CIF_LIFECYCLE_OBJECT_BASE_HPP 

#include <Foundation/orbsupport/CORBA.hpp>

#include <LifeCycle/idllib/CosLifeCycle.skel.hpp>
#include <CCMContainer/idllib/Components.stub.hpp>
#include <CCMContainer/ccmcif/Context.hpp>


namespace Cdmw {

namespace CCM {

namespace CIF {



/**
*Purpose:
*<p>    A base implementation to a CosLifeCycle::LifeCycleObject. It is
* abstract as there are still two methods to be supplied by derived classes.
* It provides default implementations to IDL operations copy, move, and remove.
*
*Features:
*<p> [Thread safety, exception safety, performance]
*    [if no pertinent write none ]
*
*@see  [if necessary] 
*@link [if necessary] 
*
*/
class LifeCycleObjectBase :
        public virtual POA_CosLifeCycle::LifeCycleObject
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param deactivator A reference to an Object Deactivator to be used
        * whenever there is a need to deactivate a CORBA object incarnated by
        * the current servant (self).
        *
        *@exception CORBA::SystemException Any CORBA system exception.
        */ 
        LifeCycleObjectBase(Context* ctx)
          throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~LifeCycleObjectBase()
                throw();

        
        //
        // IDL:omg.org/CosLifeCycle/LifeCycleObject/copy:1.0
        //
        /**
        * Purpose:
        * <p>This makes a copy of the object. The copy is located in the scope of the
        * factory finder passed as the first parameter. It returns returns an object
        * reference to the new object initialised from the existing object.
        * Current implementation throws a CORBA::NO_IMPLEMENT system exception.
        *
        *@param there A factory finder where the copy will be located
        *@param the_criteria Allows for a number of optional parameters to be passed
        * (to the factory used in creating the new object)
        *
        *@return Object reference of the copy of the object
        *
        *@exception CosLifeCycle::NoFactory The target cannot find an appropriate factory 
        * to create a copy
        *@exception CosLifeCycle::NotCopyable The implementation refuses to copy itself
        *@exception CosLifeCycle::InvalidCriteria The target does not understand the criteria
        *@exception CosLifeCycle::CannotMeetCriteria The target understands the criteria
        * but cannot satisfy the criteria
        *@exception CORBA::SystemException Any CORBA system exception
        */ 
        virtual CosLifeCycle::LifeCycleObject_ptr 
        copy(CosLifeCycle::FactoryFinder_ptr there,
             const CosLifeCycle::Criteria& the_criteria)
            throw(CosLifeCycle::NoFactory,
                  CosLifeCycle::NotCopyable,
                  CosLifeCycle::InvalidCriteria,
                  CosLifeCycle::CannotMeetCriteria,
                  CORBA::SystemException);

        //
        // IDL:omg.org/CosLifeCycle/LifeCycleObject/move:1.0
        //
        /**
        * Purpose:
        * <p>This moves the object to the scope of the factory finder passed as the
        * first parameter. The object reference for the target object remains valid
        * after move has successfully executed.
        * Current implementation throws a CORBA::NO_IMPLEMENT system exception.
        *
        *@param there A factory finder where the object will be moved
        *@param the_criteria Allows for a number of optional parameters to be passed
        * (to the factory used in migrating the new object)
        *
        *@return The object reference of the new object
        *
        *@exception CosLifeCycle::NoFactory The target cannot find an appropriate factory
        * to support migration of the object
        *@exception CosLifeCycle::NotMovable The implementation refuses to move itself
        *@exception CosLifeCycle::InvalidCriteria The target does not understand the criteria
        *@exception CosLifeCycle::CannotMeetCriteria The target understands the criteria
        * but cannot satisfy the criteria
        *@exception CORBA::SystemException Any CORBA system exception
        */ 
        virtual void move(CosLifeCycle::FactoryFinder_ptr there,
                          const CosLifeCycle::Criteria& the_criteria)
            throw(CosLifeCycle::NoFactory,
                  CosLifeCycle::NotMovable,
                  CosLifeCycle::InvalidCriteria,
                  CosLifeCycle::CannotMeetCriteria,
                  CORBA::SystemException);

        //
        // IDL:omg.org/CosLifeCycle/LifeCycleObject/remove:1.0
        //
        /**
        * Purpose:
        * <p>Remove instructs the object to cease to exist. The object reference for the 
        * target is no longer valid after remove successfully completes.
        *
        * Current implementation deactivates the current object the context
        * and registers it as removed.
        * <B>NOTA:</B> Any further call to <I>Remove</I> for this object will raise a 
        * CORBA::OBJECT_NOT_EXIST system exception.
        *
        *@exception CosLifeCycle::NotRemovable An implementation that refuses to remove
        * itself should raise this exception
        *@exception CORBA::SystemException Any CORBA system exception
        */ 
        virtual void remove()
            throw(CosLifeCycle::NotRemovable,
                  CORBA::SystemException);


    private:        
        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        LifeCycleObjectBase()
                throw(CORBA::SystemException);

        /**
        *[Attribute description]
        */ 
    private:
        // Hide copy constructor/assignment operator
        LifeCycleObjectBase(const LifeCycleObjectBase& rhs)
                throw();
        
        LifeCycleObjectBase&
        operator=(const LifeCycleObjectBase& rhs)
                throw();


        /**
        * CCM Context
        */ 
        Context*  m_context;

        bool   m_is_removed;  // true if remove operation already called

        
}; // End class LifeCycleObjectBase 

} // End namespace CIF

} // End namespace CCM

} // End namespce Cdmw

#endif // INCL_CCM_CIF_LIFECYCLE_OBJECT_BASE_HPP 

