/**
 *  \file       topics.h
 *  \brief      Event topics definitions.
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __TOPICS_H__
#define __TOPICS_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
#define ConnectionTopic_subscribe(me)   rkh_pubsub_subscribe(ConnectionTopic, \
                                                RKH_UPCAST(RKH_SMA_T, (me)))

#define ConnectionTopic_publish(ev, me) rkh_pubsub_publish(ConnectionTopic, \
                                                RKH_UPCAST(RKH_EVT_T, (ev)), \
                                                RKH_UPCAST(RKH_SMA_T, (me)))

/* -------------------------------- Constants ------------------------------ */
/* ................................ Signals ................................ */
typedef enum Topics Topics;
enum Topics
{
	ConnectionTopic,
};

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
