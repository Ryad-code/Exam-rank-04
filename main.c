#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define READ 0
#define WRITE 1

void    print_error(char *str)
{
	if (str)
		write(2, str, strlen(str));
}

void ft_cd(char **tab)
{	
	if (!tab[1] || tab[2])
		return (print_error("error: cd: bad arguments\n"));
	if (chdir(tab[1]) == -1)
	{
		print_error("error: cd: cannot change directory to ");
		print_error(tab[1]);
		print_error("\n");
	}
}
     
int	count(char *c, char **tab)
{
	int i = 0;
	int count = 1;

	while (tab[i])
	{
		if (strcmp(c, tab[i]) == 0)
			count++;
		i++;
	}
	return (count);
}

int	get_next(char *c, char **tab, int i)
{
	while (tab && tab[i] && strcmp(c, tab[i]))
		i++;
	return (i);
}

char	**get_sub_tab(char **tab,  int begin , int end)
{
	char	**res;
	int	i = 0;

	res = malloc(sizeof(char *) * (end - begin + 1));
	while (begin < end)
	{
		res[i] = tab[begin];
		i++;
		begin++;
	}
	res[i] = 0;
	return (res);
}

void	execute(char **tab, char **env, int *fd, int last)
{
	int pipefd[2];
	pid_t pid;

	if (strcmp("cd", tab[0]) == 0)
		return (ft_cd(tab));
	if (pipe(pipefd) == -1 || (pid = fork()) == -1)
	{
		print_error("error: fatal\n");
		exit(1);
	}
	if (pid == 0)
	{
		close(pipefd[READ]);
		dup2(*fd, STDIN_FILENO);
		if (last == 0)
			dup2(pipefd[WRITE], STDOUT_FILENO);
		if (execve(tab[0], tab, env) == -1)
		{
			print_error("error: cannot execute ");
			print_error(tab[0]);
			print_error("\n");
		}
		close(pipefd[WRITE]);
	}
	else
	{
		close(pipefd[WRITE]);
		waitpid(pid, NULL, 0);
		kill(pid, SIGTERM);
		if (last == 0)
			dup2(pipefd[READ], *fd);
		close(pipefd[READ]);
	}

}

int main(int ac, char **av, char **env)
{
	char	**base;
	char	**tab;
	char	**sub_tab;
	int	curs_cmd = 0;
	int	curs_pipe = 0;
	int	fd = 0;

	if (ac < 2)
		return (1);
	base = get_sub_tab(av, 1, ac);								//Tab de base
	for (int x = 0; x < count(";", base); x++)						//Boucle cmd
	{
		tab = get_sub_tab(base, curs_cmd, get_next(";", base, curs_cmd));		//Tab cmd
		curs_pipe = 0;
		for (int y = 0; y < count("|", tab); y++)					//Boucle pipe
		{
			sub_tab = get_sub_tab(tab, curs_pipe, get_next("|", tab, curs_pipe));	//Tab pipe
			if (y + 1 < count("|", tab))
				execute(sub_tab, env, &fd, 0);					//Execute
			else
				execute(sub_tab, env, &fd, 1);					//Execute last
			curs_pipe = get_next("|", tab, curs_pipe) + 1;
			free(sub_tab);
			sub_tab = NULL;
		}
		curs_cmd = get_next(";", base, curs_cmd) + 1;
		free(tab);
		tab = NULL;
	}
	free(base);
	base = NULL;
	return (0);
}
