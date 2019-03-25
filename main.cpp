#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "CQueue.h"

/*****************************************************
RTOS intertasks communication
 - fast
 - lightweight
 - simple

 In RTOS some mechanism to share data between tasks is needed.
 Commonly there is two kind of doing it: exclusive sections
 and queues.

 Queue seems more preferable because provides non-blocking
 mechanism of doing it. Low priority tasks in this case do not
 consume time of tasks with higher priority. Also exclusive 
 sections lead to priority inversion of course this situation 
 is fixed in all well known RTOSes. But any way in case of 
 using exclusive sections the low priority tasks spent CPU
 time when they entering these exclusive sections. As result this
 leads to performance degradation.

 The one problem with queue appears when sharing data structures
 are large. In that case lot of CPU time is spent by copying data
 into/from queues.
 To avoid this situation sharing data pointers instead of actual data
 seems reasonable. But to avoid memory leaks and data collisions 
 sharing mechanism should be well clarified.

 Clarification:
 - Sender task(s) creates data and sent it. This task do not worry 
 about this data after sent. It can't reuse this data or update. 
 Each time when task has new data it creates this data and sent.
 - Receiver task retrieve data from queue and process received data.
 After processing receiver delete this data.
 
 C++ compare to pure C provides easy and clear mechanism to 
 distinguish messages retrieved from queue without explicitly
 adding additional data to message, like message ID or so on.
 So we have some base message polymorphic class which is empty 
 (does not contain any data). All messages are inherited from 
 this base class with adding data field(s). It provides 
 possibility to send/receive data using pointer to base class.
 After receiving message its type can be restored using 
 dynamic_cast mechanism.

 Provided code demonstrates this intertasks communication mechanism.
 As OS chosen posix interface as some common implementation.
 In real cases should be replaced with particular RTOS 
 queue implementation.

 Note:
 For sure std::cout is not thread safe and used only for demo.
 More complex tracing mechanism is not used in order to simplify code
 and make idea more clear.
 *****************************************************/

/*****************************************************
 Measge to send, just a sample.
 In real case can be much more complicated structure.
 More bigger structure more profit.
 *****************************************************/
struct MsgADC: public MsgBase
{
    MsgADC(float v, float i) : m_voltage(v), m_current(i) {}
    float m_voltage;
    float m_current;
};

/*****************************************************
 Measge to send, just a sample.
 In real case can be much more complicated structure.
 More bigger structure more profit.
 *****************************************************/
struct MsgTemperature: public MsgBase
{
    explicit MsgTemperature(int T) : m_temperature(T) {}
    int m_temperature;
};

/*****************************************************
 Auxilary data to stop application by pressing Ctrl+C
 *****************************************************/
static volatile bool terminate = false;

void sig_handler(int signum)
{
    std::cout << "terminating..." << std::endl;
    (void)signum;
    terminate = true;
}

/*****************************************************
 Demo task wich sends the data.
 Sender task just creates data structures and send them.
 No worry about their future (delete an so on).
 Created message can't be updated or reused!
 When new data is available new message should be created.
 *****************************************************/
void ADCRoutine(Queue * queue)
{
     std::cout << "ADC Thread started" << std::endl;
     while(!terminate)
     {
        MsgADC *a = new MsgADC(3.27, 12.5);
        queue->put(a);
        usleep(500*1000);
     }
     std::cout << "ADC Thread stopped" << std::endl;
}

/*****************************************************
 Demo task wich sends the data.
 Sender task just creates data structures and send them.
 No worry about their future (delete an so on).
 Created message can't be updated or reused!
 When new data is available new message should be created.
 *****************************************************/
void TemperatureRoutine(Queue * queue)
{
     std::cout << "Temperarture Thread started" << std::endl;
     while(!terminate)
     {
        MsgTemperature *a = new MsgTemperature(32);
        queue->put(a);
        usleep(700*1000);
     }
     std::cout << "Temperarture Thread stopped" << std::endl;
}

/*****************************************************
 Demo task wich reives the data.
 Data is received from queue. After that original data 
 type is estimated. Received data is processed and finaly
 deleted.
 *****************************************************/
int main()
{
    Queue queue("/test");
    if(queue.isOpen())
    {
        struct sigaction act;
        memset(&act, 0, sizeof(act));
        act.sa_handler = sig_handler;
        sigemptyset(&act.sa_mask);
        (void)sigaction(SIGINT, &act, NULL);

        pthread_t ADCThread, TemperatureThread;
        (void)pthread_create( &ADCThread, NULL, (void* (*)(void*))ADCRoutine, &queue);
        (void)pthread_create( &TemperatureThread, NULL, (void* (*)(void*))TemperatureRoutine, &queue);
        while(!terminate)
        {
            MsgBase * received = queue.get();
            if(MsgADC* p = dynamic_cast<MsgADC*>(received))
            {
                std::cout << "On new ADC data" << std::endl;
                std::cout << "Voltage: " << p->m_voltage  << "V" << std::endl;
                std::cout << "Current: " << p->m_current << "mA" << std::endl;
            }
            else if(MsgTemperature* p = dynamic_cast<MsgTemperature*>(received))
            {
                std::cout << "On new temperature data" << std::endl;
                std::cout << "Temerature: " << p->m_temperature << "\u2103" << std::endl;
            }
            else
            {
                std::cout << "unknown message!" << std::endl;
            }
            delete received;
        }
        pthread_join(ADCThread, NULL);
        pthread_join(TemperatureThread, NULL);
    }
    else
    {
        std::cout << "failed to create queue" << std::endl;
    }
    return 0;
}