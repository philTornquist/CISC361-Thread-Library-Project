/*
 Phil Tornquist - 701526283
 Sean Moir      - 701411108
 CISC361-010
 mbox.c
*/

#include "mbox.h"
#include "t_lib.h"
#include <string.h>
#include <stdlib.h>

int mbox_create(mbox **mb)
{
	*mb = malloc(sizeof(mbox));
	(*mb)->msg = NULL;
	(*mb)->id = 0;
	return sem_init(&(*mb)->mbox_sem, 1);
}

void mbox_destroy(mbox **mb)
{
	sem_destroy(&(*mb)->mbox_sem);
	free(*mb);
}

void mbox_deposit_full(mbox *mb, char *msg, int len, int block)
{
	sem_wait(mb->mbox_sem);	

	receiveBlock *rb = mb->rcv;
	if (rb != NULL) 
	{

		receiveBlock **last = &mb->rcv;
		while (rb != NULL)
		{
			if (rb->sender == 0 || rb->sender == mb->id)
			{
				strcpy(rb->msg, msg);
				*rb->len = len;
				*rb->tid = running->thread_id;
				
				*last = rb->next;
	
				sem_signal(mb->mbox_sem);
	
				free(rb);
				sem_t *sp = rb->block;
				if (sp != NULL)
				{
					sem_signal(sp);
					sem_destroy(&sp);	
				}
				return;
			}
			last = &rb->next;
			rb = rb->next;		
		}
	}

	messageNode *tmp = malloc(sizeof(messageNode));
	tmp->message = malloc(sizeof(char)*len+1);
	memcpy(tmp->message, msg, len + 1);
	tmp->len = len;
	tmp->next = NULL;
	tmp->blocked = NULL;
	tmp->sender = running->thread_id;
	tmp->receiver = mb->id;

	if (block)
	{
		sem_init(&tmp->blocked, 0);
		sem_wait(tmp->blocked);
	}

	if (mb->msg == NULL) {
		mb->msg = tmp;
		sem_signal(mb->mbox_sem);
		return;
	}

	messageNode *end = mb->msg;
	while (end->next != NULL)
		end = end->next;
	end->next = tmp;
	sem_signal(mb->mbox_sem);
}

void mbox_block_deposit(mbox *mb, char *msg, int len)
{
	mbox_deposit_full(mb, msg, len, 1);
}

void mbox_deposit(mbox *mb, char *msg, int len)
{
	mbox_deposit_full(mb, msg, len, 0);
}

void mbox_withdraw_full(mbox *mb, int *tid, char *msg, int *len, int block)
{
	sem_wait(mb->mbox_sem);
	int sender = *tid;
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
	messageNode **last = &mb->rcv;
	while (current != NULL)
	{
		if (current->sender == *tid)
		{
			strcpy(msg, current->message);
			*len = current->len;
			
			*last = current->next;
			sem_signal(mb->mbox_sem);

			sem_t *sp = current->blocked;
			free(current->message);
			free(current);

			if (sp != NULL)
			{
				sem_signal(sp);
				sem_destroy(&sp);
			}

			return;
		}
		last = &current->next;
		current = current->next;
	}
	
	if (block)
	{
		receiveBlock *rb = malloc(sizeof(receiveBlock));
		sem_init(&rb->block, 0);
		rb->sender = sender;
		rb->tid = tid;
		rb->msg = msg;
		rb->len = len;
		
		if (mb->rcv == NULL)
		{
			mb->rcv = rb;
		}
		else
		{
			receiveBlock *end = mb->rcv;
			while (end->next != NULL) end = end->next;
			end->next = rb;
		}
		sem_signal(mb->mbox_sem);

		sem_wait(rb->block);
	}
	else
	{
		*tid = 0;
		*len = 0;
		sem_signal(mb->mbox_sem);
	}
}

void mbox_withdraw(mbox *mb, char *msg, int *len)
{
	printf("Withdraw\n");
	int tid = 0;
	mbox_withdraw_full(mb, &tid, msg, len, 0);
}

void mbox_tid_withdraw(mbox *mb, int *tid, char *msg, int *len)
{
	mbox_withdraw_full(mb, tid, msg, len, 1);
}

void mbox_block_withdraw(mbox *mb, int *tid, char *msg, int *len)
{
	mbox_withdraw_full(mb, tid, msg, len, 1);
}
