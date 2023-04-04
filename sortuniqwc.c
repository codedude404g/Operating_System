#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

const int READ_END = 0;
const int WRITE_END = 1;

// file descriptiors for pipes
int pipe_file_descriptior1[2], pipe_file_descriptior2[2];
pid_t pid1, pid2, pid3; // child processes' ids

int main(void)
{
  // create first pipe fd1
  if (pipe(pipe_file_descriptior1) == -1)
  {
    perror("pipe1");
    exit(1); // Fails
  }

  // fork first child
  pid1 = fork();
  if (pid1 == -1)
  {
    perror("fork1");
    exit(EXIT_FAILURE);
  }
  else if (pid1 == 0)
  {
    // tie write end of pipe fd1 to standard output (file descriptor 1)
    // because we want to write the output of the sort command to write end of first pipe so the next process can read
    if (dup2(pipe_file_descriptior1[1], STDOUT_FILENO) == -1)
    {
      perror("dup2");
      exit(EXIT_FAILURE);
    }

    close(pipe_file_descriptior1[READ_END]);  // Close unused read end of pipe1
    close(pipe_file_descriptior1[WRITE_END]); // Close write end of pipe1

    execlp("sort", "sort", NULL); // first child process, run sort
    perror("execlp sort");        // should not get here
    exit(EXIT_FAILURE);
  }

  // create second pipe fd2
  if (pipe(pipe_file_descriptior2) == -1)
  {
    perror("pipe2");
    exit(EXIT_FAILURE);
  }
  // fork second child
  pid2 = fork();
  if (pid2 == -1)
  {
    perror("fork2");
    exit(EXIT_FAILURE);
  }
  else if (pid2 == 0)
  { // Child process 2
    // tie read end of fd1 to standard input (file descriptor 0)
    if (dup2(pipe_file_descriptior1[READ_END], STDIN_FILENO) == -1)
    {
      perror("dup2");
      exit(EXIT_FAILURE);
    }

    close(pipe_file_descriptior1[READ_END]);  // Close read end of pipe1
    close(pipe_file_descriptior1[WRITE_END]); // Close unused write end of pipe1
    close(pipe_file_descriptior2[READ_END]);  // Close unused read end of pipe2

    // tie write end of fd2 to standard output (file descriptor 1)
    if (dup2(pipe_file_descriptior2[WRITE_END], STDOUT_FILENO) == -1)
    {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    close(pipe_file_descriptior2[WRITE_END]); // Close write end of pipe2

    // Execute uniq command
    execlp("uniq", "uniq", NULL);
    perror("execlp uniq"); // should not get here
    exit(EXIT_FAILURE);
  }

  // fork third child
  pid3 = fork(); // create third child for wc -l
  if (pid3 == -1)
  {
    perror("fork3");
    exit(EXIT_FAILURE);
  }
  else if (pid3 == 0)
  {                                   // Child process 3
    close(pipe_file_descriptior2[1]); // Close unused write end of pipe2

    // tie read end of fd2 to standard input (file descriptor 0)
    if (dup2(pipe_file_descriptior2[READ_END], STDIN_FILENO) == -1)
    {
      perror("dup2");
      exit(EXIT_FAILURE);
    }

    close(pipe_file_descriptior1[READ_END]);  // Close unused read end of pipe1
    close(pipe_file_descriptior1[WRITE_END]); // Close unused write end of pipe1
    close(pipe_file_descriptior2[READ_END]);  // Close read end of pipe2
    close(pipe_file_descriptior2[WRITE_END]); // Close unused write end of pipe2

    // Execute wc -l command
    execlp("wc", "wc", "-l", NULL);
    // execlp("cat", "cat", NULL);
    perror("execlp wc"); // should not get here
    exit(EXIT_FAILURE);
  }

  // parent process code
  // close both end of each pipes
  close(pipe_file_descriptior1[READ_END]);  // Close unused read end of pipe1
  close(pipe_file_descriptior1[WRITE_END]); // Close unused write end of pipe1
  close(pipe_file_descriptior2[READ_END]);  // Close unused read end of pipe2
  close(pipe_file_descriptior2[WRITE_END]); // Close unused write end of pipe2

  // Wait for all child processes
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);
  waitpid(pid3, NULL, 0);

  return 0;
}
