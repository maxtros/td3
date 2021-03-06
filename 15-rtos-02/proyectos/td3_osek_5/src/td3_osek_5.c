/* Copyright 2016, Ana Lattuca
 * Copyright 2016, M. Sebastian Tobar
 * Copyright 2016, Matías Pecchia
 *
 * This file is Firmware for educational purposes
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*==================[inclusions]=============================================*/
#include "os.h"               /* <= operating system header */
#include "ciaaPOSIX_stdio.h"  /* <= device handler header */
#include "ciaaPOSIX_string.h" /* <= string header */
#include "ciaak.h"            /* <= ciaa kernel header */
#include "td3_osek_5.h"         /* <= own header */

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/** \brief File descriptor for digital output ports
 *
 * Device path /dev/dio/out/0
 */
static int32_t fd_out;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
/** \brief Main function
 *
 * This is the main entry point of the software.
 *
 * \returns 0
 *
 * \remarks This function never returns. Return value is only to avoid compiler
 *          warnings or errors.
 */
int main(void)
{
   /* Starts the operating system in the Application Mode 1 */
   /* This example has only one Application Mode */
   StartOS(AppMode1);

   /* StartOs shall never returns, but to avoid compiler warnings or errors
    * 0 is returned */
   return 0;
}

/** \brief Error Hook function
 *
 * This fucntion is called from the os if an os interface (API) returns an
 * error. Is for debugging proposes. If called this function triggers a
 * ShutdownOs which ends in a while(1).
 *
 * The values:
 *    OSErrorGetServiceId
 *    OSErrorGetParam1
 *    OSErrorGetParam2
 *    OSErrorGetParam3
 *    OSErrorGetRet
 *
 * will provide you the interface, the input parameters and the returned value.
 * For more details see the OSEK specification:
 * http://portal.osek-vdx.org/files/pdf/specs/os223.pdf
 *
 */
void ErrorHook(void)
{
   ciaaPOSIX_printf("ErrorHook was called\n");
   ciaaPOSIX_printf("Service: %d, P1: %d, P2: %d, P3: %d, RET: %d\n",
       OSErrorGetServiceId(), OSErrorGetParam1(),
       OSErrorGetParam2(), OSErrorGetParam3(), OSErrorGetRet());
   ShutdownOS(0);
}

/** \brief Initial task
 *
 * This task is started automatically in the application mode 1.
 */
TASK(InitTask)
{
	uint8_t outputs;

   /* init CIAA kernel and devices */
   ciaak_start();

   /* print message (only on x86) */
   ciaaPOSIX_printf("InitTask...\n");

   /* open CIAA digital outputs */
   fd_out = ciaaPOSIX_open("/dev/dio/out/0", ciaaPOSIX_O_RDWR);
   outputs ^= 0x08;
   ciaaPOSIX_write(fd_out, &outputs, 1);

   /* terminate task */
   ciaaPOSIX_printf("InitTask: ChainTask...\n");
   ChainTask(LowPriorityTask);
}


TASK(LowPriorityTask)
{
   uint8_t outputs;

   ciaaPOSIX_printf("LowPriorityTask...\n");

   /* turn off/on output */
   ciaaPOSIX_read(fd_out, &outputs, 1);
   outputs ^= 0x08;
   ciaaPOSIX_write(fd_out, &outputs, 1);

   ciaaPOSIX_printf("LowPriorityTask: GetResource()\n");
   GetResource(RES);

   ciaaPOSIX_printf("LowPriorityTask: ActivateTaks(HighPriorityTask)\n");
   ActivateTask(HighPriorityTask);

   ciaaPOSIX_printf("LowPriorityTask: ReleaseResource()\n");
   ReleaseResource(RES);

   /* terminate task */
   ciaaPOSIX_printf("LowPriorityTask: Terminate...\n");
   TerminateTask();
}

/** \brief HighPriorityTask
 *
 *
 */
TASK(HighPriorityTask)
{
   uint8_t outputs;

   ciaaPOSIX_printf("HighPriorityTask...\n");

   ciaaPOSIX_read(fd_out, &outputs, 1);
   outputs ^= 0x04;
   ciaaPOSIX_write(fd_out, &outputs, 1);

   ciaaPOSIX_printf("HighPriorityTask: GetResource()\n");
   GetResource(RES);


   ciaaPOSIX_printf("HighPriorityTask: ReleaseResource()\n");
   ReleaseResource(RES);


   /* terminate task */
   ciaaPOSIX_printf("HighPriorityTask: Terminate...\n");
   TerminateTask();
}

/*==================[end of file]============================================*/

