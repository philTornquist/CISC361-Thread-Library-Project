/*
 Phil Tornquist - 701526283
 Sean Moir      - 701411108
 CISC361-010
 mbox.c
*/

#include "mbox.h"
#include "t_lib.h"

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
	mbox_tid_deposit(mb, 0, msg, len);
}

void mbox_tid_deposit(mbox *mb, int receiver, char *msg, int len)
{
	messageNode *tmp = malloc(sizeof(messageNode));
	tmp->message = malloc(sizeof(char)*len+1);
	memcpy(tmp->message, msg, len + 1);
	tmp->len = len;
	tmp->next = NULL;
	tmp->blocked = NULL;
	tmp->sender = running->thread_id;
	tmp->receiver = receiver;

	if (mb->msg == NULL) {
		mb->msg = tmp;
		return;
	}

	messageNode *end = mb->msg;
	while (end->next != NULL)
		end = end->next;
	end->next = tmp;
}

void mbox_block_deposit(mbox *mb, int receiver, char *msg, int len)
{
	mbox_tid_deposit(mb, receiver, msg, len);
	sem_init(&tmp->blocked, 0);
	sem_wait(tmp->blocked);
}

void mbox_withdraw(mbox *mb, char *msg, int *len)
{
	mbox_tid_withdraw(mb, 0, msg, len);
}

void mbox_tid_withdraw(mbox *mb, int *tid, char *msg, int *len)
{
	if (mb->msg == NULL) {
		*tid = 0;
		*len = 0;
		return;
	}
	if (*tid == 0) {
		messageNode *mbmsg = mb->msg;
		*tid = mbmsg->sender;
		strcpy(msg, mbmsg->message);
		*len = mbmsg->len;
		return;
	}

	messageNode *current = mb->msg;

	while (current != NULL) {
		if (current->sender == *tid) {
		strcpy(msg, current->message);
		*len = current->len;
		return;
		}
		current = current->next;
	}

	*tid = 0;
	*len = 0;
}

void mbox_block_withdraw(mbox *mb, int *tid, char *msg, int *len)
{

}