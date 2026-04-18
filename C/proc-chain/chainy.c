#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum { MAX_ARGS_COUNT = 256, MAX_CHAIN_LINKS_COUNT = 256, BUFFER_SIZE = 256 };

typedef struct {
  char* command;
  uint64_t argc;
  char* argv[MAX_ARGS_COUNT];
} chain_link_t;

typedef struct {
  uint64_t chain_links_count;
  chain_link_t chain_links[MAX_CHAIN_LINKS_COUNT];
} chain_t;

void create_chain(char* command, chain_t* chain) {
  char *command_string, *pipe_string, *pipe_token, *argument;
  char *saveptr1, *saveptr2;

  chain_link_t* chain_link = chain->chain_links;
  for (chain->chain_links_count = 0, command_string = command;;
       ++chain_link, command_string = NULL) {
    pipe_token = strtok_r(command_string, "|", &saveptr1);
    if (pipe_token == NULL) {
      break;
    }

    ++chain->chain_links_count;
    char** current_argument = &chain_link->command;

    for (chain_link->argc = 0, pipe_string = pipe_token;; pipe_string = NULL) {
      argument = strtok_r(pipe_string, " ", &saveptr2);
      if (argument == NULL) {
        break;
      }

      ++chain_link->argc;

      *current_argument = strdup(argument);

      if (current_argument == &chain_link->command) {
        chain_link->argv[0] = strdup(chain_link->command);
        current_argument = chain_link->argv + 1;
      } else {
        ++current_argument;
      }
    }

    if (chain_link->argc > 0) {
      chain_link->argv[chain_link->argc] = NULL;
    }
  }
}

void run_chain(chain_t* chain) {
  uint64_t link_index;

  int previous_pipe[2] = {-1, -1};
  int current_pipe[2] = {-1, -1};
  for (link_index = 0; link_index < chain->chain_links_count; ++link_index) {
    if (pipe(current_pipe) == -1) {
      return;
    }

    if (fork() == 0) {
      if (previous_pipe[0] != -1) {
        close(previous_pipe[1]);
        dup2(previous_pipe[0], STDIN_FILENO);
      }

      close(current_pipe[0]);
      dup2(current_pipe[1], STDOUT_FILENO);

      if (execvp(chain->chain_links[link_index].command,
                 chain->chain_links[link_index].argv) == -1) {
        exit(1);
      }
    }

    if (previous_pipe[0] != -1) {
      close(previous_pipe[0]);
      close(previous_pipe[1]);
    }

    previous_pipe[0] = current_pipe[0];
    previous_pipe[1] = current_pipe[1];
  }

  close(current_pipe[1]);

  char buffer[BUFFER_SIZE];

  int read_count;
  while ((read_count = read(current_pipe[0], buffer, BUFFER_SIZE)) >= 1) {
    write(STDOUT_FILENO, buffer, read_count);
  }

  close(current_pipe[0]);
}

int main(int argc, char* argv[]) {
  chain_t exec_chain;

  if (argc != 2) {
    return -1;
  }

  create_chain(argv[1], &exec_chain);
  run_chain(&exec_chain);

  for (size_t link_index = 0; link_index < exec_chain.chain_links_count;
       ++link_index) {
    free(exec_chain.chain_links[link_index].command);

    while (exec_chain.chain_links[link_index].argc > 0) {
      free(exec_chain.chain_links[link_index]
               .argv[exec_chain.chain_links[link_index].argc - 1]);

      --exec_chain.chain_links[link_index].argc;
    }
  }

  return 0;
}
