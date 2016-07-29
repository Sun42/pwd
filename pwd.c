#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef HAVE_NOT_DIRENT_H
# include <sys/dir.h>
#else
# include <dirent.h>
#endif
#include "my.h"


char	*my_getcwd(void);
char    **mk_str(int nb);
int	mk_ino(int ino[255]);
int	count_ino(int ino[]);
char	*get_dirname(int ino, char *str);

int	main()
{
  my_putstr(my_getcwd());
}

char	*my_getcwd(void)
{

  char	**str;
  int	ino[255];
  char	*full_path;
  char	*dname;
  int	i;

  full_path = malloc(sizeof(*full_path) * 256);
  i = mk_ino(ino);
  str = mk_str(i + 1);
  my_strcpy(full_path, "/");
  while (i > 0)
    {
      if ((dname = get_dirname(ino[i - 1], str[i])) != NULL)
	{
	  my_strcat(full_path, dname);
	  my_strcat(full_path, "/");
      }
      i--;
    } 
  return (full_path);
}

char	*get_dirname(int ino, char *str)
{
  DIR *rep;

#ifdef HAVE_NOT_DIRENT_H
  struct direct         *s_rep;
# else
  struct dirent         *s_rep;
#endif
  struct stat s_stat;
  char	*test;
  
  test = malloc(255);
  rep = opendir(str);
  while (s_rep = readdir(rep))
    {
      my_strcpy(test,str);
      my_strcat(test, "/");
      my_strcat(test,s_rep->d_name);
	
      if ((stat(test, &s_stat)) != -1)
	{
	  if (s_stat.st_ino == ino)
	    {
	      return (s_rep->d_name);
	    }
	  
	}
      /*      if (s_rep->d_ino == ino)
	      return (s_rep->d_name);*/
    }
  closedir(rep);
  return (NULL);
}

char	**mk_str(int nb)
{
  char	**str;
  int	i;

  i = 1;
  if ((str = malloc(sizeof(*str) * nb + 1)) == NULL)
    exit(-1);
  if ((str[0] = malloc(sizeof(**str) * 2)) == NULL)
    exit(-1);
  my_strcpy(str[0], ".");
  while (i < nb)
    {
      if ((str[i] = malloc(sizeof(**str) * 256)) == NULL)
	exit(-1);  
      my_strcpy(str[i], str[i- 1]);
      my_strcat(str[i], "/..");
      i++;
    }
  str[i] = 0;
  return (str);
}

int	mk_ino(int ino[255])
{
  ino_t root_ino;
  ino_t cur_ino;
  struct stat s_stat;
  char str[255];
  int	i;

  i = 0;
  my_strcpy(str, ".");
  if (stat("/", &s_stat) != 0)
    exit(-1);
  root_ino = s_stat.st_ino;
  if (lstat(str, &s_stat) != 0)
    exit(-1);
  cur_ino = s_stat.st_ino;
  ino[i] = s_stat.st_ino;
  while (root_ino != cur_ino)
    {
      i++;
      my_strcat(str, "/..");
      if (lstat(str, &s_stat) != 0)
      	  exit(-1);
      cur_ino = s_stat.st_ino;
      ino[i] = s_stat.st_ino;
    }
  return (i);
}
