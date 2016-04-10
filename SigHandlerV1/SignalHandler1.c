#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


void handler(int sig)
{
	static int count=0, sigint=0, sigquit=0;
	if(sig == SIGINT)
	{
		printf("\n[%d] %s n° %d CATCHED!!\n",count++,strsignal(sig),sigint++);
	}

	if(sig == SIGQUIT)
	{
		printf("\n[%d] %s n° %d CATCHED\n",count++, strsignal(sig),sigquit++);
	}
}

int main(void)
{
	sigset_t my_new_mask, old_mask;
	struct sigaction my_sigaction;

	printf("Inizializiamo una maschera per ignorare SIGINT e SIGQUIT\n");

	sigemptyset(&my_new_mask);
	sigaddset(&my_new_mask, SIGINT);
	sigaddset(&my_new_mask, SIGQUIT);

    	printf("Premere Ctrl+C o Ctrl+\\ per inviare segnali che per ora saranno ignorati ma che resteranno pending.\n");

	sigprocmask(SIG_BLOCK, &my_new_mask, &old_mask);
	printf("(Non premere niente per far proseguire il programma.)\n");

	sleep(10);

	printf("\nRiportiamo la maschera a com'era precedentemente, facendogli catturare anche i signal pending\n");
	sigprocmask(SIG_SETMASK, &old_mask, NULL);

	sleep(10);

	printf("\nOra gestiamo i segnali tramite la signal()\n");
	if(signal(SIGINT,handler) == SIG_ERR)
	{
		perror("Signal SIGINT");
		exit(EXIT_FAILURE);
	}

	if(signal(SIGQUIT,handler) == SIG_ERR)
	{
		perror("Signal SIGQUIT");
		exit(EXIT_FAILURE);
	}

	printf("In attesa di due signal di tipo SIGINT o SIGQUIT......\n");

	pause();
	pause();

	printf("\nOra riportiamo i segnali alla loro disposition di default.....(provare per credere!!)\n");
	if(signal(SIGINT,SIG_DFL) == SIG_ERR)
	{
		perror("SIG_DFL INT");
		exit(EXIT_FAILURE);
	}
	if(signal(SIGQUIT,SIG_DFL) == SIG_ERR)
	{
		perror("SIG_DFL QUIT");
		exit(EXIT_FAILURE);
	}

	sleep(10);

	printf("\nOra ignoriamoli semplicemente...(provare anche qui per credere)\n");
	if(signal(SIGINT,SIG_IGN) == SIG_ERR)
	{
		perror("SIG_IGN INT");
		exit(EXIT_FAILURE);
	}
	if(signal(SIGQUIT,SIG_IGN) == SIG_ERR)
	{
		perror("SIG_IGN QUIT");
		exit(EXIT_FAILURE);
	}

	sleep(10);

	printf("\nOra riportiamoli alla loro disposition di default\n");
	if(signal(SIGINT,SIG_DFL) == SIG_ERR)
	{
		perror("SIG_DFL INT");
		exit(EXIT_FAILURE);
	}
	if(signal(SIGQUIT,SIG_DFL) == SIG_ERR)
	{
		perror("SIG_DFL QUIT");
		exit(EXIT_FAILURE);
	}

	sleep(10);

	printf("\nOra gestiamoli tramite la SigAction()\n\nPer terminare digitare \"kill -9 %d\" su un altro terminale\n",getpid());

	sigemptyset(&my_sigaction.sa_mask);
	my_sigaction.sa_flags = 0;
	my_sigaction.sa_handler = handler;

	if(sigaction(SIGINT,&my_sigaction,NULL) == -1)
	{
		perror("Sigaction INT\n");
		exit(EXIT_FAILURE);
	}
	if(sigaction(SIGQUIT,&my_sigaction,NULL) == -1)
	{
		perror("Sigaction QUIT\n");
		exit(EXIT_FAILURE);
	}
    
    for(;;)
    {
	pause();
    }
	


}
