/*
 Phil Tornquist - 701526283
 Sean Moir      - 701411108
 CISC361-010
 mbox.c
*/

#include "mbox.h"

int mbox_create(mbox **mb)
{
	*mb = malloc(sizeof(mbox));
	(*mb)->msg = NULL;
	return sem_init(&(*mb)->mbox_sem, 1);
}

void mbox_destroy(mbox **mb)
{
	sem_destroy(&(*mb)->mbox_sem);
	free(*mb);
}

void mbox_deposit(mbox *mb, char *msg, int len)
{
	
}

void mbox_block_deposit(mbox *mb, char *msg, int len)
{

}

void mbox_withdraw(mbox *mb, char *msg, int *len)
{

}

void mbox_tid_withdraw(mbox *mb, int *tid, char *msg, int *len)
{

}

void mbox_block_withdraw(mbox *mb, int *tid, char *msg, int *len)
{

}