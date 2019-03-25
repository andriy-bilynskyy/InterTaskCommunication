### RTOS intertasks communication mechanism description

 - fast
 - lightweight
 - simple

In **RTOS** some mechanism to share data between tasks is needed. Commonly there is two kind of doing it: **exclusive sections** and **queues**.

**Queue** seems more preferable because provides non-blocking mechanism of doing it. Low priority tasks in this case do not consume time of tasks with higher priority. Also **exclusive  sections** lead to **priority inversion** of course this situation is fixed in all well known **RTOS**es. But any way in case of using **exclusive section**s the low priority tasks spent **CPU** time when they entering these **exclusive section**s. As result this leads to performance degradation.

The one problem with **queue** appears when sharing data structures are large. In that case lot of **CPU** time is spent by copying data into/from **queue**s. To avoid this situation sharing data pointers instead of actual data seems reasonable. But to avoid memory leaks and data collisions sharing mechanism should be well clarified.

**Mechanish clarification:**

 - **Sender task(s)** creates data and sent it. This task do not worry about this data after sent. It can't reuse this data or update.  Each time when task has new data it creates this data and sent.
 - **Receiver task** retrieve data from queue and process received data. After processing receiver delete this data.

**C++** compare to pure **C** provides easy and clear mechanism to distinguish messages retrieved from **queue** without explicitly adding additional data to message, like **message ID** or so on. So we have some **base message polymorphic class** which is empty (does not contain any data). All messages are inherited from this base class with adding data field(s). It provides possibility to send/receive data using pointer to base class. After receiving message its type can be restored using **dynamic_cast** mechanism.

Provided code demonstrates this intertasks communication mechanism. As **OS** chosen **posix** interface as some common implementation. In real cases should be replaced with particular **RTOS** **queue** implementation.

Note:
*For sure **std::cout** is not thread safe and used only for demo. More complex tracing mechanism is not used in order to simplify code
and make idea more clear.*
