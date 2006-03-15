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
#ifndef _H_OBJECT_SM
#define _H_OBJECT_SM

#define SMC_USES_IOSTREAMS

#include "ftreplicationmanager/statemap.h"

namespace Cdmw
{
    namespace FT
    {
        namespace ReplicationManager
        {
            namespace Object
            {
                // Forward declarations.
                class StateMap;
                class StateMap_INITIAL;
                class StateMap_BACKUP;
                class StateMap_PRIMARY;
                class StateMap_FAILED;
                class StateMap_ERROR;
                class StateMap_Default;
                class ObjectState;
                class ObjectContext;
                class Object;

                class ObjectState : public statemap::State
                {
                public:
                    ObjectState(const char *name, int id)
                    : statemap::State(name, id)
                    {};

                    virtual void Entry(ObjectContext&) {};
                    virtual void Exit(ObjectContext&) {};

                    virtual void Unknown(ObjectContext& context);
                    virtual void ev_failure(ObjectContext& context);
                    virtual void ev_init_backup_done(ObjectContext& context);
                    virtual void ev_init_primary_done(ObjectContext& context);
                    virtual void ev_set_primary(ObjectContext& context);

                protected:
                    virtual void Default(ObjectContext& context);
                };

                class StateMap
                {
                public:
                    static StateMap_INITIAL    INITIAL;
                    static StateMap_BACKUP    BACKUP;
                    static StateMap_PRIMARY    PRIMARY;
                    static StateMap_FAILED    FAILED;
                    static StateMap_ERROR    ERROR;
                };

                class StateMap_Default : public ObjectState
                {
                public:
                    StateMap_Default(const char *name, int id)
                    : ObjectState(name, id)
                    {};

                    virtual void Unknown(ObjectContext& context);
                };

                class StateMap_INITIAL : public StateMap_Default
                {
                public:
                    StateMap_INITIAL(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(ObjectContext&);
                    void ev_failure(ObjectContext& context);
                    void ev_init_backup_done(ObjectContext& context);
                    void ev_init_primary_done(ObjectContext& context);
                };

                class StateMap_BACKUP : public StateMap_Default
                {
                public:
                    StateMap_BACKUP(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(ObjectContext&);
                    void ev_failure(ObjectContext& context);
                    void ev_set_primary(ObjectContext& context);
                };

                class StateMap_PRIMARY : public StateMap_Default
                {
                public:
                    StateMap_PRIMARY(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(ObjectContext&);
                    void ev_failure(ObjectContext& context);
                };

                class StateMap_FAILED : public StateMap_Default
                {
                public:
                    StateMap_FAILED(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void Entry(ObjectContext&);
                };

                class StateMap_ERROR : public StateMap_Default
                {
                public:
                    StateMap_ERROR(const char *name, int id)
                    : StateMap_Default(name, id)
                    {};

                    void ev_failure(ObjectContext& context);
                    void ev_init_backup_done(ObjectContext& context);
                    void ev_init_primary_done(ObjectContext& context);
                    void ev_set_primary(ObjectContext& context);
                };

                class ObjectContext : public statemap::FSMContext
                {
                public:
                    ObjectContext(Object& owner)
                    : _owner(owner)
                    {
                        setState(StateMap::INITIAL);
                        StateMap::INITIAL.Entry(*this);
                    };

                    Object& getOwner() const
                    {
                        return(_owner);
                    };

                    ObjectState& getState() const
                    {
                        if (_state == NULL)
                        {
                            throw statemap::StateUndefinedException();
                        }

                        return (dynamic_cast<ObjectState&>(*_state));
                    };

                    void Unknown()
                    {
                        setTransition("Unknown");
                        (getState()).Unknown(*this);
                        setTransition(NULL);
                    };

                    void ev_failure()
                    {
                        setTransition("ev_failure");
                        (getState()).ev_failure(*this);
                        setTransition(NULL);
                    };

                    void ev_init_backup_done()
                    {
                        setTransition("ev_init_backup_done");
                        (getState()).ev_init_backup_done(*this);
                        setTransition(NULL);
                    };

                    void ev_init_primary_done()
                    {
                        setTransition("ev_init_primary_done");
                        (getState()).ev_init_primary_done(*this);
                        setTransition(NULL);
                    };

                    void ev_set_primary()
                    {
                        setTransition("ev_set_primary");
                        (getState()).ev_set_primary(*this);
                        setTransition(NULL);
                    };

                    static ObjectState& valueOf(int id);

                private:
                    Object& _owner;

                private:
                    const static int MIN_INDEX;
                    const static int MAX_INDEX;
                    static ObjectState* _States[];
                };
            };
        };
    };
};
#endif
