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
#include "ftreplicationmanager/CdmwFTObjectGroupData.stub.hpp"
using namespace std;


#include "ftreplicationmanager/ObjectGroup.hpp"
#include "ftreplicationmanager/ObjectGroup_sm.h"

using namespace statemap;
using namespace std;

namespace Cdmw
{
    namespace FT
    {
        namespace ReplicationManager
        {
            namespace ObjectGroup
            {
                // Static class declarations.
                StateMap_INITIAL    StateMap::INITIAL("StateMap::INITIAL", 0);
                StateMap_COMPLETE_FAILURE    StateMap::COMPLETE_FAILURE("StateMap::COMPLETE_FAILURE", 1);
                StateMap_LOST_REDUNDANCY    StateMap::LOST_REDUNDANCY("StateMap::LOST_REDUNDANCY", 2);
                StateMap_DEGRADED_REDUNDANCY    StateMap::DEGRADED_REDUNDANCY("StateMap::DEGRADED_REDUNDANCY", 3);
                StateMap_FULLY_REDUNDANT    StateMap::FULLY_REDUNDANT("StateMap::FULLY_REDUNDANT", 4);
                StateMap_FAILED    StateMap::FAILED("StateMap::FAILED", 5);
                StateMap_ERROR    StateMap::ERROR("StateMap::ERROR", 6);

                const int ObjectGroupContext::MIN_INDEX = 0;
                const int ObjectGroupContext::MAX_INDEX = 6;
                ObjectGroupState* ObjectGroupContext::_States[] = 
{
                    &StateMap::INITIAL,
                    &StateMap::COMPLETE_FAILURE,
                    &StateMap::LOST_REDUNDANCY,
                    &StateMap::DEGRADED_REDUNDANCY,
                    &StateMap::FULLY_REDUNDANT,
                    &StateMap::FAILED,
                    &StateMap::ERROR
                };

                ObjectGroupState& ObjectGroupContext::valueOf(int id)
                {
                    if (id < MIN_INDEX || id > MAX_INDEX)
                    {
                        throw (
                            IndexOutOfBoundsException(
                                id, MIN_INDEX, MAX_INDEX));
                    }

                    return (static_cast<ObjectGroupState&>(*(_States[id])));
                }

                void ObjectGroupState::Unknown(ObjectGroupContext& context)
                {
                    Default(context);
                    return;
                }

                void ObjectGroupState::ev_backup_ready(ObjectGroupContext& context, Object::Object& objctxt)
                {
                    Default(context);
                    return;
                }

                void ObjectGroupState::ev_member_removed(ObjectGroupContext& context, Object::Object& objctxt)
                {
                    Default(context);
                    return;
                }

                void ObjectGroupState::ev_primary_ready(ObjectGroupContext& context, Object::Object& objctxt)
                {
                    Default(context);
                    return;
                }

                void ObjectGroupState::Default(ObjectGroupContext& context)
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

                void StateMap_Default::Unknown(ObjectGroupContext& context)
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

                void StateMap_INITIAL::Entry(ObjectGroupContext& context)
{
                    ObjectGroup& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::ObjectGroupState_INITIAL);
                    return;
                }

                void StateMap_INITIAL::ev_primary_ready(ObjectGroupContext& context, Object::Object& objctxt)
                {
                    ObjectGroup& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_primary_ready(Object::Object& objctxt)"
                            << endl;
                    }

    (context.getState()).Exit(context);
    context.clearState();
    try
    {
        ctxt.members_count(1);
        ctxt.primary_ready(objctxt);
        context.setState(StateMap::LOST_REDUNDANCY);
    }
    catch (...)
    {
        context.setState(StateMap::LOST_REDUNDANCY);
        throw;
    }
    (context.getState()).Entry(context);

                    return;
                }

                void StateMap_COMPLETE_FAILURE::Entry(ObjectGroupContext& context)
{
                    ObjectGroup& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::ObjectGroupState_FAILED);
                    ctxt.notify_observers();
                    return;
                }

                void StateMap_COMPLETE_FAILURE::ev_primary_ready(ObjectGroupContext& context, Object::Object& objctxt)
                {
                    ObjectGroup& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_primary_ready(Object::Object& objctxt)"
                            << endl;
                    }

    (context.getState()).Exit(context);
    context.clearState();
    try
    {
        ctxt.members_count(1);
        ctxt.primary_ready(objctxt);
        context.setState(StateMap::LOST_REDUNDANCY);
    }
    catch (...)
    {
        context.setState(StateMap::LOST_REDUNDANCY);
        throw;
    }
    (context.getState()).Entry(context);

                    return;
                }

                void StateMap_LOST_REDUNDANCY::Entry(ObjectGroupContext& context)
{
                    ObjectGroup& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::ObjectGroupState_LOST_REDUNDANCY);
                    ctxt.notify_observers();
                    return;
                }

                void StateMap_LOST_REDUNDANCY::ev_backup_ready(ObjectGroupContext& context, Object::Object& objctxt)
                {
                    ObjectGroup& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_backup_ready(Object::Object& objctxt)"
                            << endl;
                    }

    if ( (ctxt.members_count() + 1) >= ctxt.min_members_count() )
    {
        (context.getState()).Exit(context);
        context.clearState();
        try
        {
            ctxt.inc_members_count();
            ctxt.backup_ready(objctxt);
            context.setState(StateMap::FULLY_REDUNDANT);
        }
        catch (...)
        {
            context.setState(StateMap::FULLY_REDUNDANT);
            throw;
        }
        (context.getState()).Entry(context);
    }
    else
    {
        (context.getState()).Exit(context);
        context.clearState();
        try
        {
            ctxt.inc_members_count();
            ctxt.backup_ready(objctxt);
            context.setState(StateMap::DEGRADED_REDUNDANCY);
        }
        catch (...)
        {
            context.setState(StateMap::DEGRADED_REDUNDANCY);
            throw;
        }
        (context.getState()).Entry(context);
    }

                    return;
                }

                void StateMap_LOST_REDUNDANCY::ev_member_removed(ObjectGroupContext& context, Object::Object& objctxt)
                {
                    ObjectGroup& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_member_removed(Object::Object& objctxt)"
                            << endl;
                    }

    if ( ctxt.members_count() == 1 )
    {
        (context.getState()).Exit(context);
        context.clearState();
        try
        {
            ctxt.members_count(0);
            ctxt.member_removed(objctxt);
            context.setState(StateMap::COMPLETE_FAILURE);
        }
        catch (...)
        {
            context.setState(StateMap::COMPLETE_FAILURE);
            throw;
        }
        (context.getState()).Entry(context);
                    }
                    else
                    {
                         StateMap_Default::ev_member_removed(context, objctxt);
                    }

                    return;
                }

                void StateMap_DEGRADED_REDUNDANCY::Entry(ObjectGroupContext& context)
{
                    ObjectGroup& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::ObjectGroupState_DEGRADED_REDUNDANCY);
                    ctxt.notify_observers();
                    return;
                }

                void StateMap_DEGRADED_REDUNDANCY::ev_backup_ready(ObjectGroupContext& context, Object::Object& objctxt)
                {
                    ObjectGroup& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_backup_ready(Object::Object& objctxt)"
                            << endl;
                    }

    if ( (ctxt.members_count() + 1) < ctxt.min_members_count() )
    {
        ObjectGroupState& EndStateName = context.getState();

        context.clearState();
        try
        {
            ctxt.inc_members_count();
            ctxt.backup_ready(objctxt);
            context.setState(EndStateName);
        }
        catch (...)
        {
            context.setState(EndStateName);
            throw;
        }
    }
    else if ( (ctxt.members_count() + 1) >= ctxt.min_members_count() )
    {
        (context.getState()).Exit(context);
        context.clearState();
        try
        {
            ctxt.inc_members_count();
            ctxt.backup_ready(objctxt);
            context.setState(StateMap::FULLY_REDUNDANT);
        }
        catch (...)
        {
            context.setState(StateMap::FULLY_REDUNDANT);
            throw;
        }
        (context.getState()).Entry(context);
    }                    else
                    {
                         StateMap_Default::ev_backup_ready(context, objctxt);
                    }

                    return;
                }

                void StateMap_DEGRADED_REDUNDANCY::ev_member_removed(ObjectGroupContext& context, Object::Object& objctxt)
                {
                    ObjectGroup& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_member_removed(Object::Object& objctxt)"
                            << endl;
                    }

    if ( ctxt.members_count() == 2 )
    {
        (context.getState()).Exit(context);
        context.clearState();
        try
        {
            ctxt.dec_members_count();
            ctxt.member_removed(objctxt);
            context.setState(StateMap::LOST_REDUNDANCY);
        }
        catch (...)
        {
            context.setState(StateMap::LOST_REDUNDANCY);
            throw;
        }
        (context.getState()).Entry(context);
    }
    else if ( ctxt.members_count() < ctxt.min_members_count() )
    {
        ObjectGroupState& EndStateName = context.getState();

        context.clearState();
        try
        {
            ctxt.dec_members_count();
            ctxt.member_removed(objctxt);
            context.setState(EndStateName);
        }
        catch (...)
        {
            context.setState(EndStateName);
            throw;
        }
    }                    else
                    {
                         StateMap_Default::ev_member_removed(context, objctxt);
                    }

                    return;
                }

                void StateMap_FULLY_REDUNDANT::Entry(ObjectGroupContext& context)
{
                    ObjectGroup& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::ObjectGroupState_FULLY_REDUNDANT);
                    ctxt.notify_observers();
                    return;
                }

                void StateMap_FULLY_REDUNDANT::ev_backup_ready(ObjectGroupContext& context, Object::Object& objctxt)
                {
                    ObjectGroup& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_backup_ready(Object::Object& objctxt)"
                            << endl;
                    }

    ObjectGroupState& EndStateName = context.getState();

    context.clearState();
    try
    {
        ctxt.inc_members_count();
        ctxt.backup_ready(objctxt);
        context.setState(EndStateName);
    }
    catch (...)
    {
        context.setState(EndStateName);
        throw;
    }

                    return;
                }

                void StateMap_FULLY_REDUNDANT::ev_member_removed(ObjectGroupContext& context, Object::Object& objctxt)
                {
                    ObjectGroup& ctxt = context.getOwner();

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_member_removed(Object::Object& objctxt)"
                            << endl;
                    }

    if ( ctxt.members_count() > ctxt.min_members_count() )
    {
        ObjectGroupState& EndStateName = context.getState();

        context.clearState();
        try
        {
            ctxt.dec_members_count();
            ctxt.member_removed(objctxt);
            context.setState(EndStateName);
        }
        catch (...)
        {
            context.setState(EndStateName);
            throw;
        }
    }
    else if ( ctxt.members_count() == 2 )
    {
        (context.getState()).Exit(context);
        context.clearState();
        try
        {
            ctxt.dec_members_count();
            ctxt.member_removed(objctxt);
            context.setState(StateMap::LOST_REDUNDANCY);
        }
        catch (...)
        {
            context.setState(StateMap::LOST_REDUNDANCY);
            throw;
        }
        (context.getState()).Entry(context);
    }
    else if ( ctxt.members_count() == ctxt.min_members_count() )
    {
        (context.getState()).Exit(context);
        context.clearState();
        try
        {
            ctxt.dec_members_count();
            ctxt.member_removed(objctxt);
            context.setState(StateMap::DEGRADED_REDUNDANCY);
        }
        catch (...)
        {
            context.setState(StateMap::DEGRADED_REDUNDANCY);
            throw;
        }
        (context.getState()).Entry(context);
    }                    else
                    {
                         StateMap_Default::ev_member_removed(context, objctxt);
                    }

                    return;
                }

                void StateMap_FAILED::Entry(ObjectGroupContext& context)
{
                    ObjectGroup& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::ObjectGroupState_FAILED);
                    ctxt.notify_observers();
                    return;
                }
            }
        }
    }
}
