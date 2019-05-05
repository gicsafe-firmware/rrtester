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
 *  \file   serial.h
 *  \brief  Specifies this module.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  CaMa  Carlos Mancón  manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __SERIAL_H__
#define __SERIAL_H__

/* ----------------------------- Include files ----------------------------- */
#define _GNU_SOURCE
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define SERIAL_PRINTF_BUFFER_SIZE 100

/* ------------------------------- Data types ------------------------------ */

enum
{
	PAR_NONE, PAR_ODD, PAR_EVEN, PAR_MARK, PAR_SPACE
};

enum
{
	STOP_1, STOP_1HALF, STOP2
};

typedef struct
{
	void (*rx)( unsigned char byte );
	void (*serial_connect)( void );
	void (*serial_disconnect)( void );
	void (*serial_data_sent)( void );
	void (*serial_ring)( void );
	void (*serial_cd_on)( void );
	void (*serial_cd_off)( void );
} SERIAL_CBACK_T;

typedef struct
{
	char com_name[20];
	unsigned baud;
	int bit_num;
	int parity;
	int stop_num;
	int is_modem;
	int fd; /* File descriptor for the port */
	SERIAL_CBACK_T *cbacks;
} SERIAL_T;
/* -------------------------- External variables --------------------------- */
extern SERIAL_T serials[];
/* -------------------------- Function prototypes -------------------------- */
void init_serial_hard( int device_no, SERIAL_CBACK_T *p );
void connect_serial( int device_no );
void tx_data( int device_no, char byte );
void tx_n_data( int device_no, char *p, char qty );
void reset_rts( int device_no );
void set_rts( int device_no );
void reset_dtr( int device_no );
void set_dtr( int device_no );
void disconnect_serial( int device_no );
void deinit_serial_hard( int device_no );


void init_serial( void (*prcv)(unsigned char byte) );
void close_mserial( void );

void ser_put_string( int ch, char * p );
void ser_printf( int ch, char *fmt, ... );
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
