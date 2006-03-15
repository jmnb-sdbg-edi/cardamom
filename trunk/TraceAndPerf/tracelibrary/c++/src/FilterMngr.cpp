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


#include <functional>

#include <Foundation/osthreads/MutexGuard.hpp>

#include <TraceAndPerf/tracelibrary/FilterMngr.hpp>


/**
 * ECR-0123
 * In addition to the addition of a new field (the component name) to
 * the traces, the handling of trace levels has been completely rewritten.
 * The main difference with the previous implementation is that the number
 * of possible levels is now fixed and equals to the constant NB_BITS.
 */


// {{{ functors

namespace
{
    /**
     * Check a bitmask against the bitmask of a Filter.
     */
    template <typename T1, typename T2>
    struct bitmask_equal_to: public std::binary_function<T1, T2, bool>
    {
        bool
        operator()(const T1& fmask, const T2& mask) const
        {
            return fmask.get_levels() == mask;
        }
    };


    /**
     * Check if a Filter equals another Filter while ignoring
     * their respective bitmasks.
     */
    template <typename T1, typename T2>
    struct filter_equal_to: public std::binary_function<T1, T2, bool>
    {
        bool
        operator()(const T1& f1, const T2& f2) const
        {
            return ((f1.get_component_name() == f2.get_component_name()) &&
                    (f1.get_domain() == f2.get_domain()));
        }
    };
}

// }}}


namespace Cdmw
{

namespace Trace
{


// {{{ functor is_contained

/**
 * Check if a Filter (f1) is contained in another Filter (f2).
 */
template <typename T1, typename T2>
struct is_contained: public std::binary_function<T1, T2, bool>
{
    bool
    operator()(const T1& f1, const T2& f2) const
    {
        if ((f2.get_component_name() == CdmwTrace::ALL_COMPONENT_NAMES) ||
            (f2.get_component_name() == f1.get_component_name()))
        {
            if ((f2.get_domain() == CdmwTrace::ALL_DOMAINS) ||
                (f2.get_domain() == f1.get_domain()))
            {
                // bitwise AND
                // if all the levels defined in f1 are contained in f2
                // then the result of AND equals the levels of f1
                if ((f2.get_levels() & f1.get_levels()) == f1.get_levels()) {
                    return true;
                }
            }
        }

        return false;
    }
};

// }}}


// {{{1 implementation of operations from the Filter class


// {{{2 constructors

/**
 * Create a new Filter with a generic component name and
 * a generic domain.
 */
Filter::Filter(void)
    : m_componentName(CdmwTrace::ALL_COMPONENT_NAMES),
      m_domain(CdmwTrace::ALL_DOMAINS)
{
    bitmask mask;
    m_levels = mask;
}


/**
 * Create a new Filter.
 *
 * @param domain the domain to match
 */
Filter::Filter(const std::string& domain)
    : m_componentName(CdmwTrace::ALL_COMPONENT_NAMES),
      m_domain(domain)
{
    bitmask mask;
    m_levels = mask;
}


/**
 * Create a new Filter.
 *
 * @param domain the domain to match
 * @param levels bit mask for the trace levels
 */
Filter::Filter(const std::string& domain,
               const bitmask levels)
    : m_componentName(CdmwTrace::ALL_COMPONENT_NAMES),
      m_domain(domain),
      m_levels(levels)
{
    // NOOP
}


/**
 * Create a new Filter.
 *
 * @param componentName the component name to match
 * @param domain the domain to match
 */
Filter::Filter(const std::string& componentName,
               const std::string& domain)
    : m_componentName(componentName),
      m_domain(domain)
{
    bitmask mask;
    m_levels = mask;
}


/**
 * Create a new Filter.
 *
 * @param componentName the component name to match
 * @param domain the domain to match
 * @param levels bit mask for the trace levels
 */
Filter::Filter(const std::string& componentName,
               const std::string& domain,
               const bitmask levels)
    : m_componentName(componentName),
      m_domain(domain),
      m_levels(levels)
{
    // NOOP
}

// }}}2


// }}}1


// {{{1 implementation of operations from the FilterMngr class


// {{{2 constructor

/**
 * Constructor.
 */
FilterMngr::FilterMngr(void)
    throw()
{
    // NOOP
}

// }}}2

// {{{2 destructor

/**
 * Destructor.
 */
FilterMngr::~FilterMngr()
    throw()
{
    // NOOP
}

// }}}2

// {{{2 activate_level()

/**
 * Activate the specified level.
 * If the level is not valid, this operation does nothing.
 *
 * @param domain activate the specified level of this domain.
 * @param level the level to activate.
 *
 * @exception OutOfMemoryException
 */
void
FilterMngr::activate_level(const std::string& domain,
                           const long level)
    throw(OutOfMemoryException)
{
    activate_level(CdmwTrace::ALL_COMPONENT_NAMES, domain, level);
}


/**
 * Activate the specified level.
 * If the level is not valid, this operation does nothing.
 *
 * @param componentName the component to which the domain is associated.
 * @param domain activate the specified level of this domain.
 * @param level the level to activate.
 *
 * @exception OutOfMemoryException
 */
void
FilterMngr::activate_level(const std::string& componentName,
                           const std::string& domain,
                           const long level)
    throw(OutOfMemoryException)
{
    // acquire mutex
    CDMW_MUTEX_GUARD(m_filterMngr_mtx);

    // check the level
    if ((level != CdmwTrace::ALL_VALUES) &&
        !((level >= 0) && ((unsigned long)level < NB_BITS)))
    {
        return;
    }

    // if we have already activated everything
    // there is no need to add a filter that activates level(s)

    // bitset<bits>::count() returns the number of bits set to 1
    if ((m_filters.size() == 0) &&
        (m_filter_any.get_levels().count() == NB_BITS))
    {
        return;
    }

    try {
        if (componentName == CdmwTrace::ALL_COMPONENT_NAMES) {
            // activate the level(s) of any domain of any component
            if (domain == CdmwTrace::ALL_DOMAINS) {
                if (level == CdmwTrace::ALL_VALUES) {
                    // activate all the levels of the most generic filter
                    m_filter_any.activate_all_levels();

                    // and discard everything from the list as anything is
                    // activated from now on
                    m_filters.clear();
                }
                // activate a particular level of any domain of any component
                else {
                    // update the level of the most generic filter
                    m_filter_any.activate_level(level);

                    // update the level of all the other filters (if any)
                    FilterCtr::iterator it = m_filters.begin();
                    FilterCtr::iterator it_end = m_filters.end();

                    for (; it != it_end; ++it) {
                        (*it).activate_level(level);
                    }
                }
            }

            // activate the level(s) of a particular domain of any component
            else {
                FilterCtr::iterator it = m_filters.begin();
                FilterCtr::iterator it_end = m_filters.end();

                // update the level(s) of the filters that
                // correspond to the specified domain
                for (; it != it_end; ++it) {
                    if ((*it).get_domain() == domain) {
                        if (level == CdmwTrace::ALL_VALUES) {
                            (*it).activate_all_levels();
                        }
                        else {
                            (*it).activate_level(level);
                        }
                    }
                }

                // add the filter (C*, D, mask) to the list
                // if it already exists then it will be moved to the tail
                // of the list
                add_filter(CdmwTrace::ALL_COMPONENT_NAMES, domain, level);
            }
        }

        // componentName != CdmwTrace::ALL_COMPONENT_NAMES
        else {
            // activate level(s) of any domain of a particular component
            if (domain == CdmwTrace::ALL_DOMAINS) {
                FilterCtr::iterator it = m_filters.begin();
                FilterCtr::iterator it_end = m_filters.end();

                // update the level(s) of the filters of the list that
                // correspond to the specified component name
                for (; it != it_end; ++it) {
                    if ((*it).get_component_name() == componentName) {
                        if (level == CdmwTrace::ALL_VALUES) {
                            (*it).activate_all_levels();
                        }
                        else {
                            (*it).activate_level(level);
                        }
                    }
                }

                // add the filter (C, D*, mask) to the list
                // if it already exists then it will be moved to the tail
                // of the list
                add_filter(componentName, CdmwTrace::ALL_DOMAINS, level);
            }

            // activate level(s) of a particular domain of a particular component
            else {
                // add the filter (C, D, mask) to the list
                // if it already exists then it will be moved to the tail
                // of the list
                add_filter(componentName, domain, level);
            }
        }
    }
    catch (std::bad_alloc&) {
        CDMW_THROW(OutOfMemoryException);
    }
}

// }}}2

// {{{2 deactivate_level()

/**
 * Deactivate the specified level.
 * If the level is not valid, this operation does nothing.
 *
 * @param domain deactivate the specified level of this domain.
 * @param level the level to activate.
 *
 * @exception OutOfMemoryException
 */
void
FilterMngr::deactivate_level(const std::string& domain,
                             const long level)
    throw(OutOfMemoryException)
{
    deactivate_level(CdmwTrace::ALL_COMPONENT_NAMES, domain, level);
}


/**
 * Deactivate the specified level.
 * If the level is not valid, this operation does nothing.
 *
 * @param componentName the component to which the domain is associated.
 * @param domain activate the specified level of this domain.
 * @param level the level to activate.
 *
 * @exception OutOfMemoryException
 */
void
FilterMngr::deactivate_level(const std::string& componentName,
                             const std::string& domain,
                             const long level)
    throw(OutOfMemoryException)
{
    // acquire mutex
    CDMW_MUTEX_GUARD(m_filterMngr_mtx);

    // check the level
    if ((level != CdmwTrace::ALL_VALUES) &&
        !((level >= 0) && ((unsigned long)level < NB_BITS)))
    {
        return;
    }

    // if we have already deactivated everything
    // there is no need to add a filter that deactivates level(s)

    // bitset<bits>::count() returns the number of bits set to 1
    if ((m_filters.size() == 0) &&
        (m_filter_any.get_levels().count() == 0))
    {
        return;
    }

    try {
        if (componentName == CdmwTrace::ALL_COMPONENT_NAMES) {
            // deactivate the level(s) of any domain of any component
            if (domain == CdmwTrace::ALL_DOMAINS) {
                if (level == CdmwTrace::ALL_VALUES) {
                    // deactivate all the levels of the most generic filter
                    m_filter_any.deactivate_all_levels();

                    // and discard everything from the list as anything is
                    // deactivated from now on
                    m_filters.clear();
                }
                // deactivate a particular level of any domain of any component
                else {
                    // update the level of the most generic filter
                    m_filter_any.deactivate_level(level);

                    // update the level of all the other filters (if any)
                    FilterCtr::iterator it = m_filters.begin();
                    FilterCtr::iterator it_end = m_filters.end();

                    for (; it != it_end; ++it) {
                        (*it).deactivate_level(level);
                    }
                }
            }

            // deactivate level(s) of a particular domain of any component
            else {
                FilterCtr::iterator it = m_filters.begin();
                FilterCtr::iterator it_end = m_filters.end();

                // update the level(s) of the filters that
                // correspond to the specified domain
                for (; it != it_end; ++it) {
                    if ((*it).get_domain() == domain) {
                        if (level == CdmwTrace::ALL_VALUES) {
                            (*it).deactivate_all_levels();
                        }
                        else {
                            (*it).deactivate_level(level);
                        }
                    }
                }

                // add the filter (C*, D, mask) to the list
                // if it already exists then it will be moved to the tail
                // of the list
                add_filter(CdmwTrace::ALL_COMPONENT_NAMES,
                           domain,
                           level,
                           false);
            }
        }

        // componentName != CdmwTrace::ALL_COMPONENT_NAMES
        else {
            // deactivate level(s) of any domain of a particular component
            if (domain == CdmwTrace::ALL_DOMAINS) {
                FilterCtr::iterator it = m_filters.begin();
                FilterCtr::iterator it_end = m_filters.end();

                // update the level(s) of the filters of the list that
                // correspond to the specified component name
                for (; it != it_end; ++it) {
                    if ((*it).get_component_name() == componentName) {
                        if (level == CdmwTrace::ALL_VALUES) {
                            (*it).deactivate_all_levels();
                        }
                        else {
                            (*it).deactivate_level(level);
                        }
                    }
                }

                // add the filter (C, D*, mask) to the list
                // if it already exists then it will be moved to the tail
                // of the list
                add_filter(componentName,
                           CdmwTrace::ALL_DOMAINS,
                           level,
                           false);
            }

            // deactivate level(s) of a particular domain of a particular component
            else {
                // add the filter (C, D, mask) to the list
                // if it already exists then it will be moved to the tail
                // of the list
                add_filter(componentName, domain, level, false);
            }
        }
    }
    catch (std::bad_alloc&) {
        CDMW_THROW(OutOfMemoryException);
    }
}

// }}}2

// {{{2 get_trace_filters()

/**
 * @return a sequence of filters whether activated or not.
 *
 * @exception OutOfMemoryException
 */
CdmwTrace::TraceFilterSeq*
FilterMngr::get_trace_filters(void)
    throw(OutOfMemoryException)
{
    // create a sequence of filters
    CdmwTrace::TraceFilterSeq* p_filterSeq = new CdmwTrace::TraceFilterSeq;

    try {
        bitmask mask_filter_any = m_filter_any.get_levels();

        // the most generic filter is considered activated
        // if at least half of the bits in the mask is set (bit = 1)
        bool filter_any_is_activated =
            (mask_filter_any.count() >= (NB_BITS / 2));

        // set sequence length
/*
        unsigned int seq_len = m_filters.size() + 1;
        if (filter_any_is_activated) {
            seq_len += NB_BITS - mask_filter_any.count();
        }
        else {
            seq_len += mask_filter_any.count();
        }
        p_filterSeq->length(seq_len);
*/
        p_filterSeq->length(1);

        // add the most generic filter first
        (*p_filterSeq)[0].the_component_name =
            CORBA::string_dup(CdmwTrace::ALL_COMPONENT_NAMES);
        (*p_filterSeq)[0].the_domain =
            CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
        (*p_filterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
        (*p_filterSeq)[0].activation = filter_any_is_activated;

        // then add the generic filters than are deduced from mask_filter_any
        int seq_idx = 1;
        if ((mask_filter_any.count() != 0) &&
            (mask_filter_any.count() != NB_BITS))
        {
            for (size_t bit = 0; bit < NB_BITS; ++bit) {
                if (filter_any_is_activated && !mask_filter_any.test(bit)) {
                    CORBA::ULong seq_len = p_filterSeq->length() + 1;
                    p_filterSeq->length(seq_len);

                    (*p_filterSeq)[seq_idx].the_component_name =
                        CORBA::string_dup(CdmwTrace::ALL_COMPONENT_NAMES);
                    (*p_filterSeq)[seq_idx].the_domain =
                        CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
                    (*p_filterSeq)[seq_idx].the_value = bit;
                    (*p_filterSeq)[seq_idx].activation = false;

                    seq_idx++;
                }
                else if (!filter_any_is_activated && mask_filter_any.test(bit)) {
                    CORBA::ULong seq_len = p_filterSeq->length() + 1;
                    p_filterSeq->length(seq_len);

                    (*p_filterSeq)[seq_idx].the_component_name =
                        CORBA::string_dup(CdmwTrace::ALL_COMPONENT_NAMES);
                    (*p_filterSeq)[seq_idx].the_domain =
                        CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
                    (*p_filterSeq)[seq_idx].the_value = bit;
                    (*p_filterSeq)[seq_idx].activation = true;

                    seq_idx++;
                }
            }
        }

        // now add the filters from the list (if any)
        FilterCtr::iterator it = m_filters.begin();
        FilterCtr::iterator it_end = m_filters.end();
        for (; it != it_end; ++it) {
            bitmask mask = (*it).get_levels();

            if (mask != mask_filter_any) {
                // the filter is considered activated
                // if at least half of the bits in the mask is set
                bool filter_is_activated = (mask.count() >= (NB_BITS / 2));

                if (filter_is_activated != filter_any_is_activated) {
                    CORBA::ULong seq_len = p_filterSeq->length() + 1;
                    p_filterSeq->length(seq_len);

                    (*p_filterSeq)[seq_idx].the_component_name =
                        CORBA::string_dup((*it).get_component_name().c_str());
                    (*p_filterSeq)[seq_idx].the_domain =
                        CORBA::string_dup((*it).get_domain().c_str());
                    (*p_filterSeq)[seq_idx].the_value = CdmwTrace::ALL_VALUES;
                    (*p_filterSeq)[seq_idx].activation = filter_is_activated;

                    seq_idx++;
                }

                if ((mask.count() != 0) && (mask.count() != NB_BITS)) {
                    for (size_t bit = 0; bit < NB_BITS; ++bit) {
                        if ((mask[bit] != mask_filter_any[bit]) ||
                            ((mask[bit] == mask_filter_any[bit]) &&
                             (filter_is_activated != filter_any_is_activated)))
                        {
                            if (filter_is_activated && !mask.test(bit)) {
                                CORBA::ULong seq_len = p_filterSeq->length() + 1;
                                p_filterSeq->length(seq_len);

                                (*p_filterSeq)[seq_idx].the_component_name =
                                    CORBA::string_dup(
                                        (*it).get_component_name().c_str());
                                (*p_filterSeq)[seq_idx].the_domain =
                                    CORBA::string_dup(
                                        (*it).get_domain().c_str());
                                (*p_filterSeq)[seq_idx].the_value = bit;
                                (*p_filterSeq)[seq_idx].activation = false;

                                seq_idx++;
                            }
                            else if (!filter_is_activated && mask.test(bit)) {
                                CORBA::ULong seq_len = p_filterSeq->length() + 1;
                                p_filterSeq->length(seq_len);

                                (*p_filterSeq)[seq_idx].the_component_name =
                                    CORBA::string_dup(
                                        (*it).get_component_name().c_str());
                                (*p_filterSeq)[seq_idx].the_domain =
                                    CORBA::string_dup(
                                        (*it).get_domain().c_str());
                                (*p_filterSeq)[seq_idx].the_value = bit;
                                (*p_filterSeq)[seq_idx].activation = true;

                                seq_idx++;
                            }
                        }
                    }
                }
            }
        }
    }
    catch (std::out_of_range&) {
        // not possible as we iterate only NB_BITS times
    }
    catch (std::bad_alloc&) {
        CDMW_THROW(OutOfMemoryException);
    }

    return p_filterSeq;
}

// }}}2

// {{{2 is_activated()

/**
 * Check if the specified level is activated.
 *
 * @param domain activate the specified level of this domain.
 * @param level the level to activate.
 *
 * @return true or false.
 *
 * @exception BadParameterException
 * @exception OutOfMemoryException
 */
bool
FilterMngr::is_activated(const std::string& domain,
                         const long level)
    throw(BadParameterException,
          OutOfMemoryException)
{
    // acquire mutex
    CDMW_MUTEX_GUARD(m_filterMngr_mtx);

    return _is_activated(CdmwTrace::ALL_COMPONENT_NAMES, domain, level);
}


/**
 * Check if the specified level is activated.
 *
 * @param componentName component to which the domain is associated.
 * @param domain activate the level for this domain.
 * @param level level to activate.
 *
 * @return true or false.
 *
 * @exception BadParameterException
 * @exception OutOfMemoryException
 */
bool
FilterMngr::is_activated(const std::string& componentName,
                         const std::string& domain,
                         const long level)
    throw(BadParameterException,
          OutOfMemoryException)
{
    // acquire mutex
    CDMW_MUTEX_GUARD(m_filterMngr_mtx);

    return _is_activated(componentName, domain, level);
}


/**
 * Check if the specified level is activated.
 * Not thread-safe.
 *
 * @param componentName component to which the domain is associated.
 * @param domain activate the level for this domain.
 * @param level level to activate.
 *
 * @return true or false.
 *
 * @exception BadParameterException
 * @exception OutOfMemoryException
 */
bool
FilterMngr::_is_activated(const std::string& componentName,
                          const std::string& domain,
                          const long level)
    throw(BadParameterException,
          OutOfMemoryException)
{
    try {
        // first, try to find a filter with the matching component name
        // and domain (the levels are ignored)
        Filter f(componentName, domain);
        FilterCtr::iterator it =
            std::find_if(m_filters.begin(),
                         m_filters.end(),
                         bind2nd(filter_equal_to<Filter, Filter>(), f));

        if (it != m_filters.end()) {
            return (*it).is_activated(level);
        }
        else {
            // we could not find it previously therefore we are going to
            // apply all the filters that matches either the specified
            // componentName or domain from the most recent to the oldest
            bitmask mask;
            bool first = true;
            bool none_applied = true;

            FilterCtr::reverse_iterator it = m_filters.rbegin();
            FilterCtr::reverse_iterator it_end = m_filters.rend();

            for (; it != it_end; ++it) {
                std::string componentName_ = (*it).get_component_name();
                std::string domain_ = (*it).get_domain();

                if ((componentName_ == CdmwTrace::ALL_COMPONENT_NAMES) ||
                    (componentName_ == componentName))
                {
                    if ((domain_ == CdmwTrace::ALL_DOMAINS) ||
                        (domain_ == domain))
                    {
                        if (first) {
                            mask = (*it).get_levels();
                            first = false;
                        }
                        else {
                            mask |= (*it).get_levels();
                        }

                        none_applied = false;
                    }
                }
            }

            return (none_applied ? m_filter_any.is_activated(level)
                                 : mask[level]);
        }
    }
    catch (std::out_of_range&) {
        std::ostringstream oss;
        oss << level << std::ends;
        CDMW_THROW2(BadParameterException, "level", oss.str());
    }
    catch (std::bad_alloc&) {
        CDMW_THROW(OutOfMemoryException);
    }
}

// }}}2

// {{{2 add_filter()

/**
 * Add a new Filter.
 *
 * @param componentName the component to which the domain is associated.
 * @param domain activate the specified level of this domain.
 * @param level the level to activate/deactivate
 * @param activate if true the level will be deactivated
 *
 * @exception OutOfMemoryException
 */
void
FilterMngr::add_filter(const std::string& componentName,
                       const std::string& domain,
                       const long level,
                       bool activate)
    throw(OutOfMemoryException)
{
    try {
        Filter f(componentName, domain);

        // look for the filter
        FilterCtr::iterator it =
            std::find_if(
                m_filters.begin(),
                m_filters.end(),
                bind2nd(filter_equal_to<Filter, Filter>(), f)
            );

        bool is_new_filter = true;

        // found
        if (it != m_filters.end()) {
            // make a copy of it
            f = (*it);
            // remove it from the list
            m_filters.erase(it);

            is_new_filter = false;
        }

        // update the level(s)
        if (level == CdmwTrace::ALL_VALUES) {
            if (activate) {
                f.activate_all_levels();
            }
            else {
                f.deactivate_all_levels();
            }
        }
        else {
            if (is_new_filter) {
                // the initial mask of the new filter is the mast that results
                // from all the filters in the list that can to the specified
                // componentName and domain

                // init the mask with the mask from the most generic filter
                bitmask mask = m_filter_any.get_levels();

                FilterCtr::const_iterator it = m_filters.begin();
                FilterCtr::const_iterator it_end = m_filters.end();

                for (; it != it_end; ++it) {
                    std::string componentName_ = (*it).get_component_name();
                    std::string domain_ = (*it).get_domain();

                    if ((componentName_ == CdmwTrace::ALL_COMPONENT_NAMES) ||
                        (componentName_ == componentName))
                    {
                        if ((domain_ == CdmwTrace::ALL_DOMAINS) ||
                            (domain_ == domain))
                        {
                            mask |= (*it).get_levels();
                        }
                    }
                }

                f.set_levels(mask);
            }

            if (activate) {
                f.activate_level(level);
            }
            else {
                f.deactivate_level(level);
            }
        }

        // append it to the list
        m_filters.push_back(f);
    }
    catch (std::bad_alloc&) {
        CDMW_THROW(OutOfMemoryException);
    }
}

// }}}2


// }}}1


} // End namespace Trace

} // End namespace Cdmw
