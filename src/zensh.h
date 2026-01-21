#ifndef ZENSH_H
#define ZENSH_H

#pragma once


#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

int tokenize(const char *input, char **argv, int arg_max);
int autocomplete(char *buffer, size_t bufsize, size_t *cursor);

int exec_pipeline(char **argv);

char **zensh_completion(const char *text, int start, int end);
char *command_generator(const char *text, int state);

int cd(char *args);
int pwd(char *args);
void type(char *string);
void history();
void exec(char **argv);

int redir(char **argv);

void clear_screen(void);

#endif /* ZENSH_H */
