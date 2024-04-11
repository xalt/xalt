#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc, char* argv[])
{
  
  pid_t parent = getpid();
  printf("Hello World! start, parent: %d\n",parent);  
  pid_t pid = fork();

  if (pid == -1)
    {
      // error, failed to fork()
    } 
  else if (pid > 0)
    {
      int status;
      pid_t my_pid = getpid();
      printf("PARENT: parent: %d, my_pid: %d\n",parent, my_pid);
      waitpid(pid, &status, 0);
    }
  else 
    {
      // we are the child
      pid_t my_pid = getpid();
      printf("CHILD: parent: %d, my_pid: %d\n",parent, my_pid);
    }


  return 0;
}
