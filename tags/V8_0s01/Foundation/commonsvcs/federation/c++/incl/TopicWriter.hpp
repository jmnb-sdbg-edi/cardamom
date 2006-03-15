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
#ifndef _CDMW_FOUNDATION_COMMONSVCS_TOPIC_WRITER_HPP_
#define _CDMW_FOUNDATION_COMMONSVCS_TOPIC_WRITER_HPP_

#include "Foundation/orbsupport/Codec.hpp"
#include "Foundation/orbsupport/TypeTraits.hpp"

/**
 * This class encapsulates the ability of writing a topic by taking
 * advantage of the available topic update framework.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0
 */
template <typename T>
class TopicWriter
{
public:
    typedef Cdmw::OrbSupport::Codec<T> Codec_t;

public:
    /**
     * Creates a writer for a given topic.
     *
     * @param topic_id the topic identifier.
     */
    TopicWriter(int topic_id) throw();

    /**
     * 
     */
    ~TopicWriter() throw();
public:
    void write(type_traits<T>::_in_type topic_data);

private:
    int m_topic_id;
    
};
#endif /* _CDMW_FOUNDATION_COMMONSVCS_TOPIC_WRITER_HPP_ */
