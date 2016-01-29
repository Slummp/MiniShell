#include "Commandes_Internes.h"
#include <readline/history.h>


void echo(char ** argv)
{
    int i;
    for (i=1;i<LongueurListe(argv);i++)
    {
        printf("%s ",argv[i]);
    }
    printf("\n");
}

void date(char ** argv)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("%s",asctime(&tm));
}

void cd(char ** argv)
{
    chdir(argv[1]);
}

void pwd(char **argv)
{
    char cwd[1024];
    printf("%s\n",getcwd(cwd,1024));
}

void history(char **argv)
{
    HIST_ENTRY **the_history_list;
    the_history_list=history_list();
    int i;
    for (i=0;i<history_length;i++)
    {
        printf("%s\n",the_history_list[i]->line);
    }
}

void hostname(char **argv)
{
    char name[1024];
    gethostname(name,1024);
    printf("%s\n",name);
}
void remote(char **argv) {
    if (strcmp(argv[0], "add")) { //Ajout de machine
        
    }
}