#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TOK_DELIM " \t\r\n"
#define RED "\033[0;31m"
#define RESET "\e[0m"
#define TK_BUFF_SIZE 1024

char *read_line();
char **split_line(char *);
int dash_exit(void);
int dash_execute(char **);
/**
 * Executing commands
 * System calls execvp fork
 */
int dash_execute(char ** args)
{
	pid_t cpid;
	int status;
	if (strcmp(args[0], "exit") == 0)
	{
		return (dash_exit());
	}
	cpid = fork();
	if (cpid == 0)
	{
		if (execvp(args[0], args) < 0)
		{
			printf("dash: command not found: %s\n", args[0]);
		}
		exit(EXIT_FAILURE);
	}
	else if (cpid < 0)
	{
		printf(RED "Error forking" RESET "\n");
	}
	else
	{
		waitpid(cpid, &status, WUNTRACED);
	}
	return (1);
}
/**
 * Exiting the shell
 * Return: Value 0 triggers successful program exit
 */
int dash_exit(void)
{
	return (0);
}
/**
 * Tokenizing Input
 * Split string with respect to some delimiters
 * First call: str1 = strtok(string_to_be_tokenized, delimiter)
 * Subsequent calls: strtok(NULL, delimiter)
 */
char **split_lines(char *line)
{
	int buffsize = 1024, position = 0;
	char **tokens = malloc(sizeof(char*) * buffsize);
	char *token;
	
	if (!tokens)
	{
		fprintf(stderr, "%sdash: Allocation errors%s\n", RED, RESET);
		exit(EXIT_FAILURE);
	}
	token = strtok(line, TOK_DELIM);
	while (token)
	{
		tokens[position] = token;
		position++;
		if (position >= buffsize)
		{
			buffsize += TK_BUFF_SIZE;
			tokens = realloc(tokens, sizeof(char *) * buffsize);
			if (!tokens)
			{
				fprintf(stderr, "%sdash: Allocation errors%s\n", RED, RESET);
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, TOK_DELIM);
	}
	tokens[position] = NULL;
	return (tokens);
}
/**
 * Reading user commands
 * read_line - get command keyed in by user
 * 
 * buffsize - initialized value of 1024 bytes
 * buffer - we use malloc to allocate space because we cannot determine before hand the
 * length of the command string entered by user.Its the most logical way to allocate more memory
 * and when required
 * while(1) {} - starts off an infinite loop
 * c - stores the char value returned by function getchar()
 * To stop the loop, we check for EOF and \n
 * Common error checks - memory allocation
 */
char *read_line(void)
{
	int buffsize = 1024, position = 0, c;
	char *buffer = malloc(sizeof(char) * buffsize);

	if (!buffer)
	{
		fprintf(stderr, "%sdash: Allocation errors%s\n", RED, RESET);
		exit(EXIT_FAILURE);
	}
	while (1)
	{
		c = getchar();
		if (c == EOF || c == '\n')
		{
			//printf("\n");
			buffer[position] = '\0';
			return (buffer);
		}
		else
			buffer[position] = c;
		position++;
		if (position >= buffsize)
		{
			buffsize += 1024;
			buffer = realloc(buffer, buffsize);
			if (!buffer)
			{
				fprintf(stderr, "dash: Allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

/**
 * prompt - lets the user know that the terminal is ready to accept commands from them.
 * e.g (~$ ).The symbol and a space after it.
 */
/**
 * loop - the loop
 *
 * line -char pointer.Hold the commad string entered by the user and retrieved using the
 * read_line()
 * @args- 2d char pointer
 * status - stores the return value of functions invoked during command execution.It
 * will determine the termination of loop. If user enters 0, the command execution process
 * returns a value of 0 which terminates the loop and the shell
 * free() -??Why are we freeing line and args yet we havent dynamically allocated these variables??
 */
void loop(void)
{
	char *line;
	char **args;
	int status = 1;

	do
	{
		printf("> ");
		line  = read_line();
		args = split_lines(line);
		status = dash_execute(args);
		free(line);
		free(args);
	} while (status);
}
int main(void)
{
	loop();
	return (0);
}
