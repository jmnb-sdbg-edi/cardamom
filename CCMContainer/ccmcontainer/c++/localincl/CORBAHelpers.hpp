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


#ifndef INCL_CDMW_CCM_CORBA_HELPERS_HPP
#define INCL_CDMW_CCM_CORBA_HELPERS_HPP

#include <functional>
#include <string>

namespace Cdmw
{
/**
 *  Purpose:
 * <p> Helper template function de duplicate sequences.
 */  
template<class SEQ>
SEQ* duplicate_sequence(const SEQ& seq)
{
    
    SEQ* _seq = new SEQ(seq.length());
    _seq->length(seq.length());
    
    CORBA::ULong seqlen = seq.length();
    for (CORBA::ULong count = 0; count < seqlen; ++count)
        (*_seq)[count] = seq[count];
    
    return _seq;
}

/**
 * Template binary function for use with stl containers to test two object references
 * for equivalence.
 */
template <typename T_var>
struct is_equivalent_to : std::binary_function<T_var,T_var,bool> {
    bool operator() (const T_var& x, const T_var& y) const
    {
        return x->_is_equivalent(y.in());
    }
};


/**
 * Template binary function for use with stl containers to test whether an object's interface is
 * identical to or inherits, directly or indirectly, from another object's interface.
 */       
template <typename T>
struct is_a : std::binary_function<T,std::string,bool> {
    bool operator() (const T& x, const std::string & repid) const
    {
        return x->_is_a(repid.c_str());
    }
};
    
}; // End of namespace Cdmw

#endif // INCL_CDMW_CCM_CORBA_HELPERS_HPP

