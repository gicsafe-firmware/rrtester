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
#include "bsp.h"
/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
void
signal_handler_IO(int sig, siginfo_t *info, void *ucontext)
{
	int i, ch, fd;
	unsigned char c;
	ch = info->si_fd;
	for (i=0; i<NUM_CHANNELS; i++)
	{
		fd = serials[i].fd;
		if (ch == fd)
		{
			while (read(fd, &c, 1))
			{
				serials[i].cbacks->rx(c);
			}
		}
	}
}

void
parseLineOptions(int device_no, struct termios* opts)
{
	int baudrate, numberOfBits;
	switch (serials[device_no].baud) {
		case 0:
			baudrate = B0;
			break;
		case 50:
			baudrate = B50;
			break;
		case 75:
			baudrate = B75;
			break;
		case 110:
			baudrate = B110;
			break;
		case 134:
			baudrate = B134;
			break;
		case 150:
			baudrate = B150;
			break;
		case 200:
			baudrate = B200;
			break;
		case 300:
			baudrate = B300;
			break;
		case 600:
			baudrate = B600;
			break;
		case 1200:
			baudrate = B1200;
			break;
		case 1800:
			baudrate = B1800;
			break;
		case 2400:
			baudrate = B2400;
			break;
		case 4800:
			baudrate = B4800;
			break;
		case 9600:
			baudrate = B9600;
			break;
		case 19200:
			baudrate = B19200;
			break;
		case 38400:
			baudrate = B38400;
			break;
		case 57600:
			baudrate = B57600;
			break;
		case 115200:
			baudrate = B115200;
			break;
		case 230400:
			baudrate = B230400;
			break;
		case 460800:
			baudrate = B460800;
			break;
		case 500000:
			baudrate = B500000;
			break;
		case 576000:
			baudrate = B576000;
			break;
		case 921600:
			baudrate = B921600;
			break;
		case 1000000:
			baudrate = B1000000;
			break;
		case 1152000:
			baudrate = B1152000;
			break;
		case 1500000:
			baudrate = B1500000;
			break;
		case 2000000:
			baudrate = B2000000;
			break;
		case 2500000:
			baudrate = B2500000;
			break;
		case 3000000:
			baudrate = B3000000;
			break;
		case 3500000:
			baudrate = B3500000;
			break;
		case 4000000:
			baudrate = B4000000;
			break;
		default:
			baudrate = B9600;
			printf("Not a valid Baudrate, falling back to 9600\n");
			break;
	}

	cfsetispeed(opts, baudrate);
	cfsetospeed(opts, baudrate);

	switch (serials[device_no].bit_num) {
	case 5:
		numberOfBits = CS5;
		break;
	case 6:
		numberOfBits = CS6;
		break;
	case 7:
		numberOfBits = CS7;
		break;
	case 8:
		numberOfBits = CS8;
		break;
	default:
		numberOfBits = CS8;
		printf("Not a valid baud size, falling back to 8\n");
		break;
	}
	opts->c_cflag &= ~CSIZE; /* Mask the character size bits */
	opts->c_cflag |= numberOfBits;

	switch (serials[device_no].parity) {
		case PAR_ODD:
			opts->c_cflag |= PARENB;
			opts->c_cflag |= PARODD;
			break;
		case PAR_EVEN:
			opts->c_cflag |= PARENB;
			opts->c_cflag &= ~PARODD;
			break;
		case PAR_MARK:
			opts->c_cflag |= PARENB | CMSPAR;
			opts->c_cflag |= PARODD;
			break;
		case PAR_SPACE:
			opts->c_cflag |= PARENB | CMSPAR;
			opts->c_cflag &= ~PARODD;
			break;
		case PAR_NONE:
			opts->c_cflag &= ~PARENB;
			break;
		default:
			opts->c_cflag &= ~PARENB;
			break;
	}

	switch (serials[device_no].stop_num) {
		case STOP_1HALF:
			opts->c_cflag &= ~CSTOPB;
			printf("1.5 bits stop not supported. Falling to 1bit stop.\n");
			break;
		case STOP_1:
			opts->c_cflag &= ~CSTOPB;
			break;
		case STOP2:
			opts->c_cflag |= CSTOPB;
			break;
		default:
			opts->c_cflag &= ~CSTOPB;
			printf("Falling to 1bit stop.\n");
			break;
	}

	/*
	 * Enable the receiver and set local mode...
	 */

	opts->c_cflag |= (CLOCAL | CREAD);

}
/* ---------------------------- Global functions --------------------------- */
void init_serial_hard( int device_no, SERIAL_CBACK_T *p ){

	serials[device_no].cbacks= p;
	struct sigaction saio;
	saio.sa_sigaction = signal_handler_IO;
	saio.sa_flags = SA_SIGINFO;
	saio.sa_restorer = NULL;
	sigaction(SIGIO,&saio,NULL);
}
void connect_serial( int device_no ){
	int fd; /* File descriptor for the port */

	fd = open(serials[device_no].com_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd)
	{
		printf("Unable to open \%s\n", serials[device_no].com_name);
	}
	else
	{
		serials[device_no].fd = fd;
		struct termios options;
		tcgetattr(fd, &options);
		parseLineOptions(device_no, &options);
		tcsetattr(fd, TCSANOW, &options);
		fcntl(fd, F_SETFL,  O_ASYNC );
		fcntl(fd, F_SETSIG, SIGIO);
	}
}

void tx_data( int device_no, char byte ){

	if ( 0 > write(serials[device_no].fd, &byte, 1) )
	{
		printf("Could not write to %s\n", serials[device_no].com_name);
	}
	else
	{
		if (NULL != serials[device_no].cbacks->serial_data_sent)
		{
			serials[device_no].cbacks->serial_data_sent();
		}
	}
}

void tx_n_data( int device_no, char *p, char qty ){

	if ( 0 > write(serials[device_no].fd, p, qty) )
	{
		printf("Could not write to %s\n", serials[device_no].com_name);
	}
	else
	{
		if (NULL != serials[device_no].cbacks->serial_data_sent)
		{
			serials[device_no].cbacks->serial_data_sent();
		}
	}
}

void reset_rts( int device_no ){
	ioctl(device_no, TIOCMBIC,TIOCM_RTS);
	printf("reset_rts called on %d Device.\n", device_no);
}
void set_rts( int device_no ){
	ioctl(device_no, TIOCMBIS, TIOCM_RTS);
	printf("set_rts called on %d Device.\n", device_no);
}
void reset_dtr( int device_no ){
	ioctl(device_no, TIOCMBIC, TIOCM_DTR);
	printf("reset_dtr called on %d Device.\n", device_no);
}
void set_dtr( int device_no ){
	ioctl(device_no, TIOCMBIS, TIOCM_DTR);
	printf("set_dtr called on %d Device.\n", device_no);
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
