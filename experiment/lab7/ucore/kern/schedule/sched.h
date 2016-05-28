#ifndef __KERN_SCHEDULE_SCHED_H__
#define __KERN_SCHEDULE_SCHED_H__


#include <io.h>
#include <sync.h>
#include <call.h>
#include <pstruct.h>
#include <schstruct.h>
#include <default_sched.h>

// the list of timer
list_entry_t timer_list;

struct sched_class *sched_class;

struct run_queue *rq;

void sched_class_enqueue(struct proc_struct *proc) {
    if (proc != idleproc) {
        call2(rq, proc,sched_class->enqueue);
    }
}

void sched_class_dequeue(struct proc_struct *proc) {
    call2(rq, proc, sched_class->dequeue);
}

struct proc_struct* sched_class_pick_next(void) {
    return call1(rq, sched_class->pick_next);
}

void sched_class_proc_tick(struct proc_struct *proc) {
    if (proc != idleproc) {
        call2(rq, proc, sched_class->proc_tick);
    }
    else {
        proc->need_resched = 1;
    }
}

struct run_queue __rq;

void sched_init(void) {
    list_init(&timer_list);

    load_default_sched_class();
    sched_class = &default_sched_class;

    rq = &__rq;
    rq->max_time_slice = 5;
    call1(rq, sched_class->init);

    printf("sched class: %s\n", sched_class->name);
}

void wakeup_proc(struct proc_struct *proc) {
    bool intr_flag;
    assert(proc->state != PROC_ZOMBIE);
    local_intr_save(intr_flag);
    {
        if (proc->state != PROC_RUNNABLE) {
            proc->state = PROC_RUNNABLE;
            proc->wait_state = 0;
            if (proc != current) {
                sched_class_enqueue(proc);
            }
        }
        else {
            printf("wakeup runnable process.\n");
        }
    }
    local_intr_restore(intr_flag);
}

void schedule(void) {
    bool intr_flag;
    struct proc_struct *next;
    local_intr_save(intr_flag);
    {
        current->need_resched = 0;
        if (current->state == PROC_RUNNABLE) {
            sched_class_enqueue(current);
        }
        if ((next = sched_class_pick_next()) != NULL) {
            sched_class_dequeue(next);
        }
        if (next == NULL) {
            next = idleproc;
        }
        next->runs ++;
        if (next != current) {
            proc_run(next);
        }
    }
    local_intr_restore(intr_flag);
}

// add timer to timer_list
void
add_timer(timer_t *timer) {
    bool intr_flag;
    list_entry_t *le;
    timer_t *next;

    local_intr_save(intr_flag);
    {
        assert(timer->expires > 0 && timer->proc != NULL);
        assert(list_empty(&(timer->timer_link)));
        le = list_next(&timer_list);
        while (le != &timer_list) {
            next = le2timer(le, timer_link);
            if (timer->expires < next->expires) {
                next->expires -= timer->expires;
                break;
            }
            timer->expires -= next->expires;
            le = list_next(le);
        }
        list_add_before(le, &(timer->timer_link));
    }
    local_intr_restore(intr_flag);
}

// del timer from timer_list
void
del_timer(timer_t *timer) {
    bool intr_flag;
    list_entry_t *le;
    timer_t *next;

    local_intr_save(intr_flag);
    {
        if (!list_empty(&(timer->timer_link))) {
            if (timer->expires != 0) {
                le = list_next(&(timer->timer_link));
                if (le != &timer_list) {
                    next = le2timer(le, timer_link);
                    next->expires += timer->expires;
                }
            }
            list_del_init(&(timer->timer_link));
        }
    }
    local_intr_restore(intr_flag);
}

// call scheduler to update tick related info, and check the timer is expired? If expired, then wakup proc
void
run_timer_list(void) {
    bool intr_flag;
    list_entry_t *le;
    timer_t *timer;
    struct proc_struct *proc;

    local_intr_save(intr_flag);
    {
        le = list_next(&timer_list);
        if (le != &timer_list) {
            timer = le2timer(le, timer_link);
            assert(timer->expires != 0);
            timer->expires --;
            while (timer->expires == 0) {
                le = list_next(le);
                proc = timer->proc;
                if (proc->wait_state != 0) {
                    assert(proc->wait_state & WT_INTERRUPTED);
                }
                else {
                    printf("process %d's wait_state == 0.\n", proc->pid);
                }
                wakeup_proc(proc);
                del_timer(timer);
                if (le == &timer_list) {
                    break;
                }
                timer = le2timer(le, timer_link);
            }
        }
        sched_class_proc_tick(current);
    }
    local_intr_restore(intr_flag);
}

#endif /* !__KERN_SCHEDULE_SCHED_H__ */
