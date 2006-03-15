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


package cdmw.ccm.cif;

import cdmw.orbsupport.ExceptionMinorCodes;

//
// IDL:omg.org/Components/HomeConfiguration:1.0
//
public class PushConsumerImpl
    extends com.thalesgroup.CdmwCcmCif.PushConsumerPOA {

    /**
     * non cdmw consumer
     */
    private org.omg.Components.EventConsumerBase ccmConsumer;

    /**
     * LifeCycleObjectBase object used for delegation
     */
    private LifeCycleObjectBase lifecycleObjectBase;

    /**
     * constructor
     * .
     * @param ctx
     * @param ccmConsumer
     */
    public PushConsumerImpl(
        com.thalesgroup.CdmwCcmContainer.CCM2Context ctx,
        org.omg.Components.EventConsumerBase ccmConsumer) {
        this.lifecycleObjectBase = new LifeCycleObjectBase(ctx);
        this.ccmConsumer = ccmConsumer;
    }

    //
    // IDL:omg.org/CosEventComm/PushConsumer/disconnect_push_consumer:1.0
    //
    public void disconnect_push_consumer() {
        // do nothing
    }

    //
    // IDL:omg.org/CosEventComm/PushConsumer/push:1.0
    //
    public void push(org.omg.CORBA.Any data) {
        /* PCR-0049
        java.io.Serializable valueType =
            org.omg.CORBA.ValueBaseHelper.extract(data);
        org.omg.Components.EventBase evt =
            (org.omg.Components.EventBase) valueType;
        */
        cdmw.orbsupport.Codec codec = new cdmw.orbsupport.Codec();
        org.omg.Components.EventBase evt = (org.omg.Components.EventBase)
            org.omg.CORBA.ValueBaseHelper.read(codec.decode(data));

        if (evt == null) {
            System.err.println("PushConsumerImpl push nil event!");
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMBadEventType,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        try {
            this.ccmConsumer.push_event(evt);
        } catch (org.omg.Components.BadEventType bet) {
            System.err.println("PushConsumerImpl push bad event type!");
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMBadEventType,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        }

    }

    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/copy:1.0
    //
    public org.omg.CosLifeCycle.LifeCycleObject copy(
        org.omg.CosLifeCycle.FactoryFinder there,
        org.omg.CosLifeCycle.NVP[] the_criteria)
        throws
            org.omg.CosLifeCycle.NoFactory,
            org.omg.CosLifeCycle.NotCopyable,
            org.omg.CosLifeCycle.InvalidCriteria,
            org.omg.CosLifeCycle.CannotMeetCriteria {
        return this.lifecycleObjectBase.copy(there, the_criteria);
    }

    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/move:1.0
    //
    public void move(
        org.omg.CosLifeCycle.FactoryFinder there,
        org.omg.CosLifeCycle.NVP[] the_criteria)
        throws
            org.omg.CosLifeCycle.NoFactory,
            org.omg.CosLifeCycle.NotMovable,
            org.omg.CosLifeCycle.InvalidCriteria,
            org.omg.CosLifeCycle.CannotMeetCriteria {
        this.lifecycleObjectBase.move(there, the_criteria);
    }

    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/remove:1.0
    //
    public void remove() throws org.omg.CosLifeCycle.NotRemovable {
        this.lifecycleObjectBase.remove();
    }

}
