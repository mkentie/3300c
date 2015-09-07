/* Copyright (C) 2002 Bertrik Sikken, Ricardo Martins
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along with
* this program; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place - Suite 330, Boston, MA 02111-1307, USA. */

/* User interface for the NIASH chipset drivers
* $Id: main.c,v 1.5 2004/10/27 18:13:19 ullsig Exp $
*/
/* FIXME: VERIFY IF THE DEVICE FILE EXISTS */
#include <windows.h>
#include <time.h>

//#include <sys/times.h>
//#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include <unistd.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/stat.h>
#include "niash_core.h"
#include "niash_xfer.h"



#define BENCH_SIZE 0x4000
#define CALIB_FILE_NAME "cal.dat"

/* Debug messages levels */
#define DBG fprintf
FILE *DBG_ASSERT = NULL;
FILE *DBG_ERR = NULL;
FILE *DBG_MSG = NULL;

/* Program information */
const char *version = "NIASH Scanner's Testtool v0.3a\n";
const char *header =
"Copyright (C) 2002 Bertrik Sikken, Ricardo Martins\n"
"This test tool is meant for experimenting / testing flatbed scanners\n"
"with the NIASH chipset (HP3300C, HP3400C, HP4300C and Agfa Touch).\n"
"It comes with NO WARRANTY, to the extent permitted by law.\n\n";
const char *help =
" -b       blinks HP4300c LEDs / shows some numbers on the LCD (UNTESTED)\n"
" -l       toggles lamp on/off\n"
" -o       calibrates for black/white level (simple generic algorithm)\n"
" -p       shows all registers\n"
" -q       supresses console messages\n"
" -s W,H   scans image with width W, height H to standard output or to\n"
"          FILENAME if specified\n"
" -t       benchmarks the USB throughput\n"
" -v       displays all available debug information\n"
" -V       prints program name and version\n"
" -w R,D   writes data D to register R\n"
"\nReport bugs to <bertrik@zonnet.nl>, <rmartins@dcc.online.pt>\n";

struct arguments
{
    int scan;
    int width;
    int height;
    int blink;
    int lamp;
    int ocalib;
    int dump;
    int bench;
    int write;
    int data;
    int reg;
    int silent;
    int verbose;
    int version;
    int help;
    char *filename;
};

/*************************************************************************
Experimental simple calibration

Basic idea:
* a strip starting at position 0 is scanned in
* per-pixel white level is determined by average of first 4 lines
* global black level is determined by minimum value of all lines
*************************************************************************/
SANE_Bool
SimpleCalib(THWParams * pHWPar, unsigned char *pabCalibTable)
{
    return SimpleCalibExt(pHWPar, pabCalibTable, NULL);
}


/* utility function to show a hexdump of a buffer */
void
DumpHex(unsigned char *pabData, int iLen, int iWidth)
{
    int i;
    for (i = 0; i < iLen; i++)
    {
        if ((i % iWidth) == 0)
        {
            printf("\n%04X", i);
        }
        printf(" %02X", pabData[i]);
    }
    printf("\n");
}

/*************************************************************************
ScanLines
=========
Helper function to scan in an image and output the result to a file.

IN  pFile     File to write image data to
pParams   User-defineable settings describing the image to be scanned
pHWParams Scanner hardware settings

*************************************************************************/
void
ScanLines(FILE * pFile, TScanParams * pParams, THWParams * pHWParams)
{
    static unsigned char abBuf[HW_PIXELS * 3];
    int iBytesPerLine;
    int iHeight;
    int iHandle;
    SANE_Bool iReversedHead;
    TDataPipe DataPipe;

    iHandle = pHWParams->iXferHandle;

    iHeight = pParams->iHeight;
    iBytesPerLine = pParams->iWidth * 3;
    DataPipe.iSkipLines = 0;
    if (pHWParams->fReg07)
    {
        DataPipe.iSkipLines +=
            (pHWParams->iTopLeftY * pParams->iLpi * 10) / 254;
    }
    iReversedHead = pHWParams->iReversedHead;

    if (InitScan(pParams, pHWParams))
    {
        CircBufferInit(iHandle, &DataPipe,
            pParams->iWidth, pParams->iHeight,
            pHWParams->iSensorSkew * pParams->iLpi / HW_LPI,
            iReversedHead, 1, 1);
        while (iHeight-- > 0)
        {
            CircBufferGetLineEx(iHandle, &DataPipe, abBuf, iReversedHead,
                SANE_TRUE);
            fwrite(abBuf, 1, iBytesPerLine, pFile);
        }
        CircBufferExit(&DataPipe);
    }
    FinishScan(pHWParams);
}

int
parse_args(int argc, char **argv, struct arguments *arguments)
{
    int width = 1325;		/* default value */
    int height = 1700;		/* default value */
    int c;
    char *arg_list = "blopqs:tvw:hV";
    int stepback = 0;
    arguments->filename = 0;
    while ((c = getopt(argc, argv, arg_list)) != -1)
    {
        switch (c)
        {
        case 'q':
            arguments->silent = 1;
            break;
        case 'v':
            arguments->verbose = 1;
            break;
        case 'b':
            arguments->blink = 1;
            break;
        case 's':
            arguments->scan = 1;
            if (sscanf(optarg, "%d,%d", &arguments->width, &arguments->height)
                < 2)
            {
                arguments->width = width;
                arguments->height = height;
                --stepback;
            }
            /* Sets dimensions */
            if (arguments->width > width || arguments->width < 1)
            {
                DBG(DBG_ERR, "Invalid width, using default\n");
                arguments->width = width;
            }
            if (arguments->height > height || arguments->height < 1)
            {
                DBG(DBG_ERR, "Invalid height, using default\n");
                arguments->height = height;
            }
            break;
        case 'l':
            arguments->lamp = 1;
            break;
        case 'o':
            arguments->ocalib = 1;
            break;
        case 'p':
            arguments->dump = 1;
            break;
        case 't':
            arguments->bench = 1;
            break;
        case 'V':
            arguments->version = 1;
            break;
        case 'w':
            /* FIXME: register / data bounds are not checked */
            arguments->write = 1;
            if (sscanf(optarg, "%x,%x", &arguments->reg, &arguments->data) !=
                2)
            {
                arguments->help = 1;
                --stepback;
            }
            break;
        case 'h':
            /* Falls */
        case '?':
            /* Falls */
        default:
            arguments->help = 1;
            return 1;
        }
    }
    /* Non-option argument */
    arguments->filename = argv[optind + stepback];
    return 0;
}

static int _fDeviceFound;
static char _szDeviceName[100];

/* */
static int
_ReportDevice(TScannerModel * pModel, char const *pszName)
{
    pModel = pModel; //unreferenced param warning

    DBG(DBG_MSG, "_ReportDevice '%s'\n", pszName);

    if (_fDeviceFound)
    {
        DBG(DBG_MSG, "Skipping '%s'\n", pszName);
        return -1;
    }

    strcpy(_szDeviceName, pszName);

    _fDeviceFound = SANE_TRUE;
    return 1;
}


/* main */
int
main(int argc, char *argv[])
{
    char *usage = "Usage: %s [OPTIONS]... [FILENAME]\n\n";
    SANE_Byte abReg[64];
    //SANE_Byte *pabBuf;
    SANE_Bool fLamp;
    THWParams HWParams;
    FILE *out_file = NULL;
    FILE *bucket = NULL;
    int i;
    struct arguments args = { 0 };	/* Arg parsing */
                                    /* Benchmarking */
                                    //struct itimerval set_bmt = { {100, 0}, {100, 0} };
                                    //struct itimerval bmrk;
                                    //double t_spent;
    int iXferHandle;
    TScanParams Params;
    static SANE_Byte abGamma[4096];
    static SANE_Byte abCalibTable[5300 * 6];
    FILE *pFile;

    /* Initializes the bit bucket */
    bucket = fopen("/dev/null", "w");
    if (!bucket)
    {
        bucket = stderr;
    }

    /* Default - only error messages are printed */
    DBG_MSG = bucket;
    DBG_ASSERT = bucket;
    DBG_ERR = stderr;

    /* Let's parse all the options */
    parse_args(argc, argv, &args);

    if (args.help)
    {
        fputs(version, stderr);
        fputs(header, stderr);
        fprintf(stderr, usage, argv[0]);
        fputs(help, stderr);
        exit(EXIT_SUCCESS);
    }

    if (args.version)
    {
        fprintf(stderr, "%s", version);
    }

    /* Check's how much debug information we should print out */
    if (args.verbose)
    {
        DBG_MSG = stderr;
        DBG_ASSERT = stderr;
    }

    if (args.silent)
    {
        DBG_ERR = bucket;
    }


    /* Scanner initialization */
    DBG(DBG_MSG, "Initializing scanner\n");
    NiashXferInit(_ReportDevice);

    if (NiashOpen(&HWParams, _szDeviceName) < 0)
    {
        DBG(DBG_ERR, "NiashOpen failed\n");
        exit(1);
    }
    iXferHandle = HWParams.iXferHandle;

    /*** GROUP 0: options that should not be executed with others ***/
    /* Scans the given area to file or stdout */
    if (args.scan)
    {
        out_file = fopen(args.filename, "wb");

        if (!out_file)
        {
            out_file = stdout;
        }

        DBG(DBG_MSG, "Scanning area (%d, %d) to %s\n", args.width,
            args.height, args.filename ? args.filename : "stdout");

        Params.iDpi = 150;
        Params.iLpi = 150;
        Params.iTop = 10;
        Params.iBottom = HP3300C_BOTTOM;
        Params.iLeft = 0;
        Params.iWidth = args.width;
        Params.iHeight = args.height;
        Params.fCalib = SANE_FALSE;

        /* use a gamma of 1.6 */
        CalcGamma(abGamma, 1.6);

        /* try to open the calibration data file */
        pFile = fopen(CALIB_FILE_NAME, "rb");
        if (pFile != NULL)
        {
            DBG(DBG_MSG, "Using calibration file\n");
            fread(abCalibTable, 1, 5300 * 3 * 2, pFile);
            fclose(pFile);
            WriteGammaCalibTable(abGamma, abGamma, abGamma, abCalibTable, 0, 0,
                &HWParams);
        }
        else
        {
            WriteGammaCalibTable(abGamma, abGamma, abGamma, NULL, 256, 0,
                &HWParams);
        }

        fprintf(out_file, "P6\n#%s%d %d\n255\n", version, args.width,
            args.height);

        ScanLines(out_file, &Params, &HWParams);
        if (out_file != stdout)
        {
            fclose(out_file);
        }


        goto exit_success;
    }

    /* Alternative way of calibration (HP3400c and HP4300c) */
    if (args.ocalib)
    {
        DBG(DBG_MSG, "Creating calibration file\n");
        SimpleCalib(&HWParams, abCalibTable);

        _unlink(CALIB_FILE_NAME);
        pFile = fopen(CALIB_FILE_NAME, "wb");
        if (pFile != NULL)
        {
            fwrite(abCalibTable, 1, 5300 * 6, pFile);
            fclose(pFile);
        }
        else
        {
            DBG(DBG_ERR, "Could not write calib file\n");
        }

        goto exit_success;
    }

    /* Toggles scanner lamp on/off */
    if (args.lamp)
    {
        DBG(DBG_MSG, "Toggling scanner lamp\n");
        if (GetLamp(&HWParams, &fLamp))
        {
            SetLamp(&HWParams, !fLamp);
        }

        goto exit_success;
    }

    /* It is supposed to blink the LEDs and show some numbers
    * on the LCD of a HP4300c scanner (UNTESTED) */
    if (args.blink)
    {
        DBG(DBG_MSG, "Blinking HP4300c LEDs\n");
        for (i = 0; i < 32; i++)
        {
            printf("0x%02X\n", i);
            NiashWriteReg(iXferHandle, 0x3c, (unsigned char)i);
            NiashWriteReg(iXferHandle, 0x3d, (unsigned char)(i & 3));
            //usleep (500000L);
            Sleep(500);
        }
        goto exit_success;
    }
    /* End of GROUP 0 */

    /*** GROUP 1: options that can be executed with others ***/
    /* Shows all registers */
    if (args.dump)
    {
        DBG(DBG_MSG, "Showing all registers\n");
        memset(abReg, 0x55, sizeof(abReg));
        for (i = 0; i < sizeof(abReg); i++)
            NiashReadReg(iXferHandle, (unsigned char)i, &abReg[i]);
        DumpHex(abReg, sizeof(abReg), 8);
    }

    /* Benchmarks the USB throughput
    * FIXME: Not very sure if the times are acurated */
    if (args.bench)
    {
        //       DBG (DBG_MSG, "Benchmarking: %d kB...\n", (BENCH_SIZE * 100) / 1024);
        //       pabBuf = malloc (BENCH_SIZE);
        // 
        //       /* Begin counting */
        //       setitimer (ITIMER_REAL, &set_bmt, NULL);
        // 
        //       if (pabBuf != NULL)
        // 	{
        // 	  NiashWriteReg (iXferHandle, 0x02, 0x84);
        // 	  for (i = 0; i < 100; i++)
        // 	    {
        // 	      NiashReadBulk (iXferHandle, pabBuf, BENCH_SIZE);
        // 	    }
        // 	  NiashWriteReg (iXferHandle, 0x02, 0x80);
        // 
        // 	  getitimer (ITIMER_REAL, &bmrk);
        // 	  t_spent = ((bmrk.it_value.tv_sec * 1000000.0) +
        // 		     bmrk.it_value.tv_usec);
        // 	  t_spent = (double) (100.0 - (t_spent / 1000000.0));
        // 
        // 	  DBG (DBG_MSG,
        // 	       "Elapsed time (real time): %5.4f seconds\nTransfer-Rate: %d kByte/s\n",
        // 	       t_spent, (int) (BENCH_SIZE / (10 * t_spent)));
        // 	  free (pabBuf);
        //	}
    }

    /* Writes data to register */
    if (args.write)
    {
        DBG(DBG_MSG, "Writing %d to register %d\n", args.data, args.reg);
        NiashWriteReg(iXferHandle, (unsigned char)args.reg, (unsigned char)args.data);
    }
    /* End of GROUP 1 */

    if (bucket != stderr && bucket != NULL)
    {
        fclose(bucket);
    }

    /* yes a goto is ugly, but here it comes in very handy for cleaning up */
exit_success:
    NiashClose(&HWParams);
    exit(EXIT_SUCCESS);
}
