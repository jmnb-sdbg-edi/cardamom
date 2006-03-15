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


package cdmw.commonsvcs.naming;

import cdmw.orbsupport.ExceptionStrings;
import cdmw.orbsupport.ExceptionMinorCodes;

import org.omg.CORBA.CompletionStatus;
import org.omg.CORBA.SystemException;
import org.omg.CORBA.BAD_PARAM;

import org.omg.CosNaming.NamingContext;
import org.omg.CosNaming.NamingContextHelper;
import org.omg.CosNaming.NameComponent;

import org.omg.CosNaming.Binding;
import org.omg.CosNaming.BindingType;
import org.omg.CosNaming.BindingListHolder;
import org.omg.CosNaming.BindingIteratorHolder;

import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.CosNaming.NamingContextPackage.CannotProceed;
import org.omg.CosNaming.NamingContextPackage.InvalidName;
import org.omg.CosNaming.NamingContextPackage.AlreadyBound;
import org.omg.CosNaming.NamingContextPackage.NotEmpty;
import org.omg.CosNaming.NamingContextPackage.NotFoundReason;

/**
 * This class provides an easier access to a org.omg.CosNaming.NamingContext
 * object similar to a file system.
 * It allows the use of strings to name objects instead of Name as defined in
 * the Interoperable Naming Service.
 * It also enhances some standard access operations to a CORBA name service
 * such as providing recursive destruction of a naming tree
 * or forcing creation of missing nodes.
 *
 */
public class NamingInterface {

    /**
     * Default chunk size for list operation.
     */
    public static final int LIST_CHUNK_SIZE = 100;

    /**
     * internal target naming context
     */
    private NamingContext context = null;

    /**
     * Constructor
     *
     * @param nc An object reference of a naming context.
     *
     * @exception BAD_PARAM Using a null context will raise a BAD_PARAM
     * system exception.
     */
    public NamingInterface(NamingContext nc) throws SystemException {

        if (nc == null) {
            int minor = ExceptionMinorCodes.BAD_PARAMNilObjectReference;
            throw new BAD_PARAM(ExceptionStrings.BAD_PARAM_reasons[minor],
                minor, CompletionStatus.COMPLETED_NO);
        }
        context = nc;

    }


    /**
     * Returns the internal naming context.
     *
     * @return The internal naming context.
     */
    public NamingContext getNamingContext() {
        return this.context;
    }

    //
    // Operations from CosNaming::NamingContext.
    //
    // NOTA:
    //        cdmw.commonsvcs.naming.InvalidNameException
    //        vs
    //        org.omg.CosNaming.NamingContextPackage.InvalidName
    //
    // Exception cdmw.commonsvcs.naming.InvalidNameException will not be raised
    // by operations that may raise
    // org.omg.CosNaming.NamingContextPackage.InvalidName exception to simplify
    // catching of exceptions. As the additional information provided
    // by cdmw.commonsvcs.naming.InvalidNameException is only useful
    // for debug purposes (its a string). However a user may call toName()
    // with the string name to check that the exception is raised
    // locally by the name parser - in which case a
    // cdmw.commonsvcs.naming.InvalidNameException will be raised again.
    //

    /**
     * Creates a binding of a name and an object
     * (an <I>nobject</I> binding) in the naming context.
     *
     * @param s Name of the object within the naming context.
     * @param o Object reference to be bound to previous parameter s
     * @param force Forces creation of missing nodes if set to <I>true</I> in
     * which case, the operation is equivalent to
     * {@link #rebind(String,org.omg.CORBA.Object,boolean)}.
     * Default value is <I>false</I>.
     *
     * @exception NotFound Indicates the name does not identify a binding.
     * The <I>why</I> member explains the reason for the exception
     * - <I>missing_node</I> will not be raised if force is set to true.
     * @exception CannotProceed Indicates that the implementation
     * has given up for some reason. The caller, however, may be able
     * to continue the operation at the returned naming context
     * (<I>ctx</I> member). The <I>rest_of_name</I> member contains
     * the remainder of the non-working name.
     * @exception InvalidName Indicates the name is invalid.
     * (The string does not represent a valid NameComponent array.
     * Implementation of the internal naming context may place
     * other restrictions on the name. An empty string for the name is invalid)
     * @exception AlreadyBound Indicates an object is already bound
     * to the specified name in the internal naming context.
     */
    public void bind(String s, org.omg.CORBA.Object o, boolean force)
        throws     NotFound,
                CannotProceed,
                InvalidName,
                AlreadyBound {

        if (!force) {

            NameComponent[] name = null;
            try {
                name = NameParser.toName(s);
            } catch(InvalidNameException ine) {
                // TODO : exception information lost
                throw new InvalidName();
            }
            this.context.bind(name, o);

        } else {
            rebind(s,o,true);
        }

    }


    /**
     * Creates a binding of a name and an object (an <I>nobject</I> binding)
     * in the naming context even if the name is already bound in the context.
     * If already bound, the previous binding must be of type <I>nobject</I>.
     *
     * @param s Name of the object within the naming context.
     * @param o Object reference to be rebound to previous parameter s.
     * @param force Forces creation of missing nodes if set to <I>true</I>.
     * Default value is <I>false</I>.
     *
     * @exception NotFound Indicates the name does not identify a binding.
     * The <I>why</I> member explains the reason for the exception
     * - <I>missing_node</I> will not be raised if force is set to true.
     * NotFound exception with a <I>why</I> reason of <I>not_object</I>
     * is also raised if already bound and the previous binding is not of type
     * <I>nobject</I>.
     * @exception CannotProceed Indicates that the implementation has given up
     * for some reason. The caller, however, may be able to continue
     * the operation at the returned naming context (<I>ctx</I> member).
     * The <I>rest_of_name</I> member contains the remainder
     * of the non-working name.
     * @exception InvalidName Indicates the name is invalid.
     * (The string does not represent a valid NameComponent array.
     * Implementation ofthe internal naming context may place other
     * restrictions on the name. An empty string for the name is invalid)
     * @exception AlreadyBound Indicates an object is already bound
     * to the specified name in the internal naming context.
     */
    public void    rebind(String s, org.omg.CORBA.Object o, boolean force)
        throws     NotFound,
                CannotProceed,
                InvalidName,
                AlreadyBound {

        NameComponent[] name = null;
        try {
            name = NameParser.toName(s);
        } catch(InvalidNameException ine) {
            // TODO : exception information lost
            throw new InvalidName();
        }

        if (!force) {
            this.context.rebind( name, o );
        } else {

            try {
                context.rebind( name, o );
            } catch(NotFound nf) {
                if ( nf.why.equals(NotFoundReason.missing_node) ) {
                    // This may raise an exception if it fails

                    // Recreate whole path and redo binding
                    // Get basename (parent context)

                    // REM: lg > 1 otherwise rebind would have succeeded
                    int lg = name.length - 1;
                    NameComponent[] name_ = new NameComponent[lg];
                    for(int i = 0; i<lg ; i++) {
                        name_[i] = name[i];
                    }
                    NamingContext nc_ = bindNewContext(name_, force);
                    NameComponent[] n_ = new NameComponent[1];
                    n_[0] = name[lg]; // last element
                    nc_.rebind(n_, o);
                } else {
                    throw nf;
                }
            }
        }

    }

    /**
     * Creates a binding of a name and a naming context (an <I>ncontext</I>
     * binding) in the naming context.
     *
     * @param s  Name of the naming context within the naming context.
     * @param nc Naming context to be bound to previous parameter s.
     * @param force Forces creation of missing nodes if set to <I>true</I> in
     * which case, the operation is equivalent to
     * {@link #rebindContext(String,org.omg.CosNaming.NamingContext,boolean)}.
     * Default value is <I>false</I>.
     *
     * @exception NotFound Indicates the name does not identify a binding.
     * The <I>why</I> member explains the reason for the exception
     * - <I>missing_node</I> will not be raised if force is set to true.
     * @exception CannotProceed Indicates that the implementation has given up
     * for some reason. The caller, however, may be able to continue
     * the operation at the returned naming context (<I>ctx</I> member).
     * The <I>rest_of_name</I> member contains the remainder
     * of the non-working name.
     * @exception InvalidName Indicates the name is invalid.
     * (The string does not represent a valid NameComponent array.
     * Implementation ofthe internal naming context may place other
     * restrictions on the name. An empty string for the name is invalid)
     * @exception AlreadyBound Indicates an object is already bound
     * to the specified name in the internal naming context.
     */
    public void    bindContext(String s, NamingContext nc, boolean force)
        throws     NotFound,
                CannotProceed,
                InvalidName,
                AlreadyBound {

        if (!force) {
            NameComponent[] name = null;
            try {
                name = NameParser.toName(s);
            } catch (InvalidNameException ine) {
                throw new InvalidName();
            }
            context.bind_context(name, nc);
        } else {
            rebindContext(s,nc,true);
        }

    }

    /**
     * Creates a binding of a name and a naming context (an <I>ncontext</I>
     * binding) in the naming context.
     *
     * @param s  Name of the naming context within the naming context.
     * @param ni NamingInterface object wrapping a naming context be bound to
     * previous parameter s.
     * @param force Forces creation of missing nodes if set to <I>true</I> in
     * which case, the operation is equivalent to
     * {@link #rebindContext(String,org.omg.CosNaming.NamingContext,boolean)}.
     * Default value is <I>false</I>.
     *
     * @exception NotFound Indicates the name does not identify a binding.
     * The <I>why</I> member explains the reason for the exception
     * - <I>missing_node</I> will not be raised if force is set to true.
     * @exception CannotProceed Indicates that the implementation has given up
     * for some reason. The caller, however, may be able to continue
     * the operation at the returned naming context (<I>ctx</I> member).
     * The <I>rest_of_name</I> member contains the remainder
     * of the non-working name.
     * @exception InvalidName Indicates the name is invalid.
     * (The string does not represent a valid NameComponent array.
     * Implementation ofthe internal naming context may place other
     * restrictions on the name. An empty string for the name is invalid)
     * @exception AlreadyBound Indicates an object is already bound
     * to the specified name in the internal naming context.
     */
    public void    bindContext(String s, NamingInterface ni, boolean force)
        throws     NotFound,
                CannotProceed,
                InvalidName,
                AlreadyBound {

        bindContext(s, ni.getNamingContext(), force);

    }

    /**
     * Creates a binding of a name and a naming context (an <I>ncontext</I>
     * binding) in the naming context even if the name is already bound
     * in the context. If already bound, the previous binding must be
     * of type <I>ncontext</I>. Use
     * {@link #rebindContext(String,NamingContext,boolean)} with care
     * as this may cause an orphaned naming context if it is already bound.
     *
     * @param s Name of the object within the naming context.
     * @param nc Naming context to be rebound to previous parameter s.
     * @param force Forces creation of missing nodes if set to <I>true</I>.
     * Default value is <I>false</I>.
     *
     * @exception NotFound Indicates the name does not identify a binding.
     * The <I>why</I> member explains the reason for the exception
     * - <I>missing_node</I> will not be raised if force is set to true.
     * @exception CannotProceed Indicates that the implementation has given up
     * for some reason. The caller, however, may be able to continue
     * the operation at the returned naming context (<I>ctx</I> member).
     * The <I>rest_of_name</I> member contains the remainder
     * of the non-working name.
     * @exception InvalidName Indicates the name is invalid.
     * (The string does not represent a valid NameComponent array.
     * Implementation ofthe internal naming context may place other
     * restrictions on the name. An empty string for the name is invalid)
     * @exception SystemException Any CORBA system exception.
     * @exception AlreadyBound Indicates an object is already bound
     * to the specified name in the internal naming context.
     */
    public void    rebindContext(String s, NamingContext nc, boolean force)
        throws     NotFound,
                CannotProceed,
                InvalidName,
                AlreadyBound {

        NameComponent[] name = null;
        try {
            name = NameParser.toName(s);
        } catch(InvalidNameException ine) {
            throw new InvalidName();
        }

        if (!force) {
            context.rebind_context(name, nc);
        } else {
            try {
                context.rebind_context(name, nc);
            } catch(NotFound nf) {

                if ( nf.why.equals(NotFoundReason.missing_node) ) {
                    // This may raise an exception if it fails

                    // Recreate whole path and redo binding
                    // Get basename (parent context)

                    // REM: lg > 1 otherwise rebind would have succeeded
                    int lg = name.length - 1;
                    NameComponent[] name_ = new NameComponent[lg];
                    for(int i = 0; i<lg ; i++) {
                        name_[i] = name[i];
                    }
                    NamingContext nc_ = bindNewContext(name_, force);
                    NameComponent[] n_ = new NameComponent[1];
                    n_[0] = name[lg]; // last element
                    nc_.rebind_context(n_, nc);
                } else {
                    throw nf;
                }
            }
        }

    }

    /**
     * Creates a binding of a name and a naming context (an <I>ncontext</I>
     * binding) in the naming context even if the name is already bound
     * in the context. If already bound, the previous binding must be of type
     * <I>ncontext</I>. Use
     * {@link #rebindContext(String,NamingContext,boolean)} with care
     * as this may cause an orphaned naming context if it is already bound.
     *
     * @param s Name of the object within the naming context.
     * @param ni NamingInterface object wrapping a naming context to be rebound
     * to previous parameter s.
     * @param force Forces creation of missing nodes if set to <I>true</I>.
     * Default value is <I>false</I>.
     *
     * @exception NotFound Indicates the name does not identify a binding.
     * The <I>why</I> member explains the reason for the exception
     * - <I>missing_node</I> will not be raised if force is set to true.
     * NotFound exception with a <I>why</I> reason of <I>not_object</I>
     * is also raised if already bound and the previous binding is not of type
     * <I>nobject</I>.
     * @exception CannotProceed Indicates that the implementation has given up
     * for some reason. The caller, however, may be able to continue
     * the operation at the returned naming context (<I>ctx</I> member).
     * The <I>rest_of_name</I> member contains the remainder
     * of the non-working name.
     * @exception InvalidName Indicates the name is invalid.
     * (The string does not represent a valid NameComponent array.
     * Implementation ofthe internal naming context may place other
     * restrictions on the name. An empty string for the name is invalid)
     */
    public void    rebindContext(String s, NamingInterface ni, boolean force)
        throws     NotFound,
                CannotProceed,
                InvalidName,
                AlreadyBound {

        rebindContext(s, ni.getNamingContext(), force);

    }

    /**
     * This retrieves the object bound to a name in a given context.
     * The name must exactly match the bound name.
     *
     * @param s Name of the binding. The name can have multiple components;
     * therefore, name resolution can traverse multiple contexts.
     *
     * @return The object bound to s. The type of the object is not returned, so
     * the caller is responsible for narrowing the object
     * to the appropriate type.
     *
     * @exception NotFound Indicates the name does not identify a binding.
     * The <I>why</I> member explains the reason for the exception
     * - <I>missing_node</I> will not be raised if force is set to true.
     * NotFound exception with a <I>why</I> reason of <I>not_object</I>
     * is also raised if already bound and the previous binding is not of type
     * <I>nobject</I>.
     * @exception CannotProceed Indicates that the implementation has given up
     * for some reason. The caller, however, may be able to continue
     * the operation at the returned naming context (<I>ctx</I> member).
     * The <I>rest_of_name</I> member contains the remainder
     * of the non-working name.
     * @exception InvalidName Indicates the name is invalid.
     * (The string does not represent a valid NameComponent array.
     * Implementation ofthe internal naming context may place other
     * restrictions on the name. An empty string for the name is invalid)
     */
    public org.omg.CORBA.Object resolve(String s)
        throws  NotFound,
                CannotProceed,
                InvalidName {

        NameComponent[] name = null;
        try {
            name = NameParser.toName(s);
        } catch(InvalidNameException ine) {
            throw new InvalidName();
        }
        return context.resolve(name);

    }

    /**
     * This removes a name binding from a context.
     *
     * @param s Name of the binding. The name can have multiple components;
     * therefore, name resolution can traverse multiple contexts.
     *
     * @exception NotFound Indicates the name does not identify a binding.
     * The <I>why</I> member explains the reason for the exception
     * - <I>missing_node</I> will not be raised if force is set to true.
     * NotFound exception with a <I>why</I> reason of <I>not_object</I>
     * is also raised if already bound and the previous binding is not of type
     * <I>nobject</I>.
     * @exception CannotProceed Indicates that the implementation has given up
     * for some reason. The caller, however, may be able to continue
     * the operation at the returned naming context (<I>ctx</I> member).
     * The <I>rest_of_name</I> member contains the remainder
     * of the non-working name.
     * @exception InvalidName Indicates the name is invalid.
     * (The string does not represent a valid NameComponent array.
     * Implementation ofthe internal naming context may place other
     * restrictions on the name. An empty string for the name is invalid)
     */
    public void    unbind(String s)
        throws  NotFound,
                CannotProceed,
                InvalidName {

        NameComponent[] name = null;
        try {
            name = NameParser.toName(s);
        } catch (InvalidNameException ine) {
            throw new InvalidName();
        }
        context.unbind(name);

    }

    /**
     * This operation returns a new NamingInterface object holding a new
     * naming context. The new context is not bound to any name.
     *
     * @return A new NamingInterface object.
     *
     * @exception SystemException Any CORBA system exception. If an
     * implementation cannot create the context.
     */
    public NamingInterface newContext() throws SystemException {

        NamingContext nc = this.context.new_context();
        return new NamingInterface(nc);

    }

    /**
     * This creates a new context, binds it to the name supplied as an
     * argument, and wraps it in a NamingInterface object. The newly-created
     * context is implemented by the same naming server as the context in
     * which it was bound.
     *
     * @param s Name of the binding to be created.
     * @param force Forces creation of missing nodes if set to <I>true</I>.
     * Default value is <I>false</I>.
     *
     * @return A NamingInterface object wrapping the newly created naming
     * context.
     *
     * @exception NotFound Indicates the name does not identify a binding.
     * The <I>why</I> member explains the reason for the exception
     * - <I>missing_node</I> will not be raised if force is set to true.
     * @exception CannotProceed Indicates that the implementation has given up
     * for some reason. The caller, however, may be able to continue
     * the operation at the returned naming context (<I>ctx</I> member).
     * The <I>rest_of_name</I> member contains the remainder
     * of the non-working name.
     * @exception InvalidName Indicates the name is invalid.
     * (The string does not represent a valid NameComponent array.
     * Implementation ofthe internal naming context may place other
     * restrictions on the name. An empty string for the name is invalid)
     * @exception AlreadyBound Indicates an object is already bound
      * to the specified name in the internal naming context.
     * @exception SystemException Any CORBA system exception.
     */
    public NamingInterface bindNewContext(String s, boolean force)
        throws  NotFound,
                AlreadyBound,
                CannotProceed,
                InvalidName,
                SystemException {

        NameComponent[] name = null;
        try {
            name = NameParser.toName(s);
        } catch (InvalidNameException ine) {
            throw new InvalidName();
        }

        NamingContext nc = bindNewContext(name, force);
        return new NamingInterface(nc);

    }


    /**
     * This deletes internal naming context. If there are bindings denoting
     * the destroyed context, these bindings are not removed. This destroys
     * internal naming context and not the current NamingInterface object!
     *
     * @exception NotEmpty The naming context contains
     * bindings. It must be empty to be destroyed.
     */
    public void    destroy() throws NotEmpty {
        context.destroy();
    }

    /**
     * This deletes internal naming context and, recursively, its bindings,
     * and child contexts.
     * If there are bindings denoting the destroyed context, these bindings are
     * not removed. The context is destroyed only if all its internal bindings
     * are removed and internal contexts destroyed, otherwise only undestroyed
     * contexts will remain.
     * This destroys internal naming context and not the current
     * NamingInterface object!
     *
     * @return <I>true</I> if done completely with no errors,
     * <I>false</I> otherwise.
     * @exception SystemException Any CORBA system exception.
     */
    public boolean destroyAll() throws SystemException {

        boolean done = false;
        if ( clearContext(context) ) {
            try {
                context.destroy();
                done = true;
            } catch (NotEmpty ne) {
                // ignore
            }
        }
        return done;

    }

    /**
     * This operation allows a caller to iterate through a set of bindings
     * in a naming context. It returns at most the requested number of bindings
     * in {@link org.omg.CosNaming.BindingListHolder}. If the naming context
     * contains additional bindings, the <I>list</I> operation returns
     * a BindingIterator with the additional bindings. If, however,
     * the naming context does not contain additional bindings, the binding
     * iterator is a null object reference.
     *
     * @param how_many Determines the maximum number of bindings to return in
     * the parameter <I>bl</I>.
     * @param bl The returned list of bindings. It contains at most
     * <I>how_many</I> elements.
     * @param bi the returned binding iterator.
     */
    public void    list(int how_many,BindingListHolder bl,
        BindingIteratorHolder bi) {

        context.list(how_many,bl,bi);

    }

    /**
     * This operation returns the list of all bindings within a naming
     * context.
     *
     * @param blist List of the bindings.
     * @param how_many Determines the maximum number of bindings to return in
     * the parameter <I>bl</I> of a standard
     * {@link org.omg.CosNaming.BindingListHolder} operation.
     * If <I>how_many</I> is set to zero, the caller is requesting to
     * use only a BindingIterator to access the bindings of a naming context.
     * However, the
     * {@link org.omg.CosNaming.BindingIterator#next_n(int,BindingListHolder)}
     * operation forbids the use of a zero value of <I>how_many</I>
     * (it raises a BAD_PARAM system exception). So in this case,
     * <I>listContext</I> will use {@link #LIST_CHUNK_SIZE} as a default value.
     *
     * @return <I>true</I> if list is done without errors, <I>false</I> if
     * interrupted.
     */
    public boolean listContext(BindingListHolder blist, int how_many) {

        boolean done = true;
        // Iterator reference
        BindingIteratorHolder it = new BindingIteratorHolder();
        // Binding list
        BindingListHolder bl = new BindingListHolder();

        //try {
            int chunkSize = how_many;
            // Get first chunk
            context.list(chunkSize,bl,it);
            // Append first chunk
            appendChunk(bl,blist);

            if (how_many == 0) {
                chunkSize = LIST_CHUNK_SIZE;
            }

            // More bindings ?
            if (it.value!=null) {
                boolean more;
                do {
                    // Get next chunk
                    more = it.value.next_n(chunkSize,bl);
                    // Append chunk
                    appendChunk(bl,blist);

                } while (more);
                // Clean up iterator
                it.value.destroy();
            }
        //} catch (SystemException se) {
        //    done = false;
        //}
        return done;

    }

    /**
     * This operation returns the list of all bindings within a naming
     * context. This operation will use {@link #LIST_CHUNK_SIZE}
     * as value for the maximum number of bindings to return.
     *
     * @param blist List of the bindings.
     *
     * @return <I>true</I> if list is done without errors, <I>false</I> if
     * interrupted.
     */
    public boolean listContext(BindingListHolder blist) {

        return listContext(blist, LIST_CHUNK_SIZE);

    }

    //
    // Additional static operations.
    //

    /**
     * This operation accepts a NameComponent array and returns
     * a stringified name.
     *
     * @param name A NameComponent array to stringify.
     *
     * @return A stringified name, INS compatible, of the input
     * NameComponent array.
     *
     * @exception InvalidNameException Indicates that the name is invalid.
     */
    public static String toString(NameComponent[] name)
        throws InvalidNameException {

        return NameParser.toString(name);

    }

    /**
     * This operation accepts a stringified name and returns
     * a NameComponent array.
     *
     * @param s A stringified name, INS compatible, to be converted
     * to a NameComponent array.
     *
     * @return A pointer to a NameComponent array that holds the decoded string.
     *
     * @exception InvalidNameException Indicates that the input string is
     * syntactically malformed.
     */
    public static NameComponent[] toName(String s)
        throws InvalidNameException {

        return NameParser.toName(s);

    }

    /**
     * This operation accepts a stringified name and deletes
     * any prefix ending in / from the name.
     * (It returns the last {@link org.omg.CosNaming.NameComponent}
     * of the name stringified).
     *
     * @param s A stringified name, INS compatible, to be parsed
     * by the operation.
     *
     * @return A stringified name, INS compatible, holding the result
     * of the operation.
     *
     * @exception InvalidNameException Indicates that the input string is
     * syntactically malformed.
     */
    public static String basename(String s) throws InvalidNameException {

        NameComponent[] name = toName(s);
        NameComponent[] basename = { name[name.length-1] };
        return toString(basename);

    }

    /**
     * This operation accepts a stringified name and returns all
     * but the last {@link org.omg.CosNaming.NameComponent} of the name.
     *
     * @param s A stringified name, INS compatible, to be parsed
     * by the operation.
     *
     * @return A stringified name, INS compatible, holding the result
     * of the operation.
     *
     * @exception InvalidNameException Indicates that the input string is
     * syntactically malformed.
     */
    public static String dirname(String s) throws InvalidNameException {

        NameComponent[] name = toName(s);
        NameComponent[] dirname = new NameComponent[name.length-1];
        for(int i=0; i<dirname.length; i++) {
            dirname[i] = name[i];
        }
        return toString(dirname);

    }

    /**
     * Default Constructor is protected to avoid
     * needless checking of validity of internal naming context before
     * every single non-static function call
     *
     * @exception SystemException Doesn't throw but added for harmonisation
     * with other constructors
     */
    protected NamingInterface() throws SystemException {}

    /**
     * Private function that actually does the job.
     * It creates a new context and binds it to the name supplied as an
     * argument. The newly-created context is implemented by the same naming
     * server as the context in which it was bound.
     *
     * @param name a NameComponent array for the binding to be created.
     * @param force Forces creation of missing nodes if set to <I>true</I>.
     * Default value is <I>false</I>.
     *
     * @return The newly created naming context.
     *
     * @exception NotFound Indicates the name does not identify a binding.
     * The <I>why</I> member explains the reason for the exception
     * - <I>missing_node</I> will not be raised if force is set to true.
     * @exception CannotProceed Indicates that the implementation has given up
     * for some reason. The caller, however, may be able to continue
     * the operation at the returned naming context (<I>ctx</I> member).
     * The <I>rest_of_name</I> member contains the remainder
     * of the non-working name.
     * @exception InvalidName Indicates the name is invalid.
     * (The string does not represent a valid NameComponent array.
     * Implementation ofthe internal naming context may place other
     * restrictions on the name. An empty string for the name is invalid)
     * @exception AlreadyBound Indicates an object is already bound
     * to the specified name in the internal naming context.
     * @exception SystemException Any CORBA system exception.
     * @see bindNewContext
     */
    private NamingContext bindNewContext(NameComponent[] name, boolean force)
        throws     NotFound,
                CannotProceed,
                InvalidName,
                AlreadyBound,
                SystemException {

        NamingContext nc = context;

        try {
            nc = context.bind_new_context(name);
        } catch(NotFound nf) {
            if ( force && nf.why.equals(NotFoundReason.missing_node) ) {
                // This may raise an exception if it fails
                int rest = nf.rest_of_name.length;
                int lg = name.length - rest;

                    if (lg > 0) {
                        NameComponent[] base = new NameComponent[lg];
                        for (int i = 0; i<lg; i++) {
                            base[i] = name[i];
                        }
                        org.omg.CORBA.Object obj = context.resolve(base);
                        try {
                            nc = NamingContextHelper.narrow(obj);
                            // Some CosNaming Implementations may return
                            // a nil object reference if the naming context
                            // have been removed meanwhile...
                            if (nc==null) {
                                throw new CannotProceed();
                            }
                        } catch(SystemException se) {
                            throw new CannotProceed();
                        }
                    }

                //  create remaining bindings
                NameComponent[] n = new NameComponent[1];
                for (int j = 0; j<rest ; j++) {
                    n[0] = nf.rest_of_name[j];
                    nc = nc.bind_new_context(n);
                }
            } else {
                throw nf;
            }

        }

        return nc;

    }

    /**
     * Clears all binding of the input naming context by calling
     * {@link #clearChunk(NamingContext,BindingListHolder)} that recursively
     * clears child contexts.
     * The naming input context is not destroyed.
     *
     * @param nc The naming context to be cleared.
     *
     * @return <I>true</I> if done completely with no errors,
     * <I>false</I> otherwise.
     */
    // Depth first
    private boolean clearContext(NamingContext nc) {

        boolean allDone = false;
        // Iterator reference
        BindingIteratorHolder it = new BindingIteratorHolder();
        // Binding list
        BindingListHolder bl = new BindingListHolder();

        //try {
            // Get first chunk
            nc.list(LIST_CHUNK_SIZE,bl,it);
            // Clear first chunk
            boolean chunkDone = clearChunk(nc, bl);

            // More bindings?
            if (it.value!=null) {
                boolean more;
                do {
                    // Get next chunk
                    System.out.println("Get next chunk");
                    more = it.value.next_n(LIST_CHUNK_SIZE,bl);
                    // Clear chunk
                    System.out.println("Clear chunk");
                    chunkDone &= clearChunk(nc, bl);
                } while (more);
                // Clean up iterator
                it.value.destroy();
            }
            allDone = chunkDone;
        //} catch(SystemException se) {
            // ignore
        //}

        return allDone;

    }

    /**
     * Removes all bindings in bl from the naming context by performing
     * a depth-first search if a binding is a naming context, in which case,
     * the naming context is first "cleared"
     * ({@link #clearContext(NamingContext)}), destroyed, and then the name
     * that was used for the binding is unbound in the parent context.
     *
     * @param nc A naming context.
     * @param bl A list of bindings of <I>nc</I> to be "cleared".
     *
     * @return <I>true</I> if done completely with no errors,
     * <I>false</I> otherwise.
     */
    private boolean    clearChunk(NamingContext nc,BindingListHolder bl) {

        boolean done = true;

        for (int i = 0; i < bl.value.length; i++) {

            try {
                // for contexts, first destroy then unbind
                if ( bl.value[i].binding_type.equals(BindingType.ncontext) ) {

                    org.omg.CORBA.Object obj = nc.resolve(bl.value[i].binding_name);
                    NamingContext _nc = null;

                    try {
                        _nc = NamingContextHelper.narrow(obj);
                        _nc.destroy();
                        nc.unbind(bl.value[i].binding_name);
                        done &= true;
                    } catch(SystemException se) {
                        done = false;
                    } catch (NotEmpty ne) {
                        boolean contextDone = clearContext(_nc);
                        if (contextDone) {
                            _nc.destroy();
                            nc.unbind(bl.value[i].binding_name);
                            done &= true;
                        }
                    }

                // for objects
                } else {
                    nc.unbind(bl.value[i].binding_name);
                }

            // keep silent (ignore)
            } catch (NotFound nf) {
                done = false;
            } catch (NotEmpty ne) {
                done = false;
            } catch (CannotProceed cp) {
                done = false;
            } catch (InvalidName in) {
                done = false;
            }

        }

        return done;

    }

    /**
     * Appends elements of bl to blist.
     *
     * @param bl list of bindings to be appended to the other list.
     * @param blist complete list of the bindings.
     */
    private void appendChunk(BindingListHolder bl,BindingListHolder blist) {

        int length;
        if ( blist.value != null ) {
            length = bl.value.length + blist.value.length;
        } else {
            length = bl.value.length;
        }

        Binding[] bd = new Binding[length];

        if ( blist.value != null ) {
            for(int k=0; k<blist.value.length; k++) {
                bd[k] = blist.value[k];
            }
        }

        for (int i=0; i<bl.value.length ; i++) {
            if ( blist.value != null ) {
                bd[i+blist.value.length] = bl.value[i];
            } else {
                bd[i] = bl.value[i];
            }
        }

        blist.value = bd;

    }


}

