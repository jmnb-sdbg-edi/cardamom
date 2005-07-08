/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

#include <Time/clockservice/ClockCatalog_impl.hpp>

///////////////////////////////////////////////////////////////////////////////

Cdmw::clock::ClockCatalog_impl::ClockCatalog_impl(void)
{
    // No-Op
}

///////////////////////////////////////////////////////////////////////////////

Cdmw::clock::ClockCatalog_impl::~ClockCatalog_impl (void)
{
    // No-Op
}

///////////////////////////////////////////////////////////////////////////////

::CosClockService::ClockCatalog::ClockEntry*
Cdmw::clock::ClockCatalog_impl::get_entry(const char * with_name)
    throw (CORBA::SystemException,
           ::CosClockService::ClockCatalog::UnknownEntry)
{
    ClockMapT_CI cmIter;
    cmIter = catalog_.find(with_name);
    if (cmIter != catalog_.end()) 
    {
        ::CosClockService::ClockCatalog::ClockEntry* entry =
              new ::CosClockService::ClockCatalog::ClockEntry();

        entry->name = CORBA::string_dup(cmIter->first.c_str());
        entry->subject = cmIter->second;
        return entry;
    }
    else throw ::CosClockService::ClockCatalog::UnknownEntry();
}

///////////////////////////////////////////////////////////////////////////////

::CosClockService::ClockCatalog::ClockEntries*
Cdmw::clock::ClockCatalog_impl::available_entries()
    throw (CORBA::SystemException)
{
    ClockMapT::size_type clockMapLength = catalog_.size();
    ::CosClockService::ClockCatalog::ClockEntries*
     clockEntries = new ::CosClockService::ClockCatalog::ClockEntries;

    if (clockMapLength) 
    {
        clockEntries->length(clockMapLength);
        ClockMapT_CI cmIter = catalog_.begin();
        for (ClockMapT::size_type i = 0; i < clockMapLength; ++i,++cmIter) 
        {
            (*clockEntries)[i].name = CORBA::string_dup(cmIter->first.c_str());
            (*clockEntries)[i].subject = cmIter->second;
        }
    }
  return clockEntries;
}

///////////////////////////////////////////////////////////////////////////////

void 
Cdmw::clock::ClockCatalog_impl::_cxx_register (
        const CosClockService::ClockCatalog::ClockEntry & entry)
    throw (CORBA::SystemException)
{
    catalog_[std::string(entry.name)] = entry.subject;
}

///////////////////////////////////////////////////////////////////////////////

void 
Cdmw::clock::ClockCatalog_impl::delete_entry (const char * with_name)
    throw (CORBA::SystemException, ::CosClockService::ClockCatalog::UnknownEntry)
{
    ClockMapT_I cmIter;

    cmIter = catalog_.find(with_name);
    if (cmIter != catalog_.end())
        catalog_.erase(cmIter);
    else
        throw ::CosClockService::ClockCatalog::UnknownEntry();
}

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
