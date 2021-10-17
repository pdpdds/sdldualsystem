#include "datadir.h"

#include "binreloc.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

bool sanity_check_datadir(char *datadir)
{
  char buf[1024];
  struct stat s;

  sprintf(buf, "%s/data/levels/1.lev", datadir);

  if(stat(buf, &s) == 0)
    {
      return (s.st_mode & S_IFMT) == S_IFREG;
    }

  return false;
}

char *setup_datadir(int argc, char *argv[])
{
  BrInitError e;
  char *datadir = 0;
  char *alternative = 0;

  if(br_init(&e) == 0)
    {
      printf("Failed to init binreloc! Error code: %d\n", e);
    }

  if(argc > 1)
    {
      alternative = argv[1];
    }

  datadir = br_find_exe_dir(alternative);

  if(!sanity_check_datadir(datadir))
    {
      // Try the alternative location
      datadir = strdup(alternative ? alternative : "");

      if(!sanity_check_datadir(datadir))
	{
	  free(datadir);

	  // Try the default location
	  datadir = strdup("/opt/retrobattle");

	  if(!sanity_check_datadir(datadir))
	    {
	      printf("ERROR: Unable to find game data!\n");
	      printf("       Try providing the path when starting, i.e.:\n");
	      printf("       # retrobattle <PATH_TO_RETROBATTLE>\n");

	      free(datadir);
	      datadir = 0;
	    }
	}
    }

  return datadir;
}
