/*
 *  --------------------------------------------------------------------------
 *
 *                               GICSAFe-Firmware
 *                               ----------------
 *
 *                      Copyright (C) 2019 CONICET-GICSAFe
 *          All rights reserved. Protected by international copyright laws.
 *
 *  Contact information:
 *  site: https://github.com/gicsafe-firmware
 *  e-mail: <someone>@<somewhere>
 *  ---------------------------------------------------------------------------
 */

/**
 *  \file   serial.c
 *  \brief  Implements the specifications.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  CaMa  Carlos Mancón  manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "serial.h"
/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void init_serial_hard( int device_no, SERIAL_CBACK_T *p ){

}
void connect_serial( int device_no ){

}
void tx_data( int device_no, char byte ){

}
void tx_n_data( int device_no, char *p, char qty ){

}
void reset_rts( int device_no ){

}
void set_rts( int device_no ){

}
void reset_dtr( int device_no ){

}
void set_dtr( int device_no ){

}
void disconnect_serial( int device_no ){

}
void deinit_serial_hard( int device_no ){

}


void init_serial( void (*prcv)(unsigned char byte) ){

}
void close_mserial( void ){

}

void ser_put_string( int ch, char * p ){

}
void ser_printf( char *fmt, ... ){

}
/* ------------------------------ End of file ------------------------------ */
