# Operating_System
concurrent programming using the POSIX standard API.
A C program which does what the Linux shell does, without doing the command
line processing. wrote a program that creates three child processes, one for each
command. Each child process will tie an end of a pipe to the standard input, standard output, or both,
and then call the execlp system call to run the command. The three child processes will be
connected by two Linux pipes, one pipe between the first two processes and the other pipe between
the second two processes.
