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
#ifndef _CDMW_FOUNDATION_ORB_SUPPORT_CODEC_IMPL_H_
#define _CDMW_FOUNDATION_ORB_SUPPORT_CODEC_IMPL_H_

void
::Cdmw::OrbSupport::Codec<CORBA::Boolean>::decode(const CORBA::OctetSeq & data,
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

void
Cdmw::OrbSupport::Codec<CORBA::ULongLong>::decode (const CORBA::OctetSeq & data,
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

#endif /* _CDMW_FOUNDATION_ORB_SUPPORT_CODEC_IMPL_H_ */
