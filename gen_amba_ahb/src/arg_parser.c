//--------------------------------------------------------
// Copyright (c) 2016-2021 by Ando Ki (andoki@gmail.com)
// The contents and codes along with it are prepared
// in the hope that it will be useful to understand
// Ando Ki's work, but WITHOUT ANY WARRANTY.
// The design is not guaranteed to work on all systems.
// While there are no known issues with using the design,
// no technical support will be provided for problems
// that might arise.
//--------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#ifdef WIN32
#	include <windows.h>
#	include <io.h>
#endif

const char program[]="gen_amba_ahb";
const unsigned int version=0x20210710;
int verbose=0;
unsigned int numM=2;
unsigned int numS=2;
unsigned int lite=0;
char module[128]="\0\0";
char prefix[128]="\0\0";
char file[256]="\0\0";
FILE *fo;

//-----------------------------------------------------
//     *name,  has_arg, *flag, val
// if (flag==NULL) getopt_long return 'val'.
// otherwise, getopt_long returns 0, and '*flag' is set by 'val'.
static struct option longopts[] = {
       {"lite"   , no_argument      , 0, 'T'}
     , {"master" , required_argument, 0, 'M'}
     , {"slave"  , required_argument, 0, 'S'}
     , {"module" , required_argument, 0, 'D'}
     , {"prefix" , required_argument, 0, 'P'}
     , {"output" , required_argument, 0, 'O'}
     , {"verbose", required_argument, 0, 'g'}
     , {"version", no_argument      , 0, 'v'}
     , {"license", no_argument      , 0, 'l'}
     , {"help"   , no_argument      , 0, 'h'}
     , { 0       , 0                , 0,  0 }
};

//--------------------------------------------------------
// 1. get simulator options from command line
// 2. returns 0 on successful completion
int arg_parser(int argc, char **argv) {
  int opt;
  int longidx=0;
  extern void help(int, char **);
  extern void print_license(void);
  extern void print_version(void);

  //-----------------------------------------------------
  while ((opt=getopt_long(argc, argv, "TM:S:D:P:O:g:vlh?", longopts, &longidx))!=-1) {
     switch (opt) {
     case 'T': lite = 1; break;
     case 'M': numM = atoi(optarg); break;
     case 'S': numS = atoi(optarg); break;
     case 'D': strcpy(module,optarg); break;
     case 'P': strcpy(prefix,optarg); break;
     case 'O': strcpy(file,optarg); break;
     case 'g': verbose = atoi(optarg); break;
     case 'v': print_version(); exit(0); break;
     case 'l': print_license(); exit(0); break;
     case 'h':
     case '?': help(argc, argv); exit(0); break;
     case  0 : //if (longopts[longidx].flag) printf("%d:%s\n", opt, longopts[longidx].name);
               //if (longopts[longidx].flag!=0) break;
               //strcpy(file,optarg);
               break;
     default: 
        fprintf(stderr, "undefined option: %c\n", optopt);
        help(argc, argv);
        exit(1);
     }
  }
  if (prefix[0]!='\0') {
      if ((prefix[0]<'A')||(prefix[0]>'z')||
          ((prefix[0]>'Z')&&(prefix[0]<'a'))) {
          fprintf(stderr, "prefix should start with alphabet, not %c.\n", prefix[0]);
          return 1;
      }
  }
  if ((lite==1)&&(numM!=1)) {
       fprintf(stderr, "Warning: num of master should be 1 for AMBA AHB-Lite\n");
       numM = 1;
  }
  if ((lite!=1)&&(numM==1)) {
       lite = 1;
  }
  if (file[0]!='\0') {
      #ifdef WIN32
      fo = fopen(file, "wb");
      #else
      fo = fopen(file, "w");
      #endif
      if (fo==NULL) {
          fprintf(stderr, "file open error\n");
          return 1;
      }
  } else {
    fo = stdout;
  }
  if (module[0]=='\0') {
      if (lite) sprintf(module, "ahb_lite_s%d", numS);
      else      sprintf(module, "amba_ahb_m%ds%d", numM, numS);
  }
  return 0;
}
#undef XXTX

//--------------------------------------------------------
void help(int argc, char **argv)
{
  fprintf(stderr, "[Usage] %s [options]\n", argv[0]);
  fprintf(stderr, "\t-T,--lite         AMBA AHB-lite\n");
  fprintf(stderr, "\t-M,--master=num   num of masters (default: %u)\n", numM);
  fprintf(stderr, "\t-S,--slave=num    num of slaves  (default: %u)\n", numS);
  fprintf(stderr, "\t-D,--module=str   module name (default: \"amba_ahb_mXsY\")\n");
  fprintf(stderr, "\t-P,--prefix=str   prefix of module\n");
  fprintf(stderr, "\t-O,--output=file  output file name (stdout if not given)\n");
  fprintf(stderr, "\t-g,--verbose=num  verbose level  (default: %d)\n", verbose);
  fprintf(stderr, "\t-v,--version      print version\n");
  fprintf(stderr, "\t-l,--license      print license message\n");
  fprintf(stderr, "\t-h                print help message\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "\tnote: amba_ahb_lite_s? for -M 1\n");
}

//--------------------------------------------------------
const char license[] =
"Copyright 2018 Ando Ki (andoki@gmail.com)\n\n\
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:\n\
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.\n\
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.\n\
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n";
//--------------------------------------------------------
void print_license(void)
{
     printf("%s %X\n\n", program, version);
     printf("%s", license);
}

//--------------------------------------------------------
void print_version(void)
{
     printf("%X\n", version);
}

//--------------------------------------------------------
// Revision history
//
// 2021.07.10: option changed, '--version/-v' added.
// 2018.07.19: '--lic' added by Ando Ki.
// 2017.09.03: '--ver' added by Ando Ki.
//             '-m' to '-d' to keep consistency
// 2017.04.08: Updated by Ando Ki.
// 2016.03.26: Startd by Ando Ki.
//--------------------------------------------------------
