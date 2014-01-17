#include <errno.h>
#include <assert.h>
#include <vector>
#include "event_demultiplexer.h"

/// @file   event_demultiplexer.cc
/// @brief  �¼���·�ַ�����ʵ��
/// @author zeshengwu<wuzesheng@vip.qq.com>
/// @date   2011-03-20
#include <boost/function.hpp>
#include "threadpool.h"
#if defined(_WIN32)
#pragma comment(lib, "Ws2_32.lib")
/// ���캯��
SelectDemultiplexer::SelectDemultiplexer()
{
    this->Clear();
}

/// ��ȡ���������¼����������о���ϵ��¼�
/// @param  handlers �¼�����������
/// @param  timeout  ��ʱʱ��
/// @retval = 0      û�з����¼��ľ��(��ʱ)
/// @retval > 0      �����¼��ľ������
/// @retval < 0      ��������
int SelectDemultiplexer::WaitEvents(
        EventHandlerListType * handlers,
		boost::shared_ptr<TaskPool> Taskprocess,
        int timeout)
{
    /// ��select��ȡ�����¼���fd����
    m_timeout.tv_sec = timeout / 1000;
    m_timeout.tv_usec = timeout % 1000 * 1000;
    int max_fd = handlers->rbegin()->first;
    int ret = select(max_fd + 1, &m_read_set, &m_write_set,
                     &m_except_set, &m_timeout);
    if (ret <= 0)
    {
        return ret;
    }
    ///ת��select�Ľ��
    EventHandlerListType::iterator it = handlers->begin();
    while (it != handlers->end())
    {
        if (FD_ISSET(it->first, &m_except_set))
        {
			if(Taskprocess)
			{
				Taskprocess->postTask(boost::bind(&EventHandler::HandleError,it->second));
			}
			else
			{
				 it->second->HandleError();
			}
            FD_CLR(it->first, &m_read_set);
            FD_CLR(it->first, &m_write_set);
        }
        else
        {
            if (FD_ISSET(it->first, &m_read_set))
            {
				if(Taskprocess)
				{
					Taskprocess->postTask(boost::bind(&EventHandler::HandleRead,it->second));
				}
				else
				{
					 it->second->HandleRead();
				}
                FD_CLR(it->first, &m_read_set);
            }
            if (FD_ISSET(it->first, &m_write_set))
            {
				if(Taskprocess)
				{
				  Taskprocess->postTask(boost::bind(&EventHandler::HandleWrite,it->second));
				}
				else
				{
				 it->second->HandleWrite();
				}
                FD_CLR(it->first, &m_write_set);
            }
        }
        FD_CLR(it->first, &m_except_set);
        ++it;
    }
    return ret;
}

/// ���þ��handle��עevt�¼�
/// @retval = 0 ���óɹ�
/// @retval < 0 ���ó���
int SelectDemultiplexer::RequestEvent(handle_type handle, event_t evt)
{
    if (evt & kReadEvent)
    {
        FD_SET(handle, &m_read_set);
    }
    if (evt & kWriteEvent)
    {
        FD_SET(handle, &m_write_set);
    }
    FD_SET(handle, &m_except_set);
    return 0;
}

/// �������handle���¼��Ĺ�ע
/// @retval = 0 �����ɹ�
/// @retval < 0 ��������
int SelectDemultiplexer::UnrequestEvent(handle_type handle)
{
    FD_CLR(handle, &m_read_set);
    FD_CLR(handle, &m_write_set);
    FD_CLR(handle, &m_except_set);
    return 0;
}

/// ������е�fd����
void SelectDemultiplexer::Clear()
{
    FD_ZERO(&m_read_set);
    FD_ZERO(&m_write_set);
    FD_ZERO(&m_except_set);
}
#elif defined(__linux__)
/// ���캯��
EpollDemultiplexer::EpollDemultiplexer()
{
    m_epoll_fd = ::epoll_create(FD_SETSIZE);
    assert(m_epoll_fd != -1);
    m_fd_num = 0;
}

/// ��������
EpollDemultiplexer::~EpollDemultiplexer()
{
    ::close(m_epoll_fd);
}

/// ��ȡ���������¼����������о���ϵ��¼�
/// @param  handlers �¼�����������
/// @param  timeout  ��ʱʱ��
/// @retval = 0      û�з����¼��ľ��(��ʱ)
/// @retval > 0      �����¼��ľ������
/// @retval < 0      ��������
int EpollDemultiplexer::WaitEvents(
       EventHandlerListType * handlers,
	   boost::shared_ptr<TaskPool> Taskprocess,
        int timeout)
{
    std::vector<epoll_event> ep_evts(m_fd_num);
    int num = epoll_wait(m_epoll_fd, &ep_evts[0], ep_evts.size(), timeout);
    if (num > 0)
    {
        for (int idx = 0; idx < num; ++idx)
        {
            handle_type handle = ep_evts[idx].data.fd;
            assert(handlers->find(handle) != handlers->end());
            if ((ep_evts[idx].events & EPOLLERR) ||
                    (ep_evts[idx].events & EPOLLHUP))
            {
				if(Taskprocess)
				{
					Taskprocess->postTask(boost::bind(&EventHandler::HandleError, (*handlers)[handle]));
				}
				else
				{
				  (*handlers)[handle]->HandleError();
				}
            }
            else
            {
                if (ep_evts[idx].events & EPOLLIN)
                {
					if(Taskprocess)
					{
						Taskprocess->postTask(boost::bind(&EventHandler::HandleRead, (*handlers)[handle]));
					}
					else
					{
						(*handlers)[handle]->HandleRead();
					}
                }
                if (ep_evts[idx].events & EPOLLOUT)
                {
					if(Taskprocess)
					{
						Taskprocess->postTask(boost::bind(&EventHandler::HandleWrite, (*handlers)[handle]));
					}
					else
					{
						(*handlers)[handle]->HandleWrite();
					}
                }
            }
        }
    }
    return num;
}

/// ���þ��handle��עevt�¼�
/// @retval = 0 ���óɹ�
/// @retval < 0 ���ó���
int EpollDemultiplexer::RequestEvent(handle_type handle, event_t evt)
{
    epoll_event ep_evt;
    ep_evt.data.fd = handle;
    ep_evt.events = 0;

    if (evt & kReadEvent)
    {
        ep_evt.events |= EPOLLIN;
    }
    if (evt & kWriteEvent)
    {
        ep_evt.events |= EPOLLOUT;
    }
    ep_evt.events |= EPOLLONESHOT;

    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, handle, &ep_evt) != 0)
    {
        if (errno == ENOENT)
        {
            if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, handle, &ep_evt) != 0)
            {
                return -errno;
            }
            ++m_fd_num;
        }
    }
    return 0;
}

/// �������handle���¼��Ĺ�ע
/// @retval = 0 �����ɹ�
/// @retval < 0 ��������
int EpollDemultiplexer::UnrequestEvent(handle_type handle)
{
    epoll_event ep_evt;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, handle, &ep_evt) != 0)
    {
        return -errno;
    }
    --m_fd_num;
    return 0;
}
#else
#error "Ŀǰ����֧�ָ�ƽ̨"
#endif // _WIN32
