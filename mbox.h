/*
 Phil Tornquist - 701526283
 Sean Moir      - 701411108
 CISC361-010
 mbox.h
*/

#ifndef MBOX_H
#define MBOX_H

#include "types.h"

struct messageNode{
	char *message;     // copy of the message 
	int  len;          // length of the message 
	int  sender;       // TID of sender thread 
	int  receiver;     // TID of receiver thread 
	messageNode *next; // pointer to next node 
	sem_t *blocked;
};

struct receiveBlock {
	sem_t *block;
	int sender;
	int *len;
	int *tid;
	char *msg;
	receiveBlock *next;
};

struct mbox{
	messageNode  *msg;       // message queue
	receiveBlock *rcv;
	int id;
	sem_t               *mbox_sem;
};

int mbox_create(mbox **mb);
void mbox_destroy(mbox **mb);
void mbox_deposit(mbox *mb, char *msg, int len);
void mbox_block_deposit(mbox *mb, char *msg, int len);
void mbox_withdraw(mbox *mb, char *msg, int *len);
void mbox_tid_withdraw(mbox *mb, int *tid, char *msg, int *len);
void mbox_block_withdraw(mbox *mb, int *tid, char *msg, int *len);

#endif /* MBOX_H */
