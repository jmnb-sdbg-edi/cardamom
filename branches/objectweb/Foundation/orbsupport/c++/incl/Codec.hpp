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


#ifndef INCL_ORBSUPPORT_CODEC_T_HPP
#define INCL_ORBSUPPORT_CODEC_T_HPP

#include <tao/CDR.h>
#include <tao/OctetSeqC.h>
#include <tao/IOP_CodecC.h>
#include <Foundation/orbsupport/TypeTraits.hpp>
#include <Foundation/common/Assert.hpp>
#include <string>

namespace Cdmw 
{
namespace OrbSupport
{

template <typename DATA>
class Codec
{
public:
    typedef typename type_traits<DATA>::_var_type     _var_type;
    typedef typename type_traits<DATA>::_in_type      _in_type;
    typedef typename type_traits<DATA>::_out_type     _out_type;
    typedef typename type_traits<DATA>::_inout_type   _inout_type;
    typedef typename type_traits<DATA>::_retn_type    _retn_type;

    /// Constructor.
    Codec (CORBA::ORB_ptr orb,
           CORBA::Octet   major = 1,
           CORBA::Octet   minor = 1 )
        throw (CORBA::SystemException)
        : m_orb (CORBA::ORB::_duplicate(orb)),
          m_major (major),
          m_minor (minor)
    {
        CDMW_ASSERT(!CORBA::is_nil(orb));
    }

    /**
     * Encode 
     */
    CORBA::OctetSeq * encode (_in_type data)
        throw (CORBA::SystemException,
               IOP::Codec::InvalidTypeForEncoding)
    {
        try {
            TAO_OutputCDR cdr ((size_t) 0,            // size
                               (int) ACE_CDR_BYTE_ORDER,
                               (ACE_Allocator *) 0,   // buffer_allocator
                               (ACE_Allocator *) 0,   // data_block_allocator
                               (ACE_Allocator *) 0,   // message_block_allocator
                               0,                     // memcpy_tradeoff
                               this->m_major,
                               this->m_minor);
            
            if ((cdr << TAO_OutputCDR::from_boolean (TAO_ENCAP_BYTE_ORDER))
                && (cdr << data))
                {
                    CORBA::OctetSeq_var safe_octet_seq = new CORBA::OctetSeq;
                    
                    safe_octet_seq->length (cdr.total_length ());
                    CORBA::Octet *buf = safe_octet_seq->get_buffer ();
                    
                    for (const ACE_Message_Block *i = cdr.begin ();
                         i != 0;
                         i = i->cont ())
                        {
                            size_t len = i->length ();
                            ACE_OS_String::memcpy (buf, i->rd_ptr (), len);
                            buf += len;
                        }
                    
                    return safe_octet_seq._retn ();
                }
            throw CORBA::MARSHAL ();
        } catch (const std::bad_alloc &) {
#if CDMW_ORB_VER < 14
            throw CORBA::NO_MEMORY (CORBA_SystemException::_tao_minor_code (TAO_DEFAULT_MINOR_CODE,
#else
            throw CORBA::NO_MEMORY (CORBA::SystemException::_tao_minor_code (TAO_DEFAULT_MINOR_CODE,
#endif
                                    ENOMEM),
                                    CORBA::COMPLETED_NO);
        }
    }
  

    /**
     * Decode 
     */
    void decode (const CORBA::OctetSeq & data,
                 _inout_type inout)
        throw (CORBA::SystemException,
               IOP::Codec::FormatMismatch)
    {
        std::auto_ptr<TAO_InputCDR> cdr_ptr;
        std::auto_ptr<ACE_Message_Block> mb_ptr;
        if ( data.mb() == 0) {
            // If data's MessageBlock is null, create a new MessageBlock (do a copy).
            // This case is only used if the data OctetSeq is the one resulting from 
            // the encode in the same process.
            // In case of the OctetSeq is send to a remote process, it is recreated
            // by ORB into the remote process with a non-null MessageBlock.
            // Thus, for Cardamom, this case is only used by Unit Tests.

            std::auto_ptr<ACE_Message_Block> tmp( 
                new ACE_Message_Block (data.length () + 2 * ACE_CDR::MAX_ALIGNMENT) );
            mb_ptr = tmp;    
            
            ACE_CDR::mb_align (mb_ptr.get());
          
            ACE_OS::memcpy (mb_ptr->rd_ptr (), data.get_buffer (), data.length ());
          
            size_t rd_pos = mb_ptr->rd_ptr () - mb_ptr->base ();
            size_t wr_pos = mb_ptr->wr_ptr () - mb_ptr->base () + data.length ();
          
            std::auto_ptr<TAO_InputCDR> tmp2(
                new TAO_InputCDR (mb_ptr->data_block (),
                                  ACE_Message_Block::DONT_DELETE,
                                  rd_pos,
                                  wr_pos,
                                  ACE_CDR_BYTE_ORDER,
                                  this->m_major,
                                  this->m_minor,
                                  this->m_orb->orb_core()));
            cdr_ptr = tmp2;

        } else {
            // data.mb() is not null
            std::auto_ptr<TAO_InputCDR> tmp(
                new TAO_InputCDR (data.mb (),
                                  ACE_CDR_BYTE_ORDER,
                                  this->m_major,
                                  this->m_minor,
                                  this->m_orb->orb_core()));
            cdr_ptr = tmp;
        }
        
        TAO_InputCDR& cdr = *(cdr_ptr.get());
        CORBA::Boolean byte_order;
        
        bool result = false;
        if (cdr >> TAO_InputCDR::to_boolean (byte_order))
            {
                cdr.reset_byte_order (ACE_static_cast (int, byte_order));
                if (cdr >> inout)
                    result = true;
            }
        if (!result)
            throw IOP::Codec::FormatMismatch ();
    }     


private:
    

    // The ORB encapsulating the ORB Core to be used when decoding values from a CDR
    /// encapsulation.
    CORBA::ORB_var m_orb;

    /// The major GIOP version associated with this Codec.
    CORBA::Octet m_major;
    
    /// The minor GIOP version associated with this Codec.
    CORBA::Octet m_minor;
};



//
// Specialization for std::string
//
template <>
class Codec<std::string>
{
public:
    typedef type_traits<std::string>::_var_type     _var_type;
    typedef type_traits<std::string>::_in_type      _in_type;
    typedef type_traits<std::string>::_out_type     _out_type;
    typedef type_traits<std::string>::_inout_type   _inout_type;
    typedef type_traits<std::string>::_retn_type    _retn_type;

    /// Constructor.
    Codec (CORBA::ORB_ptr orb,
           CORBA::Octet   major = 1,
           CORBA::Octet   minor = 1 )
        throw (CORBA::SystemException)
        : m_CORBA_String_codec (orb,major,minor)
    {
    }
    
    CORBA::OctetSeq * encode (_in_type data)
        throw (CORBA::SystemException,
               IOP::Codec::InvalidTypeForEncoding)
    {
        return m_CORBA_String_codec.encode(data.c_str());
    }
  
    void decode (const CORBA::OctetSeq & data,
                 _inout_type inout)
        throw (CORBA::SystemException,
               IOP::Codec::FormatMismatch)
    {
        CORBA::String_var s = CORBA::string_dup("");
        m_CORBA_String_codec.decode(data,s.inout());
        inout = s.in();
    }    

private:
    Codec<CORBA::String_var> m_CORBA_String_codec;
};




//
// Specialization of decode for CORBA::Boolean
//
template<>
void Codec<CORBA::Boolean>::decode(const CORBA::OctetSeq & data,
                                   _inout_type inout)
    throw (CORBA::SystemException,
           IOP::Codec::FormatMismatch)
{
    std::auto_ptr<TAO_InputCDR> cdr_ptr;
    std::auto_ptr<ACE_Message_Block> mb_ptr;
    if ( data.mb() == 0) {
          
        std::auto_ptr<ACE_Message_Block> tmp( 
            new ACE_Message_Block (data.length () + 2 * ACE_CDR::MAX_ALIGNMENT) );
        mb_ptr = tmp;    
            
        ACE_CDR::mb_align (mb_ptr.get());
          
        ACE_OS::memcpy (mb_ptr->rd_ptr (), data.get_buffer (), data.length ());
          
        size_t rd_pos = mb_ptr->rd_ptr () - mb_ptr->base ();
        size_t wr_pos = mb_ptr->wr_ptr () - mb_ptr->base () + data.length ();
          
        std::auto_ptr<TAO_InputCDR> tmp2(
            new TAO_InputCDR (mb_ptr->data_block (),
                              ACE_Message_Block::DONT_DELETE,
                              rd_pos,
                              wr_pos,
                              ACE_CDR_BYTE_ORDER,
                              this->m_major,
                              this->m_minor,
                              this->m_orb->orb_core()));
        cdr_ptr = tmp2;
    } else {
        std::auto_ptr<TAO_InputCDR> tmp(
            new TAO_InputCDR (data.mb (),
                              ACE_CDR_BYTE_ORDER,
                              this->m_major,
                              this->m_minor,
                              this->m_orb->orb_core()));
        cdr_ptr = tmp;
    }
        
    TAO_InputCDR& cdr = *(cdr_ptr.get());
        
    CORBA::Boolean byte_order;
        
    bool result = false;
    if (cdr >> TAO_InputCDR::to_boolean (byte_order))
    {
        cdr.reset_byte_order (ACE_static_cast (int, byte_order));
        ACE_InputCDR::to_boolean tb(inout);
        if (cdr >> tb)
            result = true;
    }
    if (!result)
        throw IOP::Codec::FormatMismatch ();
}    

template<>
void Codec<CORBA::ULongLong>::decode (const CORBA::OctetSeq & data,
                                      _inout_type inout)
    throw (CORBA::SystemException,
           IOP::Codec::FormatMismatch)
{
    std::auto_ptr<TAO_InputCDR> cdr_ptr;
    std::auto_ptr<ACE_Message_Block> mb_ptr;
    //        if ( data.mb() == 0) {
    // If data's MessageBlock is null, create a new MessageBlock (do a copy).
    // This case is only used if the data OctetSeq is the one resulting from 
    // the encode in the same process.
    // In case of the OctetSeq is send to a remote process, it is recreated
    // by ORB into the remote process with a non-null MessageBlock.
    // Thus, for Cardamom, this case is only used by Unit Tests.
    
    std::auto_ptr<ACE_Message_Block> tmp(new ACE_Message_Block (data.length () + 2 * ACE_CDR::MAX_ALIGNMENT) );
    mb_ptr = tmp;    
    
    ACE_CDR::mb_align (mb_ptr.get());
    
    ACE_OS::memcpy (mb_ptr->rd_ptr (), data.get_buffer (), data.length ());
    
    size_t rd_pos = mb_ptr->rd_ptr () - mb_ptr->base ();
    size_t wr_pos = mb_ptr->wr_ptr () - mb_ptr->base () + data.length ();
    
    std::auto_ptr<TAO_InputCDR> tmp2(new TAO_InputCDR (mb_ptr->data_block (),
                                                       ACE_Message_Block::DONT_DELETE,
                                                       rd_pos,
                                                       wr_pos,
                                                       ACE_CDR_BYTE_ORDER,
                                                       this->m_major,
                                                       this->m_minor,
                                                       this->m_orb->orb_core()));
    cdr_ptr = tmp2;
    
    //         } else {
    //             // data.mb() is not null
    //             std::auto_ptr<TAO_InputCDR> tmp(
    //                 new TAO_InputCDR (data.mb (),
    //                                   ACE_CDR_BYTE_ORDER,
    //                                   this->m_major,
    //                                   this->m_minor,
    //                                   this->m_orb->orb_core()));
    //             cdr_ptr = tmp;
    //         }
        
    TAO_InputCDR& cdr = *(cdr_ptr.get());
    CORBA::Boolean byte_order;
        
    bool result = false;
    if (cdr >> TAO_InputCDR::to_boolean (byte_order))
    {
        cdr.reset_byte_order (ACE_static_cast (int, byte_order));
        if (cdr >> inout)
            result = true;
    }
    if (!result)
        throw IOP::Codec::FormatMismatch ();
}  

} // namespace OrbSupport
} // namespace Cdmw
#endif //  INCL_ORBSUPPORT_CODEC_T_HPP

