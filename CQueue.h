#include <mqueue.h>
#include <bits/stdc++.h>

/*****************************************************
 Main communication message
 Can't be created, but all kind of messages should
 be inherited from this base calss.
 *****************************************************/
struct MsgBase
{
    virtual ~MsgBase() = 0;
};

/*****************************************************
 Queue C++ wrapper for better understanding.
 Use POSIX queue as most common,
 but usual can be used queue from any RTOS
 *****************************************************/
class Queue
{
public:
    Queue(const char *name, unsigned int size = 10);
    ~Queue();
    bool isOpen() const {return m_queue != -1;}
    bool put(const MsgBase * msg, unsigned int timeout_ms = UINT_MAX);
    MsgBase * get(unsigned int timeout_ms = UINT_MAX);

private:
    mqd_t           m_queue;
    const char    * mp_mame;

    static void absTimeInMs(struct timespec* ts, unsigned int timeout_ms);
};