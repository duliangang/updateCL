#ifndef REACTOR_REACTOR_H_
#define REACTOR_REACTOR_H_

#ifdef _WIN32
#define FD_SETSIZE 8192
#include <Winsock2.h>
#elif defined(__linux__)
#include <stdint.h>
#include <unistd.h>
#include <sys/epoll.h>
#endif

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "threadpool.h"
#include <map>
/// �¼�����
typedef unsigned int event_t;
enum
{
    kReadEvent    = 0x01, ///< ���¼�
    kWriteEvent   = 0x02, ///< д�¼�
    kErrorEvent   = 0x04, ///< �����¼�
	kTimerEvent   = 0x08, ///< timer�¼�
    kEventMask    = 0xff  ///< �¼�����
};

/// �������
#if defined(_WIN32)
    typedef SOCKET handle_type;
#elif defined(__linux__)
    typedef int handle_type;
#else
#error "Ŀǰ����֧�ָ�ƽ̨"
#endif // _WIN32

/// �¼�������
class EventHandler
{
public:

    /// ��ȡ��handler����Ӧ�ľ��
    virtual handle_type GetHandle() const = 0;

    /// ������¼��Ļص�����
    virtual void HandleRead() {}

    /// ����д�¼��Ļص�����
    virtual void HandleWrite() {}

    /// ��������¼��Ļص�����
    virtual void HandleError() {}

protected:

    /// ���캯��,ֻ�������
    EventHandler() {}

    /// ��������,ֻ�������
    virtual ~EventHandler() {}
};

/// reactor��ʵ����
/// TODO (zeshengwu) ʵ�ֶ��̰߳�ȫ�汾
class ReactorImplementation;

typedef std::map<handle_type,  boost::shared_ptr<EventHandler> > EventHandlerListType;
/// reactor�ӿ���
class Reactor
{
public:

    /// ���캯��
	 Reactor(boost::shared_ptr<TaskPool> pTaskPool=NULL);
    /// ��������
    ~Reactor();

    /// ��reactor��ע���ע�¼�evt��handler(������)
    /// @param  handler Ҫע����¼�������
    /// @param  evt     Ҫ��ע���¼�
    /// @retval =0      ע��ɹ�
    /// @retval <0      ע�����
    int RegisterHandler(boost::shared_ptr<EventHandler>  handler, event_t evt);

    /// ��reactor���Ƴ�handler
    /// @param  handler Ҫ�Ƴ����¼�������
    /// @retval =0      �Ƴ��ɹ�
    /// @retval <0      �Ƴ�����
    int RemoveHandler(boost::shared_ptr<EventHandler>  handler);

    /// �����¼�,�ص�ע���handler����Ӧ���¼�������
    /// @param  timeout ��ʱʱ��(����)
    void HandleEvents(int timeout = 0);

	void RegisterTimer(boost::function<void()> task,int millisecWait);

private:

    /// ��ֹcopy
    Reactor(const Reactor &);
    Reactor & operator=(const Reactor &);

private:

    ReactorImplementation * m_reactor_impl; ///< reactor��ʵ����
};
#endif // REACTOR_REACTOR_H_
