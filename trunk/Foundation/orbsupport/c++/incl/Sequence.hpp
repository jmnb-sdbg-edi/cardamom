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


#ifndef INCL_ORBSUPPORT_SEQUENCE_HPP
#define INCL_ORBSUPPORT_SEQUENCE_HPP

#include "Foundation/orbsupport/CORBA.hpp"

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    /**
    * Namespace for CDMW OrbSupport types and data.
    */

    namespace OrbSupport
    {
        /**
        *Purpose:
        *<p>    STL iterator for CORBA Sequences
        *
        */
        
        template<typename Seq, typename ElemType>
        class SequenceIterator:
                public std::iterator<std::random_access_iterator_tag, ElemType,
                                     CORBA::Long>
        {
            public:
                typedef ElemType& reference;
                typedef SequenceIterator iterator;
                typedef CORBA::Long difference_type;

                SequenceIterator(Seq sequence)
                    : m_seq(sequence),
                      m_valid(true),
                      m_index(0)
                {
                }

                SequenceIterator(Seq sequence, bool)
                    : m_seq(sequence),
                      m_valid(false),
                      m_index(0)
                {
                }

                SequenceIterator(const SequenceIterator& other)
                    : m_seq(other.m_seq),
                      m_valid(other.m_valid),
                      m_index(other.m_index)
                {
                }

                bool operator==(const iterator& iter) const
                {
                    return &m_seq == &iter.m_seq
                        && m_valid == iter.m_valid
                        && (!m_valid || m_index == iter.m_index);
                }

                bool operator<(const iterator& iter) const
                {
                    return &m_seq == &iter.m_seq
                        && m_valid == iter.m_valid
                        && m_valid
                        && m_index < iter.m_index;
                }

                bool operator!=(const iterator& iter) const
                {
                    return !(*this == iter);
                }

                bool operator>(const iterator& iter) const
                {
                    return iter < *this;
                }

                bool operator<=(const iterator& iter) const
                {
                    return !(iter < *this);
                }

                bool operator>=(const iterator& iter) const
                {
                    return !(*this < iter);
                }

                reference operator*() const
                {
                    return m_seq[m_index];
                }

                iterator& operator++()
                {
                    incr();
                    return *this;
                }

                iterator operator++(int)
                {
                    iterator tmp = *this;
                    incr();
                    return tmp;
                }

                iterator& operator--()
                {
                    decr();
                    return *this;
                }

                iterator operator--(int)
                {
                    iterator tmp = *this;
                    decr();
                    return tmp;
                }

                iterator& operator+=(difference_type delta)
                {
                    incr(delta);
                    return *this;
                }

                iterator& operator-=(difference_type delta)
                {
                    decr(delta);
                    return *this;
                }

                iterator operator+(difference_type delta) const
                {
                    iterator tmp = *this;
                    return tmp += delta;
                }

                iterator operator-(difference_type delta) const
                {
                    iterator tmp = *this;
                    return tmp -= delta;
                }

                reference operator[](CORBA::ULong index)
                {
                    return m_seq[index];
                }

            private:
                void incr()
                {
                    if (m_valid)
                    {
                        m_index++;
                        m_valid = m_index >= m_seq.length();
                    }
                }

                void incr(difference_type delta)
                {
                    if (m_valid)
                    {
                        m_index += delta;
                        m_valid = m_index >= m_seq.length();
                    }
                }

                void decr()
                {
                    if (m_valid)
                    {
                        m_index--;
                        m_valid = m_index >= 0;
                    }
                }

                void decr(difference_type delta)
                {
                    if (m_valid)
                    {
                        m_index -= delta;
                        m_valid = m_index >= 0;
                    }
                }

                Seq m_seq;
                bool m_valid;
                CORBA::ULong m_index;
        };

        /**
        *Purpose:
        *<p>    STL container wrapper for CORBA Sequences
        *
        */
        template<class Seq, typename ElemType>
        class Sequence: public Seq
        {
            public:
                typedef SequenceIterator<Seq&, ElemType> iterator;

                Sequence()
                {
                }

                Sequence(CORBA::ULong maximum)
                    : Seq(maximum)
                {
                }

                iterator begin()
                {
                    return iterator(*this);
                }

                iterator end()
                {
                    return iterator(*this, false);
                }
        };

        /**
        *Purpose:
        *<p>    STL container wrapper for CORBA Sequences
        *
        */
        template<typename Seq, typename ElemType>
        class SequenceWrapper
        {
            public:
                typedef SequenceIterator<Seq, ElemType> iterator;

                SequenceWrapper(Seq seq)
                    : m_seq(seq)
                {
                }

                iterator begin()
                {
                    return iterator(m_seq);
                }

                iterator end()
                {
                    return iterator(m_seq, false);
                }

            private:
                Seq m_seq;
        };

    } // End namespace OrbSupport
} // End namespace Cdmw

#endif // INCL_ORBSUPPORT_SEQUENCE_HPP

