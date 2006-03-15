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




#include <sys/time.h>
#include "cdmw_ossupport_OS.h"


#ifdef __cplusplus
extern "C" {
#endif


static jclass timevalClass = NULL;
static jmethodID timevalConstrID = NULL;


int getTimevalConstructor(JNIEnv *env) {
    jclass timevalClassLocal =
        (*env)->FindClass(env, "cdmw/ossupport/Timeval");
    if (timevalClassLocal == NULL) {
        return 0; // exception thrown
    }

    // Create a global reference
    timevalClass =
        (*env)->NewGlobalRef(env, timevalClassLocal);

    // The local reference is no longer useful
    (*env)->DeleteLocalRef(env, timevalClassLocal);

    // Is the global reference created successfully?
    if (timevalClass == NULL) {
        return 0; // out of memory exception thrown
    }

    // get id of constructor:
    //   Timeval(long seconds, int microseconds)
    timevalConstrID = (*env)->GetMethodID(env, timevalClass,
                              "<init>", "(II)V");
    if (timevalConstrID == NULL) {
        return 0; // exception thrown
    }
    
    return 1;
}




jobject Java_cdmw_ossupport_OS_getTime(JNIEnv *env, jclass jcl) {
    jobject result;
    jclass newExcCls = 0;
    struct timeval the_timeval;

    if (timevalClass == NULL) {
        if (! getTimevalConstructor(env)) {
            (*env)->ExceptionDescribe(env);
            (*env)->ExceptionClear(env);
            newExcCls = (*env)->FindClass(env, "java/lang/RuntimeException");
            if (newExcCls == 0) {   
                // Unable to find the new exception class, give up.
                return NULL;
            }
            (*env)->ThrowNew(env, newExcCls, "Cannot retrieve cdmw.ossupport.Timeval constructor");
        }
    }

    // get time
#if defined(CDMW_POSIX)

    
    if ( gettimeofday ( &the_timeval, NULL ) != 0 ) {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        newExcCls = (*env)->FindClass(env, "java/lang/RuntimeException");
        if (newExcCls == 0) {   
            // Unable to find the new exception class, give up.
            return NULL;
        }
        (*env)->ThrowNew(env, newExcCls, "Cannot retrieve cdmw.ossupport.Timeval constructor");
	}
	
    // create Timeval object
    result = (*env)->NewObject(env, 
                               timevalClass, 
                               timevalConstrID, 
                               the_timeval.tv_sec,
                               the_timeval.tv_usec);
    
    
#else
#    error "Cdmw: implementation not defined"
#endif

    return result;
}


#ifdef __cplusplus
}
#endif


