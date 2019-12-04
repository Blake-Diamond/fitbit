/******************************************************************************/
/*                                                                            */
/* main.c -- Demo for the PmodNAV using SPI and UART                          */
/*                                                                            */
/******************************************************************************/
/* Author: Samuel Lowe, Arthur Brown                                          */
/* Copyright 2016, Digilent Inc.                                              */
/******************************************************************************/
/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/******************************************************************************/
/* File Description:                                                          */
/*                                                                            */
/* This file contains a demo to be used with the PmodNAV. Accelerometer,      */
/* Magnetometer, Temperature, and Barometric Pressure data is captured over a */
/* SPI interface and displayed over UART.                                     */
/*                                                                            */
/* This application configures UART 16550 to baud rate 9600.                  */
/* PS7 UART (Zynq) is not initialized by this application, since bootrom/bsp  */
/* configures it to baud rate 115200                                          */
/*                                                                            */
/*  ------------------------------------------------                          */
/*  | UART TYPE   BAUD RATE                        |                          */
/*  ------------------------------------------------                          */
/*    Uartns550   9600                                                        */
/*    Uartlite    Configurable only in HW design                              */
/*    ps7_uart    115200 (configured by bootrom/bsp)                          */
/*                                                                            */
/******************************************************************************/
/* Revision History:                                                          */
/*                                                                            */
/*    11/16/2016(SamL):     Created                                           */
/*    03/16/2017(ArtVVB):   Completed & Validated                             */
/*    11/01/2017(ArtVVB):   2016.4 Maintenance                                */
/*    02/20/2018(atangzwj): Validated for Vivado 2017.4                       */
/*                                                                            */
/******************************************************************************/
/* Problems:                                                                  */
/*                                                                            */
/* In order to include "math.h" you must include m in compiler settings.      */
/* See https://www.xilinx.com/support/answers/52971.html                      */
/*                                                                            */
/******************************************************************************/

/***************************** Include Files *******************************/

#include <stdio.h>
#include "PmodNAV.h"
#include "sleep.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "qpn_port.h"                                       /* QP-nano port */
#include "bsp.h"                             /* Board Support Package (BSP) */
#include "lab3a.h"                               /* application interface */


/***************************** Function Definitions ************************/
static QEvent l_lab3Queue[30];

QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
	{ (QActive *)0,            (QEvent *)0,          0                    },
	{ (QActive *)&AO_Lab3A,    l_lab3Queue,         Q_DIM(l_lab3Queue)  }
};

Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);


int main(void) {
	Lab3A_ctor(); // inside of lab2a.c
	BSP_init(); // inside of bsp.c, starts out empty!
	QF_run(); // inside of qfn.c
   return 0;
}

