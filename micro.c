
#include "micro.h"

void print_error(const char *str)
{
	int i = 0;
	while (str[i])
		write(2, &str[i++], 1);
}

void free_double_pointer(char **ptr)
{
	int i;

	if (!ptr)
		return;
	i = -1;
	while (ptr[++i])
		free(ptr[i]);
	free(ptr);
}

void cd(const char *path)
{
	if (!path)
		return (print_error("error: cd: bad arguments\n"));
	if (chdir(path) == -1)
	{
		print_error("error: cd: cannot change ");
		print_error(path);
		print_error("\n");
	}
}

char *ft_strdup(const char *str)
{
	int i;
	char *cp;

	if (!str)
		return (NULL);
	i = -1;
	while (str[++i])
		;
	;
	cp = (char *)malloc(sizeof(char) * (i + 1));
	if (!cp)
		return (NULL);
	i = -1;
	while (str[++i])
		cp[i] = str[i];
	cp[i] = 0;
	return (cp);
}

char **get_cmd(const char **argv, int *start)
{
	char **cmd;
	int i;
	int index;
	int count;

	if (!argv && !argv[*start])
		return (NULL);
	i = *start;
	while (argv[i])
	{
		if (!strcmp(argv[i], "|") || !strcmp(argv[i], ";"))
			break;
		i++;
	}
	count = i - *start;
	if (count == 0)
		return (NULL);
	cmd = (char **)malloc(sizeof(char *) * (count + 1));
	if (!cmd)
		return (NULL);
	i = *start;
	index = 0;
	while (argv[i])
	{
		if (!strcmp(argv[i], "|") || !strcmp(argv[i], ";"))
			break;
		cmd[index++] = ft_strdup(argv[i++]);
	}
	cmd[index] = 0;
	*start = i;
	return (cmd);
}

void exe(char **cmd, int fd[2], int it_s_pipe, char *const *env)
{
	pid_t pid;

	pid = fork();
	if (pid == -1)
		return (print_error("error fatal\n"));
	if (pid == 0)
	{
		close(fd[0]);
		if (it_s_pipe == 1)
			dup2(fd[1], 1);
		if (execve(cmd[0], cmd, env) == -1)
			return (print_error("EROOR"), exit(0));
		close(fd[1]);
	}
	else
	{
		wait(NULL);
		if (it_s_pipe == 1)
			dup2(fd[0], 0);
		close(fd[1]);
	}
}

int main(int argc, const char **argv, char *const *env)
{
	char **cmd;
	int i;
	int fd[2];

	if (argc > 1)
	{
		if (pipe(fd) == -1)
			return (print_error("error: fatal\n"), 0);
		i = 1;
		while (i < argc)
		{
			cmd = NULL;
			if (!strcmp(argv[i], "cd"))
			{
				i++;
				cd(argv[i]);
			}
			else
			{
				cmd = get_cmd(argv, &i);
				if (argv[i] && !strcmp(argv[i], "|"))
					exe(cmd, fd, 1, env);
				else
					exe(cmd, fd, 0, env);
				free_double_pointer(cmd);
			}
			i++;
		}
		close(fd[0]);
		close(fd[1]);
	}
	return (0);
}