#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define handle_error(msg)\
        do{perror(msg); exit(EXIT_FAILURE);} while(0)


void handler(int sig)
{

	static int count =0, sigint =0, sigquit=0;
	if(sig == SIGINT)
	{
		printf("[%d] %s n° %d catched\n",count++, strsignal(sig), sigint++);
	}

	if(sig == SIGQUIT)
	{
		printf("[%d] %s n° %d catched\n",count++, strsignal(sig), sigquit++);
	}
}


int main(int argc, char const *argv[])
{
	sigset_t blockingMask, prevMask, pendingMask;
	struct sigaction mySa;
	int i;

	printf("\nGestiamo i segnali SIGINT e SIGQUIT tramite la 'signal()'\n");
	if(signal(SIGINT, handler) == SIG_ERR)
	{
		handle_error("signal1");
	}
	if(signal(SIGQUIT, handler) == SIG_ERR)
	{
		handle_error("signal2");
	}

	for(i = 0; i<2; i++)
	{
		printf("\n\tLanciare un SIGQUIT\n");
		pause();
		printf("\n\tLanciare un SIGINT\n");
		pause();
	}

	if(signal(SIGINT, SIG_DFL) == SIG_ERR)
	{
		handle_error("signal3");
	}
	if(signal(SIGQUIT, SIG_DFL) == SIG_ERR)
	{
		handle_error("signal4");
	}
	printf("\nBene, ora sono stati riportati alla loro disposition di default\n");
	sleep(2);
    printf("\nOra blocchiamo tutti i segnali esistenti\n");

    if(sigfillset(&blockingMask) == -1)
    {
    	handle_error("sigfillset");
    }

    if(sigprocmask(SIG_BLOCK, &blockingMask, &prevMask) == -1)
    {
    	handle_error("sigprocmask");
    }

    for(i=0; i<2; i++)
    {
    	if(i ==0)
    	{
    		printf("\n\tLanciare un SIGQUIT per vedere se diverrà pending\n");
    		sleep(5);
    	}
    	else
    	{
    		printf("\n\tLanciare un SIGINT per vedere se diverrà pending\n");
    		sleep(5);
    	}

    }

    /*Inizializiamo una maschera da riempire con i signal pending*/
    if(sigemptyset(&pendingMask) == -1)
    {
    	handle_error("sigemptyset1");
    }
    if(sigpending(&pendingMask) == -1)
    {
    	handle_error("sigending");
    }

    printf("\nSegnali pending:\nSIGINT: %s\nSIGQUIT: %s\nSIGKILL: %s\n",sigismember(&pendingMask, SIGINT) ? "on":"off", 
    	        sigismember(&pendingMask, SIGQUIT) ? "on": "off", sigismember(&pendingMask, SIGKILL) ? "on": "off");
    sleep(5);
    printf("\nOra ingnoriamo quelli \"on\" per far si che non vengano spediti al processo non appena sbloccati\n");
    sleep(5);

    if(sigismember(&pendingMask, SIGINT))
    {
    	if(signal(SIGINT, SIG_IGN) == SIG_ERR)
    	{
    		handle_error("signal5");
    	}
    	printf("\n\tSIGINT: ignore\n");
    	sleep(2);
    }

    if(sigismember(&pendingMask, SIGQUIT))
    {
    	if(signal(SIGQUIT, SIG_IGN) == SIG_ERR)
    	{
    		handle_error("signal6");
    	}

    	printf("\n\tSIGQUIT: ignore\n");
    	sleep(2);
    }

    if(sigismember(&pendingMask, SIGKILL))
    {
    	if(signal(SIGKILL, SIG_IGN) == SIG_ERR)
    	{
    		handle_error("Signal sigkill");
    	}
    	printf("Se si sta vedendo questa stampa ci deve essere stato un errore, poichè SIGKILL è immutabile\n");
    }

    printf("\n\nOra sblocchiamo i segnali e riassegnamo al processo la vecchia signal mask\n");
    if(sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
    {
    	handle_error("sigprocmask2");
    }

    sleep(4);

    printf("\n\nOra dovrebbero essere stati inviati i segnali pending e ignorati,\n....quindi dovremmo poter riportare la loro disposition a Default senza che terminino il processo\n");
    sleep(8);

    if(signal(SIGINT, SIG_DFL) == SIG_ERR)
    {
    	handle_error("signal7");
    }
    if(signal(SIGQUIT, SIG_DFL) == SIG_ERR)
    {
    	handle_error("signal8");
    }

    printf("\n......Fatto\n");
    sleep(4);
    printf("\n.....Inizializiamo la struct sigaction...\n");

    sigemptyset(&mySa.sa_mask);
    mySa.sa_handler = handler;
    mySa.sa_flags = 0;

    sleep(4);

    if(sigaction(SIGINT, &mySa, NULL) == -1)
    {
    	handle_error("sigaction1");
    }
    if(sigaction(SIGQUIT, &mySa, NULL) == -1)
    {
    	handle_error("sigaction2");
    }

    printf("\nDa ora SIGQUIT e SIGINT saranno gestiti con la 'sigaction()'\n");
    printf("\nDare il comando 'kill -9 %ld' su un altro terminale per terminare...\n", (long)getpid());

    for(;;)
    {
    	;
    }	



}
