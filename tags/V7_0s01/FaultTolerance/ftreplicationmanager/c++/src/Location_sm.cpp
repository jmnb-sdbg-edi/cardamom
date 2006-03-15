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

#include <stdlib.h>
#include <iostream>
#include "ftreplicationmanager/ObjectGroup.hpp"
#include "ftreplicationmanager/Object.hpp"
#include "ftreplicationmanager/CdmwFTLocationInfo.stub.hpp"
using namespace std;
using namespace Cdmw::FT::ReplicationManager;


#include "ftreplicationmanager/Location.hpp"
#include "ftreplicationmanager/Location_sm.h"

using namespace statemap;
using namespace std;

namespace Cdmw
{
    namespace FT
    {
        namespace ReplicationManager
        {
            namespace Location
            {
                // Static class declarations.
                StateMap_INITIAL    StateMap::INITIAL("StateMap::INITIAL", 0);
                StateMap_INSERTING_PRIMARY    StateMap::INSERTING_PRIMARY("StateMap::INSERTING_PRIMARY", 1);
                StateMap_PRIMARY    StateMap::PRIMARY("StateMap::PRIMARY", 2);
                StateMap_INSERTING_BACKUP    StateMap::INSERTING_BACKUP("StateMap::INSERTING_BACKUP", 3);
                StateMap_BACKUP    StateMap::BACKUP("StateMap::BACKUP", 4);
                StateMap_FAILED    StateMap::FAILED("StateMap::FAILED", 5);
                StateMap_ERROR    StateMap::ERROR("StateMap::ERROR", 6);

                const int LocationContext::MIN_INDEX = 0;
                const int LocationContext::MAX_INDEX = 6;
                LocationState* LocationContext::_States[] = 
{
                    &StateMap::INITIAL,
                    &StateMap::INSERTING_PRIMARY,
                    &StateMap::PRIMARY,
                    &StateMap::INSERTING_BACKUP,
                    &StateMap::BACKUP,
                    &StateMap::FAILED,
                    &StateMap::ERROR
                };

                LocationState& LocationContext::valueOf(int id)
                {
                    if (id < MIN_INDEX || id > MAX_INDEX)
                    {
                        throw (
                            IndexOutOfBoundsException(
                                id, MIN_INDEX, MAX_INDEX));
                    }

                    return (static_cast<LocationState&>(*(_States[id])));
                }

                void LocationState::Unknown(LocationContext& context)
                {
                    Default(context);
                    return;
                }

                void LocationState::ev_add_member(LocationContext& context)
                {
                    Default(context);
                    return;
                }

                void LocationState::ev_add_member(LocationContext& context, ObjectGroup::ObjectGroup& grpctxt, Object::Object& objctxt, CORBA::Object_ptr member, const ::FT::Location& primary_loc)
                {
                    Default(context);
                    return;
                }

                void LocationState::ev_create_object(LocationContext& context)
                {
                    Default(context);
                    return;
                }

                void LocationState::ev_failure(LocationContext& context)
                {
                    Default(context);
                    return;
                }

                void LocationState::ev_initial_backup(LocationContext& context)
                {
                    Default(context);
                    return;
                }

                void LocationState::ev_set_backup(LocationContext& context)
                {
                    Default(context);
                    return;
                }

                void LocationState::ev_set_primary(LocationContext& context)
                {
                    Default(context);
                    return;
                }

                void LocationState::Default(LocationContext& context)
                {
                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : Default"
                            << endl;
                    }

                    throw (
                        TransitionUndefinedException(
                            context.getState().getName(),
                            context.getTransition()));

                    return;
                }

                void StateMap_Default::Unknown(LocationContext& context)
                {
                    bool loopbackFlag = false;

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap Unknown()"
                            << endl;
                    }

    if (strcmp(context.getState().getName(), 
               StateMap::ERROR.getName())
           == 0)
    {
        loopbackFlag = true;
    }

    if (loopbackFlag == false)
    {
        (context.getState()).Exit(context);
    }

    context.setState(StateMap::ERROR);

        if (loopbackFlag == false)
        {
        (context.getState()).Entry(context);
    }

                    return;
                }

                void StateMap_INITIAL::Entry(LocationContext& context)
{
                    Location& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::LocationState_INITIAL);
                    return;
                }

                void StateMap_INITIAL::ev_add_member(LocationContext& context, ObjectGroup::ObjectGroup& grpctxt, Object::Object& objctxt, CORBA::Object_ptr member, const ::FT::Location& primary_loc)
                {
                    Location& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_add_member(ObjectGroup::ObjectGroup& grpctxtObject::Object& objctxtCORBA::Object_ptr memberconst ::FT::Location& primary_loc)"
                            << endl;
                    }

    if (ctxt.members_count() == 0 && ctxt.exp_add_member() == 0)
    {
        (context.getState()).Exit(context);
        // No actions.
        context.setState(StateMap::FAILED);
        (context.getState()).Entry(context);
    }
    else if (!ctxt.backup() && ctxt.members_count() == 0 && ctxt.exp_add_member() == 1)
    {
        (context.getState()).Exit(context);
        context.clearState();
        try
        {
            ctxt.members_count(1);
            ctxt.exp_add_member(0);
            ctxt.set_peers_to_initial_backup();
            ctxt.add_primary(grpctxt, objctxt, member, primary_loc);
            ctxt.update_primary(primary_loc);
            context.setState(StateMap::PRIMARY);
        }
        catch (...)
        {
            context.setState(StateMap::PRIMARY);
            throw;
        }
        (context.getState()).Entry(context);
    }
    else if (!ctxt.backup() && ctxt.members_count() == 0 && ctxt.exp_add_member() > 1)
    {
        (context.getState()).Exit(context);
        context.clearState();
        try
        {
            ctxt.inc_members_count();
            ctxt.dec_exp_add_member();
            ctxt.set_peers_to_initial_backup();
            ctxt.add_primary(grpctxt, objctxt, member, primary_loc);
            context.setState(StateMap::INSERTING_PRIMARY);
        }
        catch (...)
        {
            context.setState(StateMap::INSERTING_PRIMARY);
            throw;
        }
        (context.getState()).Entry(context);
    }
    else if (ctxt.backup() && ctxt.members_count() == 0 && ctxt.exp_add_member() == 1)
    {
        (context.getState()).Exit(context);
        context.clearState();
        try
        {
            ctxt.members_count(1);
            ctxt.exp_add_member(0);
            ctxt.add_backup(grpctxt, objctxt, member, primary_loc);
            context.setState(StateMap::BACKUP);
        }
        catch (...)
        {
            context.setState(StateMap::BACKUP);
            throw;
        }
        (context.getState()).Entry(context);
    }
    else if (ctxt.backup() && ctxt.members_count() == 0 && ctxt.exp_add_member() > 1)
    {
        (context.getState()).Exit(context);
        context.clearState();
        try
        {
            ctxt.inc_members_count();
            ctxt.dec_exp_add_member();
            ctxt.add_backup(grpctxt, objctxt, member, primary_loc);
            context.setState(StateMap::INSERTING_BACKUP);
        }
        catch (...)
        {
            context.setState(StateMap::INSERTING_BACKUP);
            throw;
        }
        (context.getState()).Entry(context);
    }                    else
                    {
                         StateMap_Default::ev_add_member(context, grpctxt, objctxt, member, primary_loc);
                    }

                    return;
                }

                void StateMap_INITIAL::ev_create_object(LocationContext& context)
                {
                    Location& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_create_object()"
                            << endl;
                    }

    LocationState& EndStateName = context.getState();

    context.clearState();
    try
    {
        ctxt.inc_exp_add_member();
        context.setState(EndStateName);
    }
    catch (...)
    {
        context.setState(EndStateName);
        throw;
    }

                    return;
                }

                void StateMap_INITIAL::ev_failure(LocationContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_failure()"
                            << endl;
                    }

    (context.getState()).Exit(context);
    context.setState(StateMap::FAILED);
    (context.getState()).Entry(context);

                    return;
                }

                void StateMap_INITIAL::ev_set_backup(LocationContext& context)
                {
                    Location& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_set_backup()"
                            << endl;
                    }

    LocationState& EndStateName = context.getState();

    context.clearState();
    try
    {
        ctxt.backup(true);
        context.setState(EndStateName);
    }
    catch (...)
    {
        context.setState(EndStateName);
        throw;
    }

                    return;
                }

                void StateMap_INSERTING_PRIMARY::Entry(LocationContext& context)
{
                    Location& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::LocationState_INSERTING_PRIMARY);
                    return;
                }

                void StateMap_INSERTING_PRIMARY::ev_add_member(LocationContext& context, ObjectGroup::ObjectGroup& grpctxt, Object::Object& objctxt, CORBA::Object_ptr member, const ::FT::Location& primary_loc)
                {
                    Location& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_add_member(ObjectGroup::ObjectGroup& grpctxtObject::Object& objctxtCORBA::Object_ptr memberconst ::FT::Location& primary_loc)"
                            << endl;
                    }

    if (ctxt.exp_add_member() == 1)
    {
        (context.getState()).Exit(context);
        context.clearState();
        try
        {
            ctxt.inc_members_count();
            ctxt.exp_add_member(0);
            ctxt.add_primary(grpctxt, objctxt, member, primary_loc);
            ctxt.update_primary(primary_loc);
            context.setState(StateMap::PRIMARY);
        }
        catch (...)
        {
            context.setState(StateMap::PRIMARY);
            throw;
        }
        (context.getState()).Entry(context);
    }
    else if (ctxt.exp_add_member() > 1)
    {
        LocationState& EndStateName = context.getState();

        context.clearState();
        try
        {
            ctxt.inc_members_count();
            ctxt.dec_exp_add_member();
            ctxt.add_primary(grpctxt, objctxt, member, primary_loc);
            context.setState(EndStateName);
        }
        catch (...)
        {
            context.setState(EndStateName);
            throw;
        }
    }                    else
                    {
                         StateMap_Default::ev_add_member(context, grpctxt, objctxt, member, primary_loc);
                    }

                    return;
                }

                void StateMap_INSERTING_PRIMARY::ev_create_object(LocationContext& context)
                {
                    Location& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_create_object()"
                            << endl;
                    }

    LocationState& EndStateName = context.getState();

    context.clearState();
    try
    {
        ctxt.inc_exp_add_member();
        context.setState(EndStateName);
    }
    catch (...)
    {
        context.setState(EndStateName);
        throw;
    }

                    return;
                }

                void StateMap_INSERTING_PRIMARY::ev_failure(LocationContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_failure()"
                            << endl;
                    }

    (context.getState()).Exit(context);
    context.setState(StateMap::FAILED);
    (context.getState()).Entry(context);

                    return;
                }

                void StateMap_PRIMARY::Entry(LocationContext& context)
{
                    Location& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::LocationState_PRIMARY);
                    return;
                }

                void StateMap_PRIMARY::ev_failure(LocationContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_failure()"
                            << endl;
                    }

    (context.getState()).Exit(context);
    context.setState(StateMap::FAILED);
    (context.getState()).Entry(context);

                    return;
                }

                void StateMap_PRIMARY::ev_set_backup(LocationContext& context)
                {
                    Location& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_set_backup()"
                            << endl;
                    }

    (context.getState()).Exit(context);
    context.clearState();
    try
    {
        ctxt.set_backup();
        context.setState(StateMap::BACKUP);
    }
    catch (...)
    {
        context.setState(StateMap::BACKUP);
        throw;
    }
    (context.getState()).Entry(context);

                    return;
                }

                void StateMap_INSERTING_BACKUP::Entry(LocationContext& context)
{
                    Location& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::LocationState_INSERTING_BACKUP);
                    return;
                }

                void StateMap_INSERTING_BACKUP::ev_add_member(LocationContext& context, ObjectGroup::ObjectGroup& grpctxt, Object::Object& objctxt, CORBA::Object_ptr member, const ::FT::Location& primary_loc)
                {
                    Location& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_add_member(ObjectGroup::ObjectGroup& grpctxtObject::Object& objctxtCORBA::Object_ptr memberconst ::FT::Location& primary_loc)"
                            << endl;
                    }

    if (ctxt.exp_add_member() == 1)
    {
        (context.getState()).Exit(context);
        context.clearState();
        try
        {
            ctxt.inc_members_count();
            ctxt.exp_add_member(0);
            ctxt.add_backup(grpctxt, objctxt, member, primary_loc);
            context.setState(StateMap::BACKUP);
        }
        catch (...)
        {
            context.setState(StateMap::BACKUP);
            throw;
        }
        (context.getState()).Entry(context);
    }
    else if (ctxt.exp_add_member() > 1)
    {
        LocationState& EndStateName = context.getState();

        context.clearState();
        try
        {
            ctxt.inc_members_count();
            ctxt.dec_exp_add_member();
            ctxt.add_backup(grpctxt, objctxt, member, primary_loc);
            context.setState(EndStateName);
        }
        catch (...)
        {
            context.setState(EndStateName);
            throw;
        }
    }                    else
                    {
                         StateMap_Default::ev_add_member(context, grpctxt, objctxt, member, primary_loc);
                    }

                    return;
                }

                void StateMap_INSERTING_BACKUP::ev_create_object(LocationContext& context)
                {
                    Location& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_create_object()"
                            << endl;
                    }

    LocationState& EndStateName = context.getState();

    context.clearState();
    try
    {
        ctxt.inc_exp_add_member();
        context.setState(EndStateName);
    }
    catch (...)
    {
        context.setState(EndStateName);
        throw;
    }

                    return;
                }

                void StateMap_INSERTING_BACKUP::ev_failure(LocationContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_failure()"
                            << endl;
                    }

    (context.getState()).Exit(context);
    context.setState(StateMap::FAILED);
    (context.getState()).Entry(context);

                    return;
                }

                void StateMap_BACKUP::Entry(LocationContext& context)
{
                    Location& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::LocationState_BACKUP);
                    return;
                }

                void StateMap_BACKUP::ev_failure(LocationContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_failure()"
                            << endl;
                    }

    (context.getState()).Exit(context);
    context.setState(StateMap::FAILED);
    (context.getState()).Entry(context);

                    return;
                }

                void StateMap_BACKUP::ev_set_primary(LocationContext& context)
                {
                    Location& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_set_primary()"
                            << endl;
                    }

    (context.getState()).Exit(context);
    context.clearState();
    try
    {
        ctxt.set_primary();
        context.setState(StateMap::PRIMARY);
    }
    catch (...)
    {
        context.setState(StateMap::PRIMARY);
        throw;
    }
    (context.getState()).Entry(context);

                    return;
                }

                void StateMap_FAILED::Entry(LocationContext& context)
{
                    Location& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::LocationState_FAILED);
                    return;
                }

                void StateMap_ERROR::ev_add_member(LocationContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_add_member()"
                            << endl;
                    }

                    return;
                }

                void StateMap_ERROR::ev_create_object(LocationContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_create_object()"
                            << endl;
                    }

                    return;
                }

                void StateMap_ERROR::ev_failure(LocationContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_failure()"
                            << endl;
                    }

                    return;
                }

                void StateMap_ERROR::ev_initial_backup(LocationContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_initial_backup()"
                            << endl;
                    }

                    return;
                }

                void StateMap_ERROR::ev_set_backup(LocationContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_set_backup()"
                            << endl;
                    }

                    return;
                }

                void StateMap_ERROR::ev_set_primary(LocationContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_set_primary()"
                            << endl;
                    }

                    return;
                }
            }
        }
    }
}
