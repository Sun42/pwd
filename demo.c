#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "my.h"
#include <sys/types.h>
#ifdef HAVE_NOT_DIRENT_H
# include <sys/dir.h>
#else
# include <dirent.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _MSC_VER /* _WIN32 */
#include <unistd.h>
#endif

#ifndef HAVE_GETCWD

#defineISDOT(dp) \
  (dp->d_name[0] == '.' && (dp->d_name[1] == '\0' || \
			    (dp->d_name[1] == '.' && dp->d_name[2] == '\0')))

#ifndef dirfd
#define   dirfd(dirp)     ((dirp)->dd_fd)
#endif


char *getcwd(char *pt, size_t size);


int	main(int ac, char **av)
{
  char	*pt;
  my_putstr(getcwd(pt, 1024)),

  return (0);
}

char *getcwd(char *pt, size_t size)
{
  #ifdef HAVE_NOT_DIRENT_H
  struct direct	*s_rep;
  # elif
  struct dirent *s_rep;
  #endif
  
  DIR *dir;
  dev_t dev;
  ino_t ino;
  int first;
  char *bpt, *bup;
  struct stat s;
  dev_t root_dev;
  ino_t root_ino;
  size_t ptsize, upsize;
  int ret, save_errno;
  char *ept, *eup, *up;

  /*
   * If no buffer specified by the user, allocate one as necessary.
   * If a buffer is specified, the size has to be non-zero.  The path
   * is built from the end of the buffer backwards.
   */
  if (pt)
    {
      ptsize = 0;
      if (!size)
	return (NULL);
      if (size == 1)
	return (NULL);
      ept = pt + size;
    }
  else 
    {
      if ((ret = __os_malloc(ptsize = 1024 - 4, NULL, &pt)) != 0) 
	return (NULL);
      ept = pt + ptsize;
    }
  bpt = ept - 1;
  *bpt = '\0';
  
  /*
   * Allocate bytes (1024 - malloc space) for the string of "../"'s.
   * Should always be enough (it's 340 levels).  If it's not, allocate
   * as necessary.  Special case the first stat, it's ".", not "..".
   */
  if ((ret = __os_malloc(upsize = 1024 - 4, NULL, &up)) != 0)
    goto err;
  eup = up + 1024;
  bup = up;
  up[0] = '.';
  up[1] = '\0';
  
  /* Save root values, so know when to stop. */
  if (stat("/", &s))
    return (NULL);
  root_dev = s.st_dev;
  root_ino = s.st_ino;
  for (first = 1;; first = 0)
    {
    /* Stat the current level. */
    if (lstat(up, &s))
      return (NULL);
    
    /* Save current node values. */
    ino = s.st_ino;
    dev = s.st_dev;

    /* Check for reaching root. */
    if (root_dev == dev && root_ino == ino) 
      {
	*--bpt = PATH_SEPARATOR[0];
	/*
	 * It's unclear that it's a requirement to copy the
       * path to the beginning of the buffer, but it's always
       * been that way and stuff would probably break.
       */
	bcopy(bpt, pt, ept - bpt);
      __os_free(up, upsize);
      return (pt);
    }
    
    /*
     * Build pointer to the parent directory, allocating memory
     * as necessary.  Max length is 3 for "../", the largest
     * possible component name, plus a trailing NULL.
     */
    if (bup + 3  + MAXNAMLEN + 1 >= eup)
      {
	if (__os_realloc(upsize *= 2, NULL, &up) != 0)
	  exit(-1);
	bup = up;
	eup = up + upsize;
      }
    *bup++ = '.';
    *bup++ = '.';
    *bup = '\0';
    
    /* Open and stat parent directory. */
    if (!(dir = opendir(up)) || fstat(dirfd(dir), &s))
      return (NULL);
    
    /* Add trailing slash for next directory. */
    *bup++ = PATH_SEPARATOR[0];
    
    /*
     * If it's a mount point, have to stat each element because
     * the inode number in the directory is for the entry in the
     * parent directory, not the inode number of the mounted file.
     */
    save_errno = 0;
    if (s.st_dev == dev)
      {
	for (;;) 
	  {
	    if (!(dp = readdir(dir)))
	      return (NULL);
	    if (dp->d_fileno == ino)
	      break;
	  }
      } 
    else
      for (;;) 
	{
	  if (!(dp = readdir(dir)))
	    return (NULL);
	  if (ISDOT(dp))
	    continue;
	  bcopy(dp->d_name, bup, dp->d_namlen + 1);
	  
	  /* Save the first error for later. */
	  if (lstat(up, &s))
	    {
	      my_putstr_error("une erreur est survenue"),
	      continue;
	    }
	  if (s.st_dev == dev && s.st_ino == ino)
	    break;
	}
    
    /*
     * Check for length of the current name, preceding slash,
     * leading slash.
     */
    if (bpt - pt < dp->d_namlen + (first ? 1 : 2))
      {
	size_t len, off;
	
	if (!ptsize)
	  return (NULL);
	off = bpt - pt;
	len = ept - bpt;
	if (__os_realloc(ptsize *= 2, NULL, &pt) != 0)
	  return (NULL);
	bpt = pt + off;
	ept = pt + ptsize;
	bcopy(bpt, ept - len, len);
	bpt = ept - len;
      }
    if (!first)
      *--bpt = PATH_SEPARATOR[0];
    bpt -= dp->d_namlen;
    bcopy(dp->d_name, bpt, dp->d_namlen);
    (void)closedir(dir);
    
    /* Truncate any file name. */
    *bup = '\0';
    }
}



/*  struct stat	*s_stat; 
  
#ifdef HAVE_NOT_DIRENT_H
  struct direct		*s_rep;
#else
  struct dirent		*s_rep;
#endif

  my_putstr(rep);
  while (s_rep = readdir(rep))
    {
      if (my_strcmp(s_rep->d_name, "..") == 0)
	{
	  if ((stat( "." , s_stat)) != -1)
	    {
	      my_put_nbr(s_stat->st_ino);
	      if ((parent_rep = opendir("..")) != NULL)
		parse_rep(parent_rep);
	    }
	}
    }
}

*/
