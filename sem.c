int sem_init(sem_t **sp, int sem_count)
{
	*sp = malloc(sizeof(sem_t));
	*sp->sem_count = sem_count;
}

void sem_wait(sem_t *sp)
{
	sp->sem_count--;
	if (sp->sem_count < 0)
	{
		//  If sem_count == 0 then the block_queue is empty
		if (sp->sem_count == 0)
			block_queue = running;
		else
			end_queue->next = running;

		end_queue = running;
		running = running->next;
		end_queue->next = NULL;

		swapContext(&end_queue->thread_context, &running->thread_context);
	}
}

void sem_signal(sem_t sp)
{
	sp->sem_count++;
	if (sp->sem_count <= 0)
	{
		t_queue(block_queue);
		block_queue = block_queue->next;
	}
}

void sem_destroy(sem_t **sp)
{
	free(*sp);
}