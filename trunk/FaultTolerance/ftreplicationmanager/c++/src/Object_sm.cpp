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
using namespace std;


#include "ftreplicationmanager/Object.hpp"
#include "ftreplicationmanager/Object_sm.h"

using namespace statemap;
using namespace std;

namespace Cdmw
{
    namespace FT
    {
        namespace ReplicationManager
        {
            namespace Object
            {
                // Static class declarations.
                StateMap_INITIAL    StateMap::INITIAL("StateMap::INITIAL", 0);
                StateMap_BACKUP    StateMap::BACKUP("StateMap::BACKUP", 1);
                StateMap_PRIMARY    StateMap::PRIMARY("StateMap::PRIMARY", 2);
                StateMap_FAILED    StateMap::FAILED("StateMap::FAILED", 3);
                StateMap_ERROR    StateMap::ERROR("StateMap::ERROR", 4);

                const int ObjectContext::MIN_INDEX = 0;
                const int ObjectContext::MAX_INDEX = 4;
                ObjectState* ObjectContext::_States[] = 
{
                    &StateMap::INITIAL,
                    &StateMap::BACKUP,
                    &StateMap::PRIMARY,
                    &StateMap::FAILED,
                    &StateMap::ERROR
                };

                ObjectState& ObjectContext::valueOf(int id)
                {
                    if (id < MIN_INDEX || id > MAX_INDEX)
                    {
                        throw (
                            IndexOutOfBoundsException(
                                id, MIN_INDEX, MAX_INDEX));
                    }

                    return (static_cast<ObjectState&>(*(_States[id])));
                }

                void ObjectState::Unknown(ObjectContext& context)
                {
                    Default(context);
                    return;
                }

                void ObjectState::ev_failure(ObjectContext& context)
                {
                    Default(context);
                    return;
                }

                void ObjectState::ev_init_backup_done(ObjectContext& context)
                {
                    Default(context);
                    return;
                }

                void ObjectState::ev_init_primary_done(ObjectContext& context)
                {
                    Default(context);
                    return;
                }

                void ObjectState::ev_set_primary(ObjectContext& context)
                {
                    Default(context);
                    return;
                }

                void ObjectState::Default(ObjectContext& context)
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

                void StateMap_Default::Unknown(ObjectContext& context)
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

                void StateMap_INITIAL::Entry(ObjectContext& context)
{
                    Object& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::MemberState_INITIAL);
                    return;
                }

                void StateMap_INITIAL::ev_failure(ObjectContext& context)
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

                void StateMap_INITIAL::ev_init_backup_done(ObjectContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_init_backup_done()"
                            << endl;
                    }

    (context.getState()).Exit(context);
    context.setState(StateMap::BACKUP);
    (context.getState()).Entry(context);

                    return;
                }

                void StateMap_INITIAL::ev_init_primary_done(ObjectContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_init_primary_done()"
                            << endl;
                    }

    (context.getState()).Exit(context);
    context.setState(StateMap::PRIMARY);
    (context.getState()).Entry(context);

                    return;
                }

                void StateMap_BACKUP::Entry(ObjectContext& context)
{
                    Object& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::MemberState_BACKUP);
                    return;
                }

                void StateMap_BACKUP::ev_failure(ObjectContext& context)
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

                void StateMap_BACKUP::ev_set_primary(ObjectContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_set_primary()"
                            << endl;
                    }

    (context.getState()).Exit(context);
    context.setState(StateMap::PRIMARY);
    (context.getState()).Entry(context);

                    return;
                }

                void StateMap_PRIMARY::Entry(ObjectContext& context)
{
                    Object& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::MemberState_PRIMARY);
                    return;
                }

                void StateMap_PRIMARY::ev_failure(ObjectContext& context)
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

                void StateMap_FAILED::Entry(ObjectContext& context)
{
                    Object& ctxt = context.getOwner();

                    ctxt.fsm_state(::CdmwFT::MemberState_FAILED);
                    return;
                }

                void StateMap_ERROR::ev_failure(ObjectContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_failure()"
                            << endl;
                    }

                    return;
                }

                void StateMap_ERROR::ev_init_backup_done(ObjectContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_init_backup_done()"
                            << endl;
                    }

                    return;
                }

                void StateMap_ERROR::ev_init_primary_done(ObjectContext& context)
                {

                    if (context.getDebugFlag() == true)
                    {
                        ostream& str = context.getDebugStream();

                        str << "TRANSITION   : StateMap ev_init_primary_done()"
                            << endl;
                    }

                    return;
                }

                void StateMap_ERROR::ev_set_primary(ObjectContext& context)
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
