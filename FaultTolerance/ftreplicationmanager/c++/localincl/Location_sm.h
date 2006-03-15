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
#ifndef _H_LOCATION_SM
#define _H_LOCATION_SM

#define SMC_USES_IOSTREAMS

#include "ftreplicationmanager/statemap.h"
#include "ftreplicationmanager/ObjectGroup.hpp"
#include "ftreplicationmanager/Object.hpp"

namespace Cdmw
{
    namespace FT
    {
        namespace ReplicationManager
        {
            namespace Location
            {
                // Forward declarations.
                class StateMap;
                class StateMap_INITIAL;
                class StateMap_INSERTING_PRIMARY;
                class StateMap_PRIMARY;
                class StateMap_INSERTING_BACKUP;
                class StateMap_BACKUP;
                class StateMap_FAILED;
                class StateMap_ERROR;
                class StateMap_Default;
                class LocationState;
                class LocationContext;
                class Location;

                class LocationState : public statemap::State
                {
                public:
                    LocationState(const char *name, int id)
                    : statemap::State(name, id)
                    {};

                    virtual void Entry(LocationContext&) {};
                    virtual void Exit(LocationContext&) {};

                    virtual void Unknown(LocationContext& context);
                    virtual void ev_add_member(LocationContext& context);
                    virtual void ev_add_member(LocationContext& context, ObjectGroup::ObjectGroup& grpctxt, Object::Object& objctxt, CORBA::Object_ptr member, const ::FT::Location& primary_loc);
                    virtual void ev_create_object(LocationContext& context);
                    virtual void ev_failure(LocationContext& context);
                    virtual void ev_initial_backup(LocationContext& context);
                    virtual void ev_set_backup(LocationContext& context);
                    virtual void ev_set_primary(LocationContext& context);

                protected:
                    virtual void Default(LocationContext& context);
                };

                class StateMap
                {
                public:
                    static StateMap_INITIAL    INITIAL;
                    static StateMap_INSERTING_PRIMARY    INSERTING_PRIMARY;
                    static StateMap_PRIMARY    PRIMARY;
                    static StateMap_INSERTING_BACKUP    INSERTING_BACKUP;
                    static StateMap_BACKUP    BACKUP;
                    static StateMap_FAILED    FAILED;
                    static StateMap_ERROR    ERROR;
                };

                class StateMap_Default : public LocationState
                {
                public:
                    StateMap_Default(const char *name, int id)
                    : LocationState(name, id)
                    {};

                    virtual void Unknown(LocationContext& context);
                };

                class StateMap_INITIAL : public StateMap_Default
                {
                public:
                    StateMap_INITIAL(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(LocationContext&);
                    void ev_add_member(LocationContext& context, ObjectGroup::ObjectGroup& grpctxt, Object::Object& objctxt, CORBA::Object_ptr member, const ::FT::Location& primary_loc);
                    void ev_create_object(LocationContext& context);
                    void ev_failure(LocationContext& context);
                    void ev_set_backup(LocationContext& context);
                };

                class StateMap_INSERTING_PRIMARY : public StateMap_Default
                {
                public:
                    StateMap_INSERTING_PRIMARY(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(LocationContext&);
                    void ev_add_member(LocationContext& context, ObjectGroup::ObjectGroup& grpctxt, Object::Object& objctxt, CORBA::Object_ptr member, const ::FT::Location& primary_loc);
                    void ev_create_object(LocationContext& context);
                    void ev_failure(LocationContext& context);
                };

                class StateMap_PRIMARY : public StateMap_Default
                {
                public:
                    StateMap_PRIMARY(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(LocationContext&);
                    void ev_failure(LocationContext& context);
                    void ev_set_backup(LocationContext& context);
                };

                class StateMap_INSERTING_BACKUP : public StateMap_Default
                {
                public:
                    StateMap_INSERTING_BACKUP(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(LocationContext&);
                    void ev_add_member(LocationContext& context, ObjectGroup::ObjectGroup& grpctxt, Object::Object& objctxt, CORBA::Object_ptr member, const ::FT::Location& primary_loc);
                    void ev_create_object(LocationContext& context);
                    void ev_failure(LocationContext& context);
                };

                class StateMap_BACKUP : public StateMap_Default
                {
                public:
                    StateMap_BACKUP(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(LocationContext&);
                    void ev_failure(LocationContext& context);
                    void ev_set_primary(LocationContext& context);
                };

                class StateMap_FAILED : public StateMap_Default
                {
                public:
                    StateMap_FAILED(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(LocationContext&);
                };

                class StateMap_ERROR : public StateMap_Default
                {
                public:
                    StateMap_ERROR(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void ev_add_member(LocationContext& context);
                    void ev_create_object(LocationContext& context);
                    void ev_failure(LocationContext& context);
                    void ev_initial_backup(LocationContext& context);
                    void ev_set_backup(LocationContext& context);
                    void ev_set_primary(LocationContext& context);
                };

                class LocationContext : public statemap::FSMContext
                {
                public:
                    LocationContext(Location& owner)
                    : _owner(owner)
                    {
                        setState(StateMap::INITIAL);
                        StateMap::INITIAL.Entry(*this);
                    };

                    Location& getOwner() const
                    {
                        return(_owner);
                    };

                    LocationState& getState() const
                    {
                        if (_state == NULL)
                        {
                            throw statemap::StateUndefinedException();
                        }

                        return (dynamic_cast<LocationState&>(*_state));
                    };

                    void Unknown()
                    {
                        setTransition("Unknown");
                        (getState()).Unknown(*this);
                        setTransition(NULL);
                    };

                    void ev_add_member()
                    {
                        setTransition("ev_add_member");
                        (getState()).ev_add_member(*this);
                        setTransition(NULL);
                    };

                    void ev_add_member(ObjectGroup::ObjectGroup& grpctxt, Object::Object& objctxt, CORBA::Object_ptr member, const ::FT::Location& primary_loc)
                    {
                        setTransition("ev_add_member");
                        (getState()).ev_add_member(*this, grpctxt, objctxt, member, primary_loc);
                        setTransition(NULL);
                    };

                    void ev_create_object()
                    {
                        setTransition("ev_create_object");
                        (getState()).ev_create_object(*this);
                        setTransition(NULL);
                    };

                    void ev_failure()
                    {
                        setTransition("ev_failure");
                        (getState()).ev_failure(*this);
                        setTransition(NULL);
                    };

                    void ev_initial_backup()
                    {
                        setTransition("ev_initial_backup");
                        (getState()).ev_initial_backup(*this);
                        setTransition(NULL);
                    };

                    void ev_set_backup()
                    {
                        setTransition("ev_set_backup");
                        (getState()).ev_set_backup(*this);
                        setTransition(NULL);
                    };

                    void ev_set_primary()
                    {
                        setTransition("ev_set_primary");
                        (getState()).ev_set_primary(*this);
                        setTransition(NULL);
                    };

                    static LocationState& valueOf(int id);

                private:
                    Location& _owner;

                private:
                    const static int MIN_INDEX;
                    const static int MAX_INDEX;
                    static LocationState* _States[];
                };
            };
        };
    };
};
#endif
