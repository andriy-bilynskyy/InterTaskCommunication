#include "CQueue.h"

MsgBase::~MsgBase(){}

Queue::Queue(const char *name, unsigned int size) : m_queue(-1), mp_mame(name)
{
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = size,
        .mq_msgsize = sizeof(MsgBase*),
        .mq_curmsgs = 0,
        .__pad = {}
    };
    m_queue = mq_open(mp_mame, O_CREAT | O_RDWR, 0600, &attr);
}

Queue::~Queue()
{
    if(isOpen())
    {
        (void)mq_close(m_queue);
        (void)mq_unlink(mp_mame);
    }
}

/*****************************************************
 Send the pointer to messave via queue,
 Not actual message!
 *****************************************************/
bool Queue::put(const MsgBase * msg, unsigned int timeout_ms)
{
    struct timespec ts;
    absTimeInMs(&ts, timeout_ms);
    bool sent = false;
    if(isOpen())
    {
        sent = !mq_timedsend(m_queue, (const char*)&msg, sizeof(MsgBase*), 1, &ts);
    }
    (void)timeout_ms;
    return sent;
}

/*****************************************************
 Receive the pointer to messave via queue,
 Not actual message!
 *****************************************************/
MsgBase * Queue::get(unsigned int timeout_ms)
{
    struct timespec ts;
    absTimeInMs(&ts, timeout_ms);
    MsgBase * msg = NULL;
    if(isOpen())
    {
        (void)mq_timedreceive(m_queue, (char *)&msg, sizeof(MsgBase*), NULL, &ts);
    }
    return msg;
}

void Queue::absTimeInMs(struct timespec* ts, unsigned int timeout_ms)
{
        (void)clock_gettime(CLOCK_REALTIME, ts);
        ts->tv_sec += timeout_ms/1000u;
        ts->tv_nsec += 1000000L * (timeout_ms % 1000);
        if(ts->tv_nsec >= 1000000000L)
        {
            ts->tv_nsec -= 1000000000L;
            ts->tv_sec ++;
        }
}
