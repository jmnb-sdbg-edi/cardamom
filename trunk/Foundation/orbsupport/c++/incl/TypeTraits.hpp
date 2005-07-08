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


#ifndef INCL_CDMW_ORBSUPPORT_TYPE_TRAITS_HPP
#define INCL_CDMW_ORBSUPPORT_TYPE_TRAITS_HPP

#include <string>
#include "Foundation/orbsupport/CORBA.hpp"

namespace Cdmw 
{
namespace OrbSupport
{

    
/**
 * The type_traits template is used to hide differences between all CORBA
 * types (basic, struct, sequences, object references...).
 * The template parameter should be a CORBA type.
 * Whatever this parameter is, type_traits defines some types which
 * could always be used:<ul>
 * <li><b>_var_type:</b>   type with memory self-management (to be used to 
 *                         store an instance of the CORBA type for example.)
 * <li><b>_in_type:</b>    type to be used as in parameter of an operation.
 * <li><b>_out_type:</b>   type to be used as out parameter of an operation.
 * <li><b>_inout_type:</b> type to be used as inout parameter of an operation.
 * <li><b>_retn_type:</b>  type to be used as return parameter of an operation.
 * </ul>
 * 
 * The default definition of type_traits is usable for CORBA Objects.
 * This means that only a template instantiation is necessary to use 
 * type_traits with a CORBA interface definition. e.g.: <br>
 * <i>Cdmw::OrbSupport::type_traits<MyModule::MyInterface>::_var_type
 *       myObjectReference;</i>
 *
 * CARDAMOM already redefines and instantiate type_traits templates for the
 * CORBA basic types: <br>
 * <i>CORBA::Boolean, CORBA::Short, CORBA::UShort, CORBA::Long, CORBA::ULong, 
 * CORBA::LongLong, CORBA::ULongLong, CORBA::LongDouble, CORBA::Float, 
 * CORBA::Double, CORBA::Octet, CORBA::Char, CORBA::WChar, char* (string) 
 * and CORBA::WChar* (wstring).</i>
 * 
 * For fixed-length struct or unions, the type_traits template should be 
 * redefined and instantiated using the following macro: <br>
 * <b> DEFINE_FIXED_LENGTH_DATA_TRAITS(MyModule::MyStruct); </b>
 * For variable-length struct or unions and for sequences, the type_traits 
 * template should be  redefined and instantiated using the following macro: <br>
 * <b> DEFINE_VARIABLE_LENGTH_DATA_TRAITS(MyModule::MyStruct); </b>
 *
 * If a template redefinition is missing for a non-interface CORBA type,
 * the following compilation error could occur: <br>
 * <i> "no type named `_ptr_type' in `class T'" </i>
 */    
template<typename T>
struct type_traits
{
    /**
     * type of _ptr (specific to CORBA Objects).
     */
    typedef typename T::_ptr_type    _ptr_type;

    /**
     * type with memory self-management.
     */
    typedef typename T::_var_type    _var_type;

    /**
     * type for in parameter.
     */
    typedef _ptr_type                _in_type;

    /**
     * type for inout parameter.
     */
    typedef _ptr_type&               _inout_type;

    /**
     * type for out parameter.
     */
    typedef _ptr_type&               _out_type;

    /**
     * type for return parameter.
     */
    typedef _ptr_type                _retn_type;
    
    /**
     * Returns a default value for the template type.
     * This operation is to be used for _var_type initialization.
     */
    static _retn_type _default_value() { 
        return T::_nil();
    };
    
    /**
     * Converts a _var_type into a _in_type. 
     */
    static _in_type _in(const _var_type& v) { 
        return v.in();
    };
    
    /**
     * Converts a _var_type into a _retn_type.
     */
    static _retn_type _retn(_var_type& v) { 
        return v._retn();
    };
    
    /**
     * Converts a _var_type into a _inout_type.
     */
    static _inout_type _inout(_var_type& v) { 
        return v.inout();
    };
    
    /**
     * Returns a copy of the _in_type.
     */
    static _retn_type _copy(_in_type i) { 
        return T::_duplicate(i);
    };
};



/**    
 * Traits class for simple types and enum
 */
#define DEFINE_SIMPLE_DATA_TRAITS(T) \
namespace Cdmw { \
namespace OrbSupport { \
template<> \
struct type_traits<T> \
{ \
    typedef T                       _var_type; \
    typedef T                       _in_type; \
    typedef T&                      _inout_type; \
    typedef T&                      _out_type; \
    typedef T                       _retn_type;  \
 \
    static _retn_type _default_value() { \
        return T(); \
    }; \
    static _in_type _in(const _var_type& v) { \
        return v; \
    }; \
    static _retn_type _retn(_var_type & v) { \
        return v; \
    }; \
    static _inout_type _inout(_var_type & v) { \
        return v; \
    }; \
    static _retn_type _copy(_in_type i) { \
        return i; \
    }; \
}; \
}; \
}



/**
 * Traits class for fixed-length struct and union 
 */
#define DEFINE_FIXED_LENGTH_DATA_TRAITS(T) \
namespace Cdmw { \
namespace OrbSupport { \
template<> \
struct type_traits<T> \
{ \
    typedef T::_var_type   _var_type; \
    typedef const T&                _in_type; \
    typedef T&                      _inout_type; \
    typedef T&                      _out_type; \
    typedef T                       _retn_type;  \
 \
    static _retn_type _default_value() { \
        return T(); \
    }; \
    static _in_type _in(const _var_type& v) { \
        return v.in(); \
    }; \
    static _retn_type _retn(_var_type & v) { \
        return v._retn(); \
    }; \
    static _inout_type _inout(_var_type & v) { \
        return v.inout(); \
    }; \
    static _retn_type _copy(_in_type i) { \
        return i; \
    }; \
}; \
}; \
}


/**   
 * Traits class for variable-length struct and union 
 * and for sequence types
 */
#define DEFINE_VARIABLE_LENGTH_DATA_TRAITS(T) \
namespace Cdmw { \
namespace OrbSupport { \
template<> \
struct type_traits<T> \
{ \
    typedef T::_var_type            _var_type; \
    typedef const T&                _in_type; \
    typedef T&                      _inout_type; \
    typedef T*&                     _out_type; \
    typedef T*                      _retn_type;  \
 \
    static _retn_type _default_value() { \
        return new T(); \
    }; \
    static _in_type _in(const _var_type& v) { \
        return v.in(); \
    }; \
    static _retn_type _retn(_var_type & v) { \
        return v._retn(); \
    }; \
    static _inout_type _inout(_var_type & v) { \
        return v.inout(); \
    }; \
    static _retn_type _copy(_in_type i) { \
        return new T(i); \
    }; \
}; \
}; \
}

    


//
// Define traits class for fixed-length array types
//   TODO: check in inout and out types
//
#define DEFINE_FIXED_LENGTH_ARRAY_TRAITS(T) \
namespace Cdmw { \
namespace OrbSupport { \
template<> \
struct type_traits<T> \
{ \
    typedef T::_var_type            _var_type; \
    typedef T                       _slice_type; \
    typedef const _slice_type       _in_type; \
    typedef _slice_type*            _inout_type; \
    typedef _slice_type*            _out_type; \
    typedef _slice_type*            _retn_type; \
 \
    static _retn_type _default_value() { \
        return new _slice_type(); \
    }; \
    static _in_type _in(const _var_type& v) { \
        return v.in(); \
    }; \
    static _retn_type _retn(_var_type & v) { \
        return v._retn(); \
    }; \
    static _inout_type _inout(_var_type & v) { \
        return v.inout(); \
    }; \
    static _retn_type _copy(_in_type i) { \
        return new _slice_type(i); \
    }; \
}; \
}; \
}

//
// Define traits class for variable-length array types
//   TODO: check in inout and out types
//
#define DEFINE_VARIABLE_LENGTH_ARRAY_TRAITS(T) \
namespace Cdmw { \
namespace OrbSupport { \
template<> \
struct type_traits<T> \
{ \
    typedef T::_var_type            _var_type; \
    typedef T                       _slice_type; \
    typedef const _slice_type*      _in_type; \
    typedef _slice_type*            _inout_type; \
    typedef _slice_type*&           _out_type; \
    typedef _slice_type*            _retn_type; \
 \
    static _retn_type _default_value() { \
        return new _slice_type(); \
    }; \
    static _in_type _in(const _var_type& v) { \
        return v.in(); \
    }; \
    static _retn_type _retn(_var_type & v) { \
        return v._retn(); \
    }; \
    static _inout_type _inout(_var_type & v) { \
        return v.inout(); \
    }; \
    static _retn_type _copy(_in_type i) { \
        return new _slice_type(i); \
    }; \
}; \
}; \
}



// SPECIALISE type_traits for CORBA String (char* and CORBA::String_var)
template <>
struct type_traits<char*>
{
    typedef CORBA::String_var   _var_type;
    typedef const CORBA::Char*  _in_type;
    typedef CORBA::Char*&       _inout_type;
    typedef CORBA::Char*&       _out_type;
    typedef CORBA::Char*        _retn_type;
    
    static _retn_type _default_value() {
        return CORBA::string_dup("");
    };
    static _in_type _in(const _var_type& v) {
        return v.in();
    };
    static _retn_type _retn(_var_type& v) { 
        return v._retn();
    };
    static _inout_type _inout(_var_type& v) { 
        return v.inout();
    };
    static _retn_type _copy(_in_type i) {
        return CORBA::string_dup(i);
    };
};

template <>
struct type_traits<CORBA::String_var>
{
    typedef CORBA::String_var   _var_type;
    typedef const CORBA::Char*  _in_type;
    typedef CORBA::Char*&       _inout_type;
    typedef CORBA::Char*&       _out_type;
    typedef CORBA::Char*        _retn_type;
    
    static _retn_type _default_value() {
        return CORBA::string_dup("");
    };
    static _in_type _in(const _var_type& v) {
        return v.in();
    };
    static _retn_type _retn(_var_type& v) { 
        return v._retn();
    };
    static _inout_type _inout(_var_type& v) { 
        return v.inout();
    };
    static _retn_type _copy(_in_type i) {
        return CORBA::string_dup(i);
    };
};

// SPECIALISE type_traits for std::string
template <>
struct type_traits<std::string>
{
    typedef std::string         _var_type;
    typedef const std::string&  _in_type;
    typedef std::string&        _inout_type;
    typedef std::string&        _out_type;
    typedef std::string         _retn_type;
    
    static _retn_type _default_value() {
        return std::string("");
    };
    static _in_type _in(const _var_type& v) {
        return v;
    };
    static _retn_type _retn(_var_type& v) { 
        return v;
    };
    static _inout_type _inout(_var_type& v) { 
        return v;
    };
    static _retn_type _copy(_in_type i) {
        return i;
    };
};


// SPECIALISE type_traits for CORBA WString (WChar* and CORBA::WString_var)
template <>
struct type_traits<CORBA::WChar*>
{
    typedef CORBA::WString_var   _var_type;
    typedef const CORBA::WChar*  _in_type;
    typedef CORBA::WChar*&       _inout_type;
    typedef CORBA::WChar*&       _out_type;
    typedef CORBA::WChar*        _retn_type;
    
/*****  TODO: specialize this if needed
    static _retn_type _default_value() {
        return CORBA::wstring_dup("");    // error: cannot convert "const wchar_t*" in "const WChar*"
    };
*/
    static _in_type _in(const _var_type& v) {
        return v.in();
    };
    static _retn_type _retn(_var_type& v) { 
        return v._retn();
    };
    static _inout_type _inout(_var_type& v) { 
        return v.inout();
    };
    static _retn_type _copy(_in_type i) {
        return CORBA::wstring_dup(i);
    };
};

template <>
struct type_traits<CORBA::WString_var>
{
    typedef CORBA::WString_var   _var_type;
    typedef const CORBA::WChar*  _in_type;
    typedef CORBA::WChar*&       _inout_type;
    typedef CORBA::WChar*&       _out_type;
    typedef CORBA::WChar*        _retn_type;
    
/*****  TODO: specialize this if needed
    static _retn_type _default_value() {
        return CORBA::wstring_dup(L"");    // error: cannot convert "const wchar_t*" in "const WChar*" 
    };
*/
    static _in_type _in(const _var_type& v) {
        return v.in();
    };
    static _retn_type _retn(_var_type& v) { 
        return v._retn();
    };
    static _inout_type _inout(_var_type& v) { 
        return v.inout();
    };
    static _retn_type _copy(_in_type i) {
        return CORBA::wstring_dup(i);
    };
};




} // namespace OrbSupport
} // namespace Cdmw


// SPECIALISE type_traits for common CORBA data types
DEFINE_SIMPLE_DATA_TRAITS(CORBA::Boolean);
DEFINE_SIMPLE_DATA_TRAITS(CORBA::Short);
DEFINE_SIMPLE_DATA_TRAITS(CORBA::UShort);
DEFINE_SIMPLE_DATA_TRAITS(CORBA::Long);
DEFINE_SIMPLE_DATA_TRAITS(CORBA::ULong);
DEFINE_SIMPLE_DATA_TRAITS(CORBA::LongLong);
DEFINE_SIMPLE_DATA_TRAITS(CORBA::ULongLong);
DEFINE_SIMPLE_DATA_TRAITS(CORBA::LongDouble);
DEFINE_SIMPLE_DATA_TRAITS(CORBA::Float);
DEFINE_SIMPLE_DATA_TRAITS(CORBA::Double);
DEFINE_SIMPLE_DATA_TRAITS(CORBA::Char);


#endif //  INCL_CDMW_ORBSUPPORT_TYPE_TRAITS_HPP

