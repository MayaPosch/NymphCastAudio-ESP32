

#include "frame_queue.h"


// --- FRAME QUEUE INIT ---
int FrameQueueC::frame_queue_init(FrameQueue *f, PacketQueue *pktq, int max_size, int keep_last) {
    int i;
#ifndef ESP_PLATFORM
    memset(f, 0, sizeof(FrameQueue));
#endif
    /* if (!(f->mutex = SDL_CreateMutex())) {
        av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
        return AVERROR(ENOMEM);
    }
    if (!(f->cond = SDL_CreateCond())) {
        av_log(NULL, AV_LOG_FATAL, "SDL_CreateCond(): %s\n", SDL_GetError());
        return AVERROR(ENOMEM);
    } */
	
	//f = new FrameQueue;
	
	f->cond = new Poco::Condition;
    f->pktq = pktq;
    f->max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
    f->keep_last = !!keep_last;
    for (i = 0; i < f->max_size; i++)
        if (!(f->queue[i].frame = av_frame_alloc()))
            return AVERROR(ENOMEM);
    return 0;
}

static void frame_queue_unref_item(Frame *vp)
{
    av_frame_unref(vp->frame);
    avsubtitle_free(&vp->sub);
}

void FrameQueueC::frame_queue_destroy(FrameQueue *f) {
    int i;
    for (i = 0; i < f->max_size; i++) {
        Frame *vp = &f->queue[i];
        frame_queue_unref_item(vp);
        av_frame_free(&vp->frame);
    }
    //SDL_DestroyMutex(f->mutex);
    //SDL_DestroyCond(f->cond);
	delete f->cond;
	//delete f;
}

void FrameQueueC::frame_queue_signal(FrameQueue *f) {
    //SDL_LockMutex(f->mutex);
	f->mutex.lock();
    //SDL_CondSignal(f->cond);
	f->cond->signal();
    //SDL_UnlockMutex(f->mutex);
	f->mutex.unlock();
}

Frame *FrameQueueC::frame_queue_peek(FrameQueue *f) {
    return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}

Frame *FrameQueueC::frame_queue_peek_next(FrameQueue *f) {
    return &f->queue[(f->rindex + f->rindex_shown + 1) % f->max_size];
}

Frame *FrameQueueC::frame_queue_peek_last(FrameQueue *f) {
    return &f->queue[f->rindex];
}

Frame *FrameQueueC::frame_queue_peek_writable(FrameQueue *f) {
    /* wait until we have space to put a new frame */
    //SDL_LockMutex(f->mutex);
	f->mutex.lock();
    while (f->size >= f->max_size && !f->pktq->abort_request) {
        //SDL_CondWait(f->cond, f->mutex);
		f->cond->wait(f->mutex);
    }
	
    //SDL_UnlockMutex(f->mutex);
	f->mutex.unlock();

    if (f->pktq->abort_request)
        return NULL;

    return &f->queue[f->windex];
}

Frame *FrameQueueC::frame_queue_peek_readable(FrameQueue *f) {
    /* wait until we have a readable a new frame */
    //SDL_LockMutex(f->mutex);
	f->mutex.lock();
    while (f->size - f->rindex_shown <= 0 && !f->pktq->abort_request) {
        //SDL_CondWait(f->cond, f->mutex);
		f->cond->wait(f->mutex);
    }
	
    //SDL_UnlockMutex(f->mutex);
	f->mutex.unlock();

    if (f->pktq->abort_request)
        return NULL;

    return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}

void FrameQueueC::frame_queue_push(FrameQueue *f) {
    if (++f->windex == f->max_size)
        f->windex = 0;
	
    //SDL_LockMutex(f->mutex);
	f->mutex.lock();
    f->size++;
    //SDL_CondSignal(f->cond);
	f->cond->signal();
    //SDL_UnlockMutex(f->mutex);
	f->mutex.unlock();
}

void FrameQueueC::frame_queue_next(FrameQueue *f) {
    if (f->keep_last && !f->rindex_shown) {
        f->rindex_shown = 1;
        return;
    }
	
    frame_queue_unref_item(&f->queue[f->rindex]);
    if (++f->rindex == f->max_size) {
        f->rindex = 0;
	}
	
	//SDL_LockMutex(f->mutex);
	f->mutex.lock();
    f->size--;
    //SDL_CondSignal(f->cond);
	f->cond->signal();
    //SDL_UnlockMutex(f->mutex);
	f->mutex.unlock();
}

/* return the number of undisplayed frames in the queue */
int FrameQueueC::frame_queue_nb_remaining(FrameQueue *f) {
    return f->size - f->rindex_shown;
}

/* return last shown position */
int64_t FrameQueueC::frame_queue_last_pos(FrameQueue *f) {
    Frame *fp = &f->queue[f->rindex];
    if (f->rindex_shown && fp->serial == f->pktq->serial)
        return fp->pos;
    else
        return -1;
}
