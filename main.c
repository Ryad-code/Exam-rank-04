#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#define FALSE 0
#define TRUE 1

int next_occur(char *c, char **tab,  int index)
{
    while (tab && tab[index] && strcmp(c, tab[index]))
        index++;
    return (index);
}

char **sub_tab(char **tab, int begin, int end)
{
    char **sub;
    int i = 0;

    if (!(sub = (char **)malloc(sizeof(char *) * (end - begin + 1))))
        return (NULL);
    while (begin < end)
        sub[i++] = tab[begin++];
    sub[i] = 0;
    return (sub);
}

int nb_segments(char *c, char **tab)
{
    int i = 0;
    int count = 1;
    if (!tab || !tab[0])
        return 0;
    while (tab[i])
    {
        if (!strcmp(tab[i++], c))
            count++;
    }
    return count;
}
//.....................................................................MAIN PATRICK
int main(int ac , char **av, char **env)
{
	char	**cmds;
	char	**tab;
	int	last_pipe = -1, j = 0, nb_cmd = 0;

	if (ac < 2)
		return (1);
	nb_cmd = nb_segments("|", av + 1);
	cmds = sub_tab(av, 1, ac);
//	printf("nb_cmd = %d\n", nb_cmd);
	for (int i = 0; i < nb_segments("|", av + 1); i++)
	{
		j = 0;
//		printf("next = %d\n", next_occur("|", av, last_pipe + 1));
		tab = sub_tab(cmds, last_pipe + 1, next_occur("|", cmds, last_pipe + 1))	;
		while (tab[j])
		{
			printf("tab[%d] = %s\n", j, tab[j]);
			j++;
		}
	free(tab);
	tab = NULL;
	last_pipe = next_occur("|", cmds, last_pipe + 1);
	}
	return (0);
}
