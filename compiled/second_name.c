#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

char * hostname()
{
  struct addrinfo hints, *info;
  int gai_result;
  struct utsname u;


  uname(&u);
  
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_CANONNAME;
  
  if ((gai_result = getaddrinfo(u.nodename, "http", &hints, &info)) != 0) 
    {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
      exit(1);
    }

  char* my_hostname = strdup(info->ai_canonname);

  freeaddrinfo(info);

  return my_hostname;
}

const char* xalt_syshost()
{
  //char * my_hostname = hostname();
  char * my_hostname = "login1.stampede.tacc.utexas.edu";

  
    



  // Check for first "."
  char * p = strchr(my_hostname,'.');
  if (p == NULL)
    return my_hostname; // return name if there is no dotage

  // Check for second dot
  char * start = ++p;
  p = strchr(p,'.');

  if (p == NULL)
    return start; // There is no second dot

  int    len = p - start;
  char * w   = malloc(len+1);
  memcpy(w,start,len);
  w[len] = '\0';
  return w;
  
}

int main()
{
  printf("syshost: %s\n",xalt_syshost());
  return 0;
}
