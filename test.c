#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


int main() {
  long nprocs = -1;
  long nprocs_max = -1;
#ifdef _WIN32
#ifndef _SC_NPROCESSORS_ONLN
SYSTEM_INFO info;
GetSystemInfo(&info);

#define sysconf(a) info.dwNumberOfProcessors
#define _SC_NPROCESSORS_ONLN
#endif
#endif
#ifdef _SC_NPROCESSORS_ONLN
  nprocs = sysconf(_SC_NPROCESSORS_ONLN);
  if (nprocs < 1)
  {
    fprintf(stderr, "Could not determine number of CPUs online:\n%s\n", 
strerror (errno));
    exit (EXIT_FAILURE);
  }
  nprocs_max = sysconf(_SC_NPROCESSORS_CONF);
  if (nprocs_max < 1)
  {
    fprintf(stderr, "Could not determine number of CPUs configured:\n%s\n", 
strerror (errno));
    exit (EXIT_FAILURE);
  }
  printf ("%ld of %ld processors online\n",nprocs, nprocs_max);
  exit (EXIT_SUCCESS);
#else
  fprintf(stderr, "Could not determine number of CPUs");
  exit (EXIT_FAILURE);
#endif
}
