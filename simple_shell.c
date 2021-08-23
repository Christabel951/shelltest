#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

/**
 * built in commands
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

//list of built in commands followed by their corresponding functions
char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};
int (*builtin_func[]) (char **) = {
	&lsh_cd,
	&lsh_help,
	&lsh_exit
};
int lsh _num_builtins()
{
	return (sizeof(builtin_str )/ sizeof(char**))
}
//Builtin function implementation
int lsh_cd(char **args)
{
	if (!args[1])
	{
		fprintf(stderr, "lsh: expected argument to \"cd\n");
	}
	else
	{
		if (chdir(args[1]) != 0)
		{
			perror("lsh");
		}
	}
	return (1);
}

int lsh_help(char **args)
{
	int i;
	printf("Stephene Brennan's LHS\n");
	printf("Type program names and arguments, and hit enter\n");
	printf("The following are built in: \n");
	for (i = 0; i < lsh_num_builtins(); i++)
	{
		printf("  %s\n", builtin_str[i]);
	}
	printf("Use the man command for information on other programs\n");
	return (1);
}
int lsh_exit(char **args)
{
	return (0);
}




/**
 * lsh_launch - execute commands
 */
int lsh_launch(char **args)
{
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0)
	{
		//Child process
		if (execvp(args[0], args) == -1)
		{
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		//Error forking
		perror("lsh");
	}
	else
	{
		//parent process
		do
		{
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return (1);
}
/**
 * lsh_execute - launch a built in or process
 */
int lsh_execute(char **args)
{
	int i;
	if (!args[0])
	{
		//an empty command was entered
		return (1);
	}
	for (i = 0; i < lsh_num_builtins(); i++)
	{
		if (strcmp(args[0], builtin_str[i]) == 0)
		{
			return ((*builtin_func[i])(args));
		}
	}
	return (lsh_launch(args));
}
/**
 * lsh_split_line - break string into a seies of tokens using a delimiter
 *
 */
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM "\t\r\n\a"
char **lsh_split_line(char *line)
{
        int bufsize = LSH_TOK_BUFSIZE, position = 0;
        char **tokens = malloc(sizeof(char) * bufsize);
        char *token;

        if (!tokens)
        {
                fprintf(stderr, "lsh: allocation error");
                exit(EXIT_FAILURE);
        }
        token = strtok(line, LSH_TOK_DELIM);
        while (token)
        {
                tokens[position] = token;
                position++;

                if (position >= bufsize)
                {
                        bufsize += LSH_TOK_BUFSIZE;
                        tokens = realloc(tokens, sizeof(char) * bufsize);
                        if (!tokens)
                        {
                                fprintf(stderr, "lsh: allocation error\n");
                                exit(EXIT_FAILURE);
                        }
                }
                token = strtok(NULL, LSH_TOK_DELIM);
        }
        tokens[position] = NULL;
        return (tokens);
}
/**
 * lsh_read_line - read a line from stdin
 *
 * Description: Allocate using a standard memory block of size 1024
 * If the size exceeds then reallocate with more space
 * Return: string
 */
#define LSH_RL_BUFSIZE 1024
char *lsh_read_line(void)
{
	int bufsize = LSH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

	if (!buffer)
	{
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}
	while (1)
	{
		//Read a character
		c = getchar();

		//if we hit an EOF, replace it with a null character and return
		if (c == EOF || X == '\n')
		{
			buffer[position] = '\0';
			return (buffer);
		}
		else
		{
			buffer[position] = c;
		}
		position++;
		//if we have exceeded the buffer, reallocate
		if (position >= bufsize)
		{
			bufsize += LSH_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer)
			{
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}
/**
 * lsh_loop - prints the prompt, read the commads from the commandline 
 * and executes the commands as long as the status is true
 */
void lsh_loop(void)
{
	char *line;
	char **args;
	int status;
	do
	{
		printf("($)");
		line = lsh_read_line();
		args = lsh_split_line(line);
		status = lsh_execute(args);

		free(line);
		free(args);
	}while (status);
}
/**
 * main - read command from stdin and execute
 * argc: number of arguments including command name
 * argv: array of strings - arguments
 * Return: EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	//Initialize Load config files
	
	//Interpret: Run command loop
	lsh_loop();

	//Perform any shutdown/ cleanup
	
	return (EXIT_SUCCESS);
}
