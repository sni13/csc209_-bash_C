/* ls | tr a b > text.txt */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int spawn_proc (int in, int out, char** cmd) {
    pid_t pid;

    printf("in: %d out: %d cmd: %s\n", in, out, cmd[0]);

    if ((pid = fork ()) == 0) {
        if (in != 0) {
          dup2 (in, 0);
          close (in);
        }
        if (out != 1) {
          dup2 (out, 1);
          close (out);
        }

      return execvp (cmd[0], cmd);
    }

    return pid;
}

void fork_pipes (char** cmd[], const char *redirection) {
  int i, n;
  int in, out, fd[2];

  in = 0;

  // obtain n from the NULL terminated cmd array
  for (n = 0; cmd[n]; ++n)
    ;

  // process all but the last elemet of the pipe
    for (i = 0; i < n-1; ++i) {
        pipe(fd);
        spawn_proc(in, fd[1], cmd[i]);
        close(fd [1]);
        in = fd [0];
  }

  // process the last element of the pipe
    if (redirection) {
        out = open(redirection, O_WRONLY | O_CREAT | O_TRUNC);
        fchmod(out, 0666);
    } else
        out = STDOUT_FILENO;

    if (in != 0)
        dup2(in, 0);

    spawn_proc(in, out, cmd[i]);
}

int main()
{

    char *cmd1[] = {"ls", NULL};
    char *cmd2[] = {"tr", "e", "f", NULL};
    char **cmd[] = { cmd1, cmd2, NULL};

    // redirected to text.txt
    fork_pipes(cmd, "text.txt");

    // no redirection
    fork_pipes(cmd, NULL);

    // another example with a longer pipe 
    {
        char *cmd1[] = {"echo", "hello world", NULL};
        char *cmd2[] = {"tee", NULL};
        char *cmd3[] = {"tee", NULL};
        char *cmd4[] = {"tr", "lo", "10", NULL};

        char **cmd[] = {cmd1, cmd2, cmd3, cmd4, NULL};

        // redirected to redirection.txt
        fork_pipes(cmd, "redirection.txt");

        // no redirected
        fork_pipes(cmd, NULL);
    }

    return 0;
}