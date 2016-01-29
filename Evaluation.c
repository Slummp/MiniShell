#include "Shell.h"
#include "Evaluation.h"
#include "Commandes_Internes.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int
evaluer_expr(Expression *e)
{
    int statusSequence;
    int p[2];
    int pid;

    switch (e->type)
    {
        //**********Commandes Internes**********//
        case SIMPLE:
            // ECHO
            if (strcmp(e->arguments[0],"echo") == 0)
            {
                echo(e->arguments);
            }

            // DATE
            else if (strcmp(e->arguments[0],"date") == 0)
            {
                date(e->arguments);
            }

            // CD
            else if (strcmp(e->arguments[0],"cd") == 0)
            {
                cd(e->arguments);
            }

            // PWD
            else if (strcmp(e->arguments[0],"pwd") == 0)
            {
                pwd(e->arguments);
            }

            // HISTORY
            else if (strcmp(e->arguments[0],"history") == 0)
            {
                history(e->arguments);
            }

            // HOSTNAME
            else if (strcmp(e->arguments[0],"hostname") == 0)
            {
                hostname(e->arguments);
            }

            // KILL
            else if (strcmp(e->arguments[0],"kill") == 0)
            {
                kill(e->arguments[1],e->arguments[2]);
            }

            // EXIT
            else if (strcmp(e->arguments[0],"exit") == 0)
            {
                exit(0);
            }

            //**********Commandes Externes**********//
            else if (strcmp(e->arguments[0],"remote") == 0)
            {
                remote(e->arguments);
            }

            //**********Remote shell**********//
            else
            {
                pid = fork();
                if (pid==0)
                {
                    execvp(e->arguments[0],e->arguments);
                    return 0;
                }
                else
                {
                    wait(NULL);
                }
            }
            break;

        //**********Exressions**********//
        // expression &
        case BG:
            pid = fork();
            if (pid==0)
            {
                execvp(e->arguments[0],e->arguments);
                return 0;
            }
            break;

        // expression ; expression
        case SEQUENCE:
            evaluer_expr(e->gauche);
            evaluer_expr(e->droite);
            break;

        // expression || expression
        case SEQUENCE_OU:
            statusSequence = evaluer_expr(e->gauche);
            if (statusSequence==0)
                evaluer_expr(e->droite);
            break;

        // expression && expression
        case SEQUENCE_ET:
            statusSequence = evaluer_expr(e->gauche);
            if (statusSequence==1)
                evaluer_expr(e->droite);
            else
                return 0;
            break;

        // (expression)
        case SOUS_SHELL:
            break;

        // expression | expression
        case PIPE:
            pipe(p);
            pid = fork();
            if (pid==0)
            {
                dup2(p[0],0);
                printf("\n");
                //evaluer_expr(e->droite);
                execvp(e->droite->arguments[0],e->droite->arguments);
            }
            else {
                //close(p[0]);
                //close(p[1]);
                //evaluer_expr(e->gauche);
                //wait();
                int pid2 = fork();
                if (pid2==0)
                {
                    dup2(p[1], 1);
                    execvp(e->gauche->arguments[0],e->gauche->arguments);
                    printf("`\n");
                    //evaluer_expr(e->gauche);
                }
                else
                {
                    wait(NULL);
                }
            }
            break;

        // expression > fichier
        case REDIRECTION_O:
            pid=fork();
            if (pid==0)
            {
                int fd = open(e->arguments[0], O_WRONLY | O_CREAT, 0666);
                dup2(fd,1);
                execvp(e->gauche->arguments[0],e->gauche->arguments);
                close(fd);
            }
            else
            {
                wait(NULL);
            }
            break;

        // expression 2> fichier
        case REDIRECTION_E:
            pid=fork();
            if (pid==0)
            {
                int fd = open(e->arguments[0], O_WRONLY | O_CREAT, 0666);
                dup2(fd,2);
                execvp(e->gauche->arguments[0],e->gauche->arguments);
                close(fd);
            }
            else
            {
                wait(NULL);
            }
            break;

        // expression &> fichier
        case REDIRECTION_EO:
            pid=fork();
            if (pid==0)
            {
                int fd = open(e->arguments[0], O_WRONLY | O_CREAT, 0666);
                dup2(fd,1);
                dup2(fd,2);
                execvp(e->gauche->arguments[0],e->gauche->arguments);
                close(fd);
            }
            else
            {
                wait(NULL);
            }
            break;

        // expression < fichier
        case REDIRECTION_I:
            pid=fork();
            if (pid==0)
            {
                printf("%s",e->arguments[0]);
                int fd = open(e->arguments[0], O_RDONLY);
                dup2(fd,0);
                execvp(e->gauche->arguments[0],e->gauche->arguments);
                close(fd);
            }
            else
            {
                wait(NULL);
            }
            break;

        // expression >> fichier
        case REDIRECTION_A:
            pid=fork();
            if (pid==0)
            {
                int fd = open(e->arguments[0], O_WRONLY | O_CREAT, 0666);
                lseek(fd,0, SEEK_END);
                dup2(fd,1);
                execvp(e->gauche->arguments[0],e->gauche->arguments);
                close(fd);
            }
            else
            {
                wait(NULL);
            }
            break;

        // Sinon...
        default:
            break;
    }

    return 1;
}
