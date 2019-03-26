/* Defines  InitTime() and  double cputime() */

/* This was adapted from the file sysbits.c of the YAP compiler:    */
/*************************************************************************
*									 *
*	 YAP Prolog 							 *
*									 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
*									 *
**************************************************************************
*									 *
* File:		sysbits.c						 *
* Last rev:	4/03/88							 *
* mods:									 *
* comments:	very much machine dependent routines			 *
*									 *
*************************************************************************/

/*  the YAP compiler is available from http://www.ncc.up.pt/~vsc/Yap   */


#ifdef sun
#define SUN 1
#endif

#include <sys/param.h>
#include <sys/types.h>
#include <sys/times.h>

static double real_cputime();
static void InitTime();

#define TicksPerSec	100.0

/* #ifdef SUN */
/* #ifdef sparc */
/* #define TicksPerSec	100.0 */
/* #else */
/* #define TicksPerSec	60.0 */
/* #endif */

/* #endif */

/* #ifdef hpux */
/* #define TicksPerSec	60.0 */
/* #endif */
/* #ifdef mips */
/* #ifndef ultrix */
/* #define TicksPerSec     100.0 */
/* #endif ultrix */
/* #endif mips */
/* #ifdef ultrix */
/* #define TicksPerSec	60.0 */
/* #endif */
/* #ifdef STRIDE */
/* #define TicksPerSec	100.0 */
/* #endif */
/* #ifdef DGUX */
/* #define bsd4_2 */
/* #endif */

/* #ifdef bsd4_2 */

/* #include <sys/time.h> */
/* #include <sys/resource.h> */

/* static double real_cputime() */
/* { */
/*   struct rusage rusage; */
  
/*   getrusage(RUSAGE_SELF,&rusage); */
/*   return (double)rusage.ru_utime.tv_sec+(double)rusage.ru_utime.tv_usec/1.0e6; */
/* } */

/* #else  */
static double real_cputime()
{
	struct tms t;
	times(&t);
	return(( ((double) t.tms_utime) / TicksPerSec));
}
/* #endif */

static double StartOfTimes;

static void InitTime()
{
	StartOfTimes = real_cputime();
}

double cputime()
{
	return(real_cputime()-StartOfTimes);
}

