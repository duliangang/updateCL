#ifndef REACTOR_EVENT_DEMULTIPLEXER_H_
#define REACTOR_EVENT_DEMULTIPLEXER_H_

#include <set>
#include <map>
#include "reactor.h"

/// @file   event_demultiplexer.h
/// @brief  �¼���·�ַ�����
/// @author zeshengwu<wuzesheng@vip.qq.com>
/// @date   2011-03-20

class EventDemultiplexer
{
public:

    /// ��������
    virtual ~EventDemultiplexer() {}

    /// ��ȡ���������¼����������о���ϵ��¼�
    /// @param  handlers �¼�����������
    /// @param  timeout  ��ʱʱ��
    /// @retval = 0      û�з����¼��ľ��(��ʱ)
    /// @retval > 0      �����¼��ľ������
    /// @retval < 0      ��������
    virtual int WaitEvents(EventHandlerListType* handlers,
						   boost::shared_ptr<TaskPool> Taskprocess,
                           int timeout = 0) = 0;

    /// ���þ��handle��עevt�¼�
    /// @retval = 0 ���óɹ�
    /// @retval < 0 ���ó���
    virtual int RequestEvent(handle_type handle, event_t evt) = 0;

    /// �������handle���¼��Ĺ�ע
    /// @retval = 0 �����ɹ�
    /// @retval < 0 ��������
    virtual int UnrequestEvent(handle_type handle) = 0;
};

///////////////////////////////////////////////////////////////////////////////

/// ��selectʵ�ֵĶ�·�¼��ַ���
class SelectDemultiplexer : public EventDemultiplexer
{
public:

    /// ���캯��
    SelectDemultiplexer();

    /// ��ȡ���������¼����������о���ϵ��¼�
    /// @param  handlers �¼�����������
    /// @param  timeout  ��ʱʱ��
    /// @retval = 0      û�з����¼��ľ��(��ʱ)
    /// @retval > 0      �����¼��ľ������
    /// @retval < 0      ��������
    virtual int WaitEvents(EventHandlerListType * handlers,
						   boost::shared_ptr<TaskPool> Taskprocess,
                           int timeout = 0);

    /// ���þ��handle��עevt�¼�
    /// @retval = 0 ���óɹ�
    /// @retval < 0 ���ó���
    virtual int RequestEvent(handle_type handle, event_t evt);

    /// �������handle���¼��Ĺ�ע
    /// @retval = 0 �����ɹ�
    /// @retval < 0 ��������
    virtual int UnrequestEvent(handle_type handle);

private:

    /// ������е�fd����
    void Clear();

private:

    fd_set              m_read_set;   ///< ���¼�fd����
    fd_set              m_write_set;  ///< д�¼�fd����
    fd_set              m_except_set; ///< �쳣�¼�fd����
    timeval             m_timeout;    ///< ��ʱ
};

///////////////////////////////////////////////////////////////////////////////

/// ��epollʵ�ֵĶ�·�¼��ַ���
class EpollDemultiplexer : public EventDemultiplexer
{
public:

    /// ���캯��
    EpollDemultiplexer();

    /// ��������
    ~EpollDemultiplexer();

    /// ��ȡ���������¼����������о���ϵ��¼�
    /// @param  handlers �¼�����������
    /// @param  timeout  ��ʱʱ��
    /// @retval = 0      û�з����¼��ľ��(��ʱ)
    /// @retval > 0      �����¼��ľ������
    /// @retval < 0      ��������
    virtual int WaitEvents(EventHandlerListType* handlers,
						   boost::shared_ptr<TaskPool> Taskprocess,
                           int timeout = 0);

    /// ���þ��handle��עevt�¼�
    /// @retval = 0 ���óɹ�
    /// @retval < 0 ���ó���
    virtual int RequestEvent(handle_type handle, event_t evt);

    /// �������handle���¼��Ĺ�ע
    /// @retval = 0 �����ɹ�
    /// @retval < 0 ��������
    virtual int UnrequestEvent(handle_type handle);

private:

    int  m_epoll_fd; ///< epoll�����fd
    int  m_fd_num;   ///< ����fd�ĸ���
};




#endif // REACTOR_EVENT_DEMULTIPLEXER_H_
