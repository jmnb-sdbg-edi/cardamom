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


#ifndef INCL_COMMNSVCS_NAMING_NAMINGINTERFACE_HPP
#define INCL_COMMNSVCS_NAMING_NAMINGINTERFACE_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CosNaming.stub.hpp"
#include "Foundation/common/Exception.hpp"

#include "Foundation/commonsvcs/naming/InvalidNameException.hpp"


#include <string>
#include <list>

namespace Cdmw
{
namespace CommonSvcs
{
    
    namespace Naming
    {

        /**
        *Purpose:
        *<p> This class provides an easier access to a CosNaming::NamingContext object 
        * similar to a file system.
        * It allows the use of strings to name objects instead of CosNaming::Name  as defined in 
        * the Interoperable Naming Service.
        * It also enhances some standard access operations to a CORBA name service such as 
        * providing recursive destruction of a naming tree or forcing creation of missing 
        * nodes.
        *Features:
        *<p> Assignement operator is not thread safe as it is meaningless to assign
        * it another naming context while using it at the same time for bind ... etc.
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */


        class NamingInterface
        {

            public:

                /**
                * Purpose:
                * <p> Constructor
                * 
                *@param nc An object reference of a naming context.
                *
                *@exception CORBA::BAD_PARAM Using a nil context will raise a CORBA::BAD_PARAM
                * system exception.
                *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
                * may throw CORBA system exceptions.
                */
                NamingInterface(CosNaming::NamingContext_ptr nc)
                throw(CORBA::SystemException);

                /**
                * Purpose:
                * <p>  Copy constructor
                * 
                *@param rhs NamingInterface object to be copied.
                *
                *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
                * may throw CORBA system exceptions.
                */
                NamingInterface(const NamingInterface & rhs)
                throw(CORBA::SystemException);


                /**
                * Purpose:
                * <p> Destructor. It releases internal reference of the naming context.
                * 
                */
                virtual
                ~NamingInterface()
                throw();


                /**
                * Purpose:
                * <p> Assignement operator. It is exception safe <B>BUT NOT</B> thread safe
                * as it is meaningless to assign it another naming context while using it
                * at the same time for bind ... etc.
                * 
                *@param rhs A NamingInterface object to assign to current object.
                *
                *@return A reference to the current object.
                *
                *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
                * may throw CORBA system exceptions.
                */
                NamingInterface&
                operator=(const NamingInterface& rhs)
                throw(CORBA::SystemException);

                /**
                * Purpose:
                * <p> Returns a duplicate of the internal naming context. Caller is 
                * responsible for releasing it when done (better store it in a 
                * CosNaming::NamingContext_var)
                *
                *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
                * may throw CORBA system exceptions.
                */
                inline CosNaming::NamingContext_ptr name_context() const
                throw (CORBA::SystemException)
                {
                    return CosNaming::NamingContext::_duplicate(m_context.in());
                }

                //
                // Operations from CosNaming::NamingContext.
                //
                // NOTA:
                //        Cdmw::CommonSvcs::Naming::InvalidName
                //        vs
                //        CosNaming::NamingContext::InvalidName
                //
                // Exception Cdmw::CommonSvcs::Naming::InvalidName will not be raised
                // by operations that may raise CosNaming::NamingContext::InvalidName
                // exception to simplify catching of exceptions. As the additional
                // information provided by Cdmw::CommonSvcs::Naming::InvalidName is only
                // useful for debug purposes (its a string). However a user may call
                // to_name() with the string name to check that the exception is raised
                // locally by the name parser - in which case a
                // Cdmw::CommonSvcs::Naming::InvalidName will be raised again.
                //

                /**
                * Purpose:
                * <p> Creates a binding of a name and an object (an <I>nobject</I> binding)
                * in the naming context.
                *
                *@param s Name of the object within the naming context.
                *@param o Object reference to be bound to previous parameter s
                *@param force Forces creation of missing nodes if set to <I>true</I> in 
                * which case, the operation is equivalent to <B>rebind</B>. Default value 
                * is <I>false</I>.
                *
                *@exception CosNaming::NamingContext::NotFound Indicates the name does 
                * not identify a binding. The <I>why</I> member explains the reason for 
                * the exception - <I>missing_node</I> will not be raised if force is set 
                * to true.
                *@exception CosNaming::NamingContext::CannotProceed Indicates that the 
                * implementation has given up for some reason. The caller, however, may be
                * able to continue the operation at the returned naming context (<I>ctx</I> member).
                * The <I>rest_of_name</I> member contains the remainder of the non-working name.
                *@exception CosNaming::NamingContext::InvalidName Indicates the name is invalid.
                * (The string does not represent a valid CosNaming::Name. Implementation of
                * the internal naming context may place other restrictions on the name. An empty
                * string for the name is invalid)
                *@exception CosNaming::NamingContext::AlreadyBound Indicates an object is 
                * already bound to the specified name in the internal naming context.
                *@exception CORBA::SystemException Any CORBA system exception.
                */
                void
                bind(const std::string& s, CORBA::Object_ptr o, bool force = false)
                throw(CosNaming::NamingContext::NotFound,
                      CosNaming::NamingContext::CannotProceed,
                      CosNaming::NamingContext::InvalidName,
                      CosNaming::NamingContext::AlreadyBound,
                      CORBA::SystemException);

                /**
                * Purpose:
                * <p>Creates a binding of a name and an object (an <I>nobject</I> binding) 
                * in the naming context even if the name is already bound in the context. 
                * If already bound, the previous binding must be of type <I>nobject</I>.
                * 
                *@param s Name of the object within the naming context.
                *@param o Object reference to be rebound to previous parameter s.
                *@param force Forces creation of missing nodes if set to <I>true</I>. 
                * Default value is <I>false</I>.
                *
                *@exception CosNaming::NamingContext::NotFound Indicates the name does 
                * not identify a binding. The <I>why</I> member explains the reason for 
                * the exception - <I>missing_node</I> will not be raised if force is set 
                * to true. NotFound exception with a <I>why</I> reason of <I>not_object</I>
                * is also raised if already bound and the previous binding is not of type
                * <I>nobject</I>.
                *@exception CosNaming::NamingContext::CannotProceed Indicates that the 
                * implementation has given up for some reason. The caller, however, may be
                * able to continue the operation at the returned naming context (<I>ctx</I> member).
                * The <I>rest_of_name</I> member contains the remainder of the non-working name.
                *@exception CosNaming::NamingContext::InvalidName Indicates the name is invalid.
                * (The string does not represent a valid CosNaming::Name. Implementation of
                * the internal naming context may place other restrictions on the name. An empty
                * string for the name is invalid)
                *@exception CORBA::SystemException Any CORBA system exception.
                */
                void
                rebind(const std::string& s, CORBA::Object_ptr o, bool force = false)
                throw(CosNaming::NamingContext::NotFound,
                      CosNaming::NamingContext::CannotProceed,
                      CosNaming::NamingContext::InvalidName,
                      CORBA::SystemException);

                /**
                * Purpose:
                * <p> Creates a binding of a name and a naming context (an <I>ncontext</I> 
                * binding) in the naming context.
                *
                *@param s  Name of the naming context within the naming context.
                *@param nc Naming context to be bound to previous parameter s.
                *@param force Forces creation of missing nodes if set to <I>true</I> in 
                * which case, the operation is equivalent to <B>rebind_context</B>. 
                * Default value is <I>false</I>.
                *
                *@exception CosNaming::NamingContext::NotFound Indicates the name does 
                * not identify a binding. The <I>why</I> member explains the reason for 
                * the exception - <I>missing_node</I> will not be raised if force is set 
                * to true.
                *@exception CosNaming::NamingContext::CannotProceed Indicates that the 
                * implementation has given up for some reason. The caller, however, may be
                * able to continue the operation at the returned naming context (<I>ctx</I> member).
                * The <I>rest_of_name</I> member contains the remainder of the non-working name.
                *@exception CosNaming::NamingContext::InvalidName Indicates the name is invalid.
                * (The string does not represent a valid CosNaming::Name. Implementation of
                * the internal naming context may place other restrictions on the name. An empty
                * string for the name is invalid)
                *@exception CosNaming::NamingContext::AlreadyBound Indicates an object is 
                * already bound to the specified name in the internal naming context.
                *@exception CORBA::SystemException Any CORBA system exception (attempts 
                * to bind a nil context raise a CORBA::BAD_PARAM system exception).
                */
                void
                bind_context(const std::string& s, CosNaming::NamingContext_ptr nc, bool force = false)
                throw(CosNaming::NamingContext::NotFound,
                      CosNaming::NamingContext::CannotProceed,
                      CosNaming::NamingContext::InvalidName,
                      CosNaming::NamingContext::AlreadyBound,
                      CORBA::SystemException);

                /**
                * Purpose:
                * <p> Creates a binding of a name and a naming context (an <I>ncontext</I> 
                * binding) in the naming context.
                *
                *@param s  Name of the naming context within the naming context.
                *@param ni NamingInterface object wrapping a naming context be bound to
                * previous parameter s.
                *@param force Forces creation of missing nodes if set to <I>true</I> in 
                * which case, the operation is equivalent to <B>rebind_context</B>. 
                * Default value is <I>false</I>.
                *
                *@exception CosNaming::NamingContext::NotFound Indicates the name does 
                * not identify a binding. The <I>why</I> member explains the reason for 
                * the exception - <I>missing_node</I> will not be raised if force is set 
                * to true.
                *@exception CosNaming::NamingContext::CannotProceed Indicates that the 
                * implementation has given up for some reason. The caller, however, may be
                * able to continue the operation at the returned naming context (<I>ctx</I> member).
                * The <I>rest_of_name</I> member contains the remainder of the non-working name.
                *@exception CosNaming::NamingContext::InvalidName Indicates the name is invalid.
                * (The string does not represent a valid CosNaming::Name. Implementation of
                * the internal naming context may place other restrictions on the name. An empty
                * string for the name is invalid)
                *@exception CosNaming::NamingContext::AlreadyBound Indicates an object is 
                * already bound to the specified name in the internal naming context.
                *@exception CORBA::SystemException Any CORBA system exception (attempts 
                * to bind a nil context raise a CORBA::BAD_PARAM system exception).
                */
                void
                bind_context(const std::string& s, const NamingInterface& ni, bool force = false)
                throw(CosNaming::NamingContext::NotFound,
                      CosNaming::NamingContext::CannotProceed,
                      CosNaming::NamingContext::InvalidName,
                      CosNaming::NamingContext::AlreadyBound,
                      CORBA::SystemException);

                /**
                * Purpose:
                * <p>Creates a binding of a name and a naming context (an <I>ncontext</I> 
                * binding) in the naming context even if the name is already bound in the context. 
                * If already bound, the previous binding must be of type <I>ncontext</I>.
                * Use <I>rebind_context</I> with care as this may cause an orphaned naming 
                * context if it is already bound.
                * 
                *@param s Name of the object within the naming context.
                *@param nc Naming context to be rebound to previous parameter s.
                *@param force Forces creation of missing nodes if set to <I>true</I>. 
                * Default value is <I>false</I>.
                *
                *@exception CosNaming::NamingContext::NotFound Indicates the name does 
                * not identify a binding. The <I>why</I> member explains the reason for 
                * the exception - <I>missing_node</I> will not be raised if force is set 
                * to true. NotFound exception with a <I>why</I> reason of <I>not_context</I>
                * is also raised if already bound and the previous binding is not of type
                * <I>ncontext</I>.
                *@exception CosNaming::NamingContext::CannotProceed Indicates that the 
                * implementation has given up for some reason. The caller, however, may be
                * able to continue the operation at the returned naming context (<I>ctx</I> member).
                * The <I>rest_of_name</I> member contains the remainder of the non-working name.
                *@exception CosNaming::NamingContext::InvalidName Indicates the name is invalid.
                * (The string does not represent a valid CosNaming::Name. Implementation of
                * the internal naming context may place other restrictions on the name. An empty
                * string for the name is invalid)
                *@exception CORBA::SystemException Any CORBA system exception.
                */
                void
                rebind_context(const std::string& s, CosNaming::NamingContext_ptr nc, bool force = false)
                throw(CosNaming::NamingContext::NotFound,
                      CosNaming::NamingContext::CannotProceed,
                      CosNaming::NamingContext::InvalidName,
                      CORBA::SystemException);
                /**
                * Purpose:
                * <p>Creates a binding of a name and a naming context (an <I>ncontext</I> 
                * binding) in the naming context even if the name is already bound in the context. 
                * If already bound, the previous binding must be of type <I>ncontext</I>.
                * Use <I>rebind_context</I> with care as this may cause an orphaned naming 
                * context if it is already bound.
                * 
                *@param s Name of the object within the naming context.
                *@param ni NamingInterface object wrapping a naming context to be rebound
                * to previous parameter s.
                *@param force Forces creation of missing nodes if set to <I>true</I>. 
                * Default value is <I>false</I>.
                *
                *@exception CosNaming::NamingContext::NotFound Indicates the name does 
                * not identify a binding. The <I>why</I> member explains the reason for 
                * the exception - <I>missing_node</I> will not be raised if force is set 
                * to true. NotFound exception with a <I>why</I> reason of <I>not_context</I>
                * is also raised if already bound and the previous binding is not of type
                * <I>ncontext</I>.
                *@exception CosNaming::NamingContext::CannotProceed Indicates that the 
                * implementation has given up for some reason. The caller, however, may be
                * able to continue the operation at the returned naming context (<I>ctx</I> member).
                * The <I>rest_of_name</I> member contains the remainder of the non-working name.
                *@exception CosNaming::NamingContext::InvalidName Indicates the name is invalid.
                * (The string does not represent a valid CosNaming::Name. Implementation of
                * the internal naming context may place other restrictions on the name. An empty
                * string for the name is invalid)
                *@exception CORBA::SystemException Any CORBA system exception.
                */
                void
                rebind_context(const std::string& s, const NamingInterface& ni, bool force = false)
                throw(CosNaming::NamingContext::NotFound,
                      CosNaming::NamingContext::CannotProceed,
                      CosNaming::NamingContext::InvalidName,
                      CORBA::SystemException);

                /**
                * Purpose:
                * <p> This retrieves the object bound to a name in a given context. The
                * name must exactly match the bound name.
                * 
                *@param s Name of the binding. The name can have multiple components; 
                * therefore, name resolution can traverse multiple contexts.
                * 
                *@return The object bound to s. The type of the object is not returned, so
                * the caller is responsible for narrowing the object to the appropriate type.
                *
                *@exception CosNaming::NamingContext::NotFound Indicates the name does 
                * not identify a binding. The <I>why</I> member explains the reason for 
                * the exception - <I>missing_node</I> or <I>not_context</I>.
                *@exception CosNaming::NamingContext::CannotProceed Indicates that the 
                * implementation has given up for some reason. The caller, however, may be
                * able to continue the operation at the returned naming context (<I>ctx</I> member).
                * The <I>rest_of_name</I> member contains the remainder of the non-working name.
                *@exception CosNaming::NamingContext::InvalidName Indicates the name is invalid.
                * (The string does not represent a valid CosNaming::Name. Implementation of
                * the internal naming context may place other restrictions on the name. An empty
                * string for the name is invalid)
                *@exception CORBA::SystemException Any CORBA system exception.
                */
                CORBA::Object_ptr
                resolve(const std::string& s)
                throw(CosNaming::NamingContext::NotFound,
                      CosNaming::NamingContext::CannotProceed,
                      CosNaming::NamingContext::InvalidName,
                      CORBA::SystemException);

                /**
                * Purpose:
                * <p>This removes a name binding from a context.
                * 
                *@param s Name of the binding. The name can have multiple components; 
                * therefore, name resolution can traverse multiple contexts.
                *     
                *@exception CosNaming::NamingContext::NotFound Indicates the name does 
                * not identify a binding. The <I>why</I> member explains the reason for 
                * the exception - <I>missing_node</I> or <I>not_context</I>.
                *@exception CosNaming::NamingContext::CannotProceed Indicates that the 
                * implementation has given up for some reason. The caller, however, may be
                * able to continue the operation at the returned naming context (<I>ctx</I> member).
                * The <I>rest_of_name</I> member contains the remainder of the non-working name.
                *@exception CosNaming::NamingContext::InvalidName Indicates the name is invalid.
                * (The string does not represent a valid CosNaming::Name. Implementation of
                * the internal naming context may place other restrictions on the name. An empty
                * string for the name is invalid)
                *@exception CORBA::SystemException Any CORBA system exception.
                */
                void
                unbind(const std::string& s)
                throw(CosNaming::NamingContext::NotFound,
                      CosNaming::NamingContext::CannotProceed,
                      CosNaming::NamingContext::InvalidName,
                      CORBA::SystemException);

                /**
                * Purpose:
                * <p>This operation returns a new NamingInterface object holding a new 
                * naming context. The new context is not bound to any name.
                * 
                *@return A new NamingInterface object.
                *
                *@exception CORBA::SystemException Any CORBA system exception. If an 
                * implementation cannot create the context (it may have placed limits on 
                * the number of naming contexts), it may raise CORBA::IMP_LIMIT system 
                * exception.
                */
                NamingInterface
                new_context()
                throw(CORBA::SystemException);

                /**
                * Purpose:
                * <p> This creates a new context, binds it to the name supplied as an
                * argument, and wraps it in a NamingInterface object. The newly-created
                * context is implemented by the same naming server as the context in
                * which it was bound.
                * 
                *@param s Name of the binding to be created.
                *@param force Forces creation of missing nodes if set to <I>true</I>. 
                * Default value is <I>false</I>.
                *
                *@return A NamingInterface object wrapping the newly created naming 
                * context.
                *
                *@exception CosNaming::NamingContext::NotFound Indicates the name does 
                * not identify a binding. The <I>why</I> member explains the reason for 
                * the exception - <I>missing_node</I> will not be raised if force is set 
                * to true.
                *@exception CosNaming::NamingContext::CannotProceed Indicates that the 
                * implementation has given up for some reason. The caller, however, may be
                * able to continue the operation at the returned naming context (<I>ctx</I> member).
                * The <I>rest_of_name</I> member contains the remainder of the non-working name.
                *@exception CosNaming::NamingContext::InvalidName Indicates the name is invalid.
                * (The string does not represent a valid CosNaming::Name. Implementation of
                * the internal naming context may place other restrictions on the name. An empty
                * string for the name is invalid)
                *@exception CosNaming::NamingContext::AlreadyBound Indicates an object is 
                * already bound to the specified name in the parent context.
                *@exception CORBA::SystemException Any CORBA system exception.
                */
                NamingInterface
                bind_new_context(const std::string& s, bool force = false)
                throw(CosNaming::NamingContext::NotFound,
                      CosNaming::NamingContext::AlreadyBound,
                      CosNaming::NamingContext::CannotProceed,
                      CosNaming::NamingContext::InvalidName,
                      CORBA::SystemException);

                /**
                * Purpose:
                * <p>This deletes internal naming context. If there are bindings denoting
                * the destroyed context, these bindings are not removed. This destroys
                * internal naming context and not the current NamingInterface object!
                * 
                *@exception CosNaming::NamingContext::NotEmpty The naming context contains
                * bindings. It must be empty to be destroyed.
                *@exception CORBA::SystemException Any CORBA system exception
                */
                void
                destroy()
                throw(CosNaming::NamingContext::NotEmpty,
                      CORBA::SystemException);
                /**
                * Purpose:
                * <p>This deletes internal naming context and, recursively, its bindings,
                * and child contexts. 
                * If there are bindings denoting the destroyed context, these bindings are 
                * not removed. The context is destroyed only if all its internal bindings are
                * removed and internal contexts destroyed, otherwise only undestroyed contexts
                * will remain.
                * This destroys internal naming context and not the current NamingInterface
                * object!
                * 
                *@return <B>true</B> if done completely with no errors, <B>false</B> otherwise.
                *@exception CORBA::SystemException Any CORBA system exception.
                */
                bool
                destroy_all()
                throw(CORBA::SystemException);

                /**
                * Purpose:
                * <p>This operation allows a caller to iterate through a set of bindings in a 
                * naming context. It returns at most the requested number of bindings in
                * <I>BindingList bl</I>. If the naming context contains additional bindings, 
                * the <I>list</I> operation returns a BindingIterator with the additional 
                * bindings. If, however, the naming context does not contain additional 
                * bindings, the binding iterator is a nil object reference.
                * 
                *@param how_many Determines the maximum number of bindings to return in
                * the parameter <I>bl</I>.
                *@param bl A sequence where each element is a Binding containing a Name
                * of length 1 representing a single NameComponent. It contains at most
                * <I>how_many</I> elements.
                *@param bi A reference to an iterator object to access any remaining 
                * bindings not returned by <I>BindingIterator bi</I>. A nil reference 
                * indicates that the <I>bl</I> parameter contains all of the bindings in
                * the context. <B>A naming service implementation is free to destroy an
                * iterator object at any time without warning. Be prepared to handle 
                * CORBA::OBJECT_NOT_EXIST system exception from calls to an iterator!</B>
                *
                *@exception CORBA::SystemException Any CORBA system exception.
                */
                void
                list(CORBA::ULong how_many,
                     CosNaming::BindingList_out bl,
                     CosNaming::BindingIterator_out bi)
                throw(CORBA::SystemException);

                typedef std::list<CosNaming::Binding> BindingList;

                /**
                * Default chunk size for CosNaming::NamingContext::list operation.
                */
                static const CORBA::ULong LIST_CHUNK_SIZE;

                /**
                * Purpose:
                * <p>This operation returns the list of all bindings within a naming 
                * context in an STL container (list).
                * 
                *@param blist A user provided STL container to hold all the bindings.
                *@param how_many Determines the maximum number of bindings to return in
                * the parameter <I>bl</I> of a standard <I>CosNaming::NamingContext::list</I>
                * operation. If <I>how_many</I> is set to zero, the caller is requesting to 
                * use only a BindingIterator to access the bindings of a naming context. 
                * However, the <I>BindingIterator::next_n()</I> operation forbids the use of
                * a zero value of <I>how_many</I> (it raises a CORBA::BAD_PARAM system 
                * exception). So in this case, <I>list_context</I> will use 
                * <B>LIST_CHUNK_SIZE</B> as a default value.
                *
                *@return <B>true</B> if list is done without errors, <B>false</B> if 
                * interrupted.
                */
                bool
                list_context(BindingList& blist,
                             CORBA::ULong how_many = LIST_CHUNK_SIZE)
                throw();

                //
                // Additional local operations from CosNaming::NamingContextExt
                // interface.
                //
                /**
                * Purpose:
                * <p> This operation accepts a CosNaming::Name and returns a stringified
                * name.
                * 
                *@param name A CosNaming::Name to stringify.
                *
                *@return A stringified name, INS compatible, of the input CosNaming::Name.
                *
                *@exception InvalidNameException Indicates that the name is invalid (length 0)
                */
                static std::string to_string(const CosNaming::Name& name)
                throw(InvalidNameException);

                /**
                * Purpose:
                * <p>This operation accepts a stringified name and returns a CosNaming::Name.
                * 
                *@param s A stringified name, INS compatible, to be converted to a CosNaming::Name.
                *
                *@return A pointer to a CosNaming::Name that holds the decoded string.
                *
                *@exception InvalidNameException Indicates that the input string is syntactically
                * malformed.
                */
                static CosNaming::Name* to_name(const std::string& s)
                throw(InvalidNameException);

                /**
                * Purpose:
                * <p>This operation accepts a stringified name and deletes any prefix ending 
                * in / from the name. (It returns the last <I>NameComponent</I> of the name
                * stringified).
                * 
                *@param s A stringified name, INS compatible, to be parsed by the operation.
                *
                *@return A stringified name, INS compatible, holding the result of the operation.
                *
                *@exception InvalidNameException Indicates that the input string is syntactically
                * malformed.
                */
                static std::string basename(const std::string& s)
                throw(InvalidNameException);

                /**
                * Purpose:
                * <p>This operation accepts a stringified name and returns all but the last 
                * <I>NameComponent</I> of the name.
                * 
                *@param s A stringified name, INS compatible, to be parsed by the operation.
                *
                *@return A stringified name, INS compatible, holding the result of the operation.
                *
                *@exception InvalidNameException Indicates that the input string is syntactically
                * malformed.
                */
                static std::string dirname(const std::string& s)
                throw(InvalidNameException);

                /**
                * Purpose:
                * <p>This operation add escape character for a stringified name before any reserved char such '/' or '.'
                * 
                *@param s A string
                *
                *@return A stringified name, INS compatible, holding the result of the operation.
                *
                */
	        static std::string NamingInterface::addEscape(const std::string& s);

                /**
                *[Attribute description]
                */


            protected:

                /**
                * Purpose:
                * <p> Default Constructor is protected to avoid
                * needless checking of validity of internal naming context before
                * every single non-static function call
                * 
                *@exception CORBA::SystemException Doesn't throw but added for harmonisation
                * with other constructors
                */
                NamingInterface() throw(CORBA::SystemException);


            private:
                /**
                * Purpose:
                * <p>Private function that actually does the job.
                * It creates a new context and binds it to the name supplied as an
                * argument. The newly-created context is implemented by the same naming
                * server as the context in which it was bound.
                * 
                *@param name a CosNaming::Name for the binding to be created.
                *@param force Forces creation of missing nodes if set to <I>true</I>. 
                * Default value is <I>false</I>.
                *
                *@return A pointer (object reference) to the newly created naming context.
                *
                *@exception CosNaming::NamingContext::NotFound Indicates the name does 
                * not identify a binding. The <I>why</I> member explains the reason for 
                * the exception - <I>missing_node</I> will not be raised if force is set 
                * to true.
                *@exception CosNaming::NamingContext::CannotProceed Indicates that the 
                * implementation has given up for some reason. The caller, however, may be
                * able to continue the operation at the returned naming context (<I>ctx</I> member).
                * The <I>rest_of_name</I> member contains the remainder of the non-working name.
                *@exception CosNaming::NamingContext::InvalidName Indicates the name is invalid.
                * (Implementation of the internal naming context may place other restrictions
                * on the name. An empty string for the name is invalid)
                *@exception CosNaming::NamingContext::AlreadyBound Indicates an object is 
                * already bound to the specified name in the parent context.
                *@exception CORBA::SystemException Any CORBA system exception.
                *@see bind_new_context
                */
                CosNaming::NamingContext_ptr
                NamingInterface::bind_new_context(const CosNaming::Name & name, bool force)
                throw(CosNaming::NamingContext::NotFound,
                      CosNaming::NamingContext::AlreadyBound,
                      CosNaming::NamingContext::CannotProceed,
                      CosNaming::NamingContext::InvalidName,
                      CORBA::SystemException);

                /**
                * Purpose:
                * <p>Clears all binding of the input naming context by calling 
                * <I>clear_chunk</I> that recursively clears child contexts.
                * The naming input context is not destroyed.
                * 
                *@param nc The naming context to be cleared.
                *
                *@return <B>true</B> if done completely with no errors, <B>false</B> otherwise.
                */ 
                // Depth first
                bool
                clear_context(CosNaming::NamingContext_ptr nc)
                throw();

                /**
                * Purpose:
                * <p>Removes all bindings in bl from the naming context by performing 
                * a depth-first search if a binding is a naming context, in which case, 
                * the naming context is first "cleared" (<I>clear_context</I>), destroyed,
                * and then the name that was used for the binding is unbound in the parent
                * context.
                * 
                *@param nc A naming context.
                *@param bl A list of bindings of <I>nc</I> to be "cleared".
                *
                *@return <B>true</B> if done completely with no errors, <B>false</B> otherwise.
                */
                bool
                clear_chunk(CosNaming::NamingContext_ptr nc,
                            const CosNaming::BindingList & bl)
                throw();

                /**
                * Purpose:
                * <p>Appends elements of bl to blist.
                * 
                *@param
                *@return
                *@exception
                */
                void
                append_chunk(const CosNaming::BindingList & bl,
                             BindingList& blist);



                /**
                * internal target naming context 
                */
                CosNaming::NamingContext_var m_context;

        }; // End class NamingInterface

    } // End namespace Naming
    
} // End namespace CommonSvcs
} // End namespace Cdmw

#endif // INCL_COMMNSVCS_NAMING_NAMINGINTERFACE_HPP






