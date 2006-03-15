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
#ifndef _H_OBJECTGROUP_SM
#define _H_OBJECTGROUP_SM

#define SMC_USES_IOSTREAMS

#include "ftreplicationmanager/statemap.h"
#include "ftreplicationmanager/Object.hpp"


namespace Cdmw
{
    namespace FT
    {
        namespace ReplicationManager
        {
            namespace ObjectGroup
            {
                // Forward declarations.
                class StateMap;
                class StateMap_INITIAL;
                class StateMap_COMPLETE_FAILURE;
                class StateMap_LOST_REDUNDANCY;
                class StateMap_DEGRADED_REDUNDANCY;
                class StateMap_FULLY_REDUNDANT;
                class StateMap_FAILED;
                class StateMap_ERROR;
                class StateMap_Default;
                class ObjectGroupState;
                class ObjectGroupContext;
                class ObjectGroup;

                class ObjectGroupState : public statemap::State
                {
                public:
                    ObjectGroupState(const char *name, int id)
                    : statemap::State(name, id)
                    {};

                    virtual void Entry(ObjectGroupContext&) {};
                    virtual void Exit(ObjectGroupContext&) {};

                    virtual void Unknown(ObjectGroupContext& context);
                    virtual void ev_backup_ready(ObjectGroupContext& context, Object::Object& objctxt);
                    virtual void ev_member_removed(ObjectGroupContext& context, Object::Object& objctxt);
                    virtual void ev_primary_ready(ObjectGroupContext& context, Object::Object& objctxt);

                protected:
                    virtual void Default(ObjectGroupContext& context);
                };

                class StateMap
                {
                public:
                    static StateMap_INITIAL    INITIAL;
                    static StateMap_COMPLETE_FAILURE    COMPLETE_FAILURE;
                    static StateMap_LOST_REDUNDANCY    LOST_REDUNDANCY;
                    static StateMap_DEGRADED_REDUNDANCY    DEGRADED_REDUNDANCY;
                    static StateMap_FULLY_REDUNDANT    FULLY_REDUNDANT;
                    static StateMap_FAILED    FAILED;
                    static StateMap_ERROR    ERROR;
                };

                class StateMap_Default : public ObjectGroupState
                {
                public:
                    StateMap_Default(const char *name, int id)
                    : ObjectGroupState(name, id)
                    {};

                    virtual void Unknown(ObjectGroupContext& context);
                };

                class StateMap_INITIAL : public StateMap_Default
                {
                public:
                    StateMap_INITIAL(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(ObjectGroupContext&);
                    void ev_primary_ready(ObjectGroupContext& context, Object::Object& objctxt);
                };

                class StateMap_COMPLETE_FAILURE : public StateMap_Default
                {
                public:
                    StateMap_COMPLETE_FAILURE(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(ObjectGroupContext&);
                    void ev_primary_ready(ObjectGroupContext& context, Object::Object& objctxt);
                };

                class StateMap_LOST_REDUNDANCY : public StateMap_Default
                {
                public:
                    StateMap_LOST_REDUNDANCY(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(ObjectGroupContext&);
                    void ev_backup_ready(ObjectGroupContext& context, Object::Object& objctxt);
                    void ev_member_removed(ObjectGroupContext& context, Object::Object& objctxt);
                };

                class StateMap_DEGRADED_REDUNDANCY : public StateMap_Default
                {
                public:
                    StateMap_DEGRADED_REDUNDANCY(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(ObjectGroupContext&);
                    void ev_backup_ready(ObjectGroupContext& context, Object::Object& objctxt);
                    void ev_member_removed(ObjectGroupContext& context, Object::Object& objctxt);
                };

                class StateMap_FULLY_REDUNDANT : public StateMap_Default
                {
                public:
                    StateMap_FULLY_REDUNDANT(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(ObjectGroupContext&);
                    void ev_backup_ready(ObjectGroupContext& context, Object::Object& objctxt);
                    void ev_member_removed(ObjectGroupContext& context, Object::Object& objctxt);
                };

                class StateMap_FAILED : public StateMap_Default
                {
                public:
                    StateMap_FAILED(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(ObjectGroupContext&);
                };

                class StateMap_ERROR : public StateMap_Default
                {
                public:
                    StateMap_ERROR(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                };

                class ObjectGroupContext : public statemap::FSMContext
                {
                public:
                    ObjectGroupContext(ObjectGroup& owner)
                    : _owner(owner)
                    {
                        setState(StateMap::INITIAL);
                        StateMap::INITIAL.Entry(*this);
                    };

                    ObjectGroup& getOwner() const
                    {
                        return(_owner);
                    };

                    ObjectGroupState& getState() const
                    {
                        if (_state == NULL)
                        {
                            throw statemap::StateUndefinedException();
                        }

                        return (dynamic_cast<ObjectGroupState&>(*_state));
                    };

                    void Unknown()
                    {
                        setTransition("Unknown");
                        (getState()).Unknown(*this);
                        setTransition(NULL);
                    };

                    void ev_backup_ready(Object::Object& objctxt)
                    {
                        setTransition("ev_backup_ready");
                        (getState()).ev_backup_ready(*this, objctxt);
                        setTransition(NULL);
                    };

                    void ev_member_removed(Object::Object& objctxt)
                    {
                        setTransition("ev_member_removed");
                        (getState()).ev_member_removed(*this, objctxt);
                        setTransition(NULL);
                    };

                    void ev_primary_ready(Object::Object& objctxt)
                    {
                        setTransition("ev_primary_ready");
                        (getState()).ev_primary_ready(*this, objctxt);
                        setTransition(NULL);
                    };

                    static ObjectGroupState& valueOf(int id);

                private:
                    ObjectGroup& _owner;

                private:
                    const static int MIN_INDEX;
                    const static int MAX_INDEX;
                    static ObjectGroupState* _States[];
                };
            };
        };
    };
};
#endif
