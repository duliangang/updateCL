#ifndef _REGEDIT_MONITOR_H_
#define _REGEDIT_MONITOR_H_
#include <boost/signals2.hpp>
#include <boost/function.hpp>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
enum RegeditMonitorStatus
{
	REGEDIT_STATUS_ADD=0x00,
	REGEDIT_STATUS_DEL,
	REGEDIT_STATUS_CHANGE,
	REGEDIT_STATUS_END,
};
#define REGEDIT_STATUS_MAX REGEDIT_STATUS_END

struct RegeditData
{
	std::string Path;
	std::map<std::string,std::string> m_KeyVal;
};
class RegeditMonitor:boost::noncopyable
{
public:
	RegeditMonitor();

	void connect( boost::function< void (RegeditData* /*old*/,RegeditData*/*new*/) > ); 

	void run(const std::string& MonitorPath);

	void stop();

	~RegeditMonitor();
	
	
private:

	bool m_isStop;

	boost::signals2::signal< void (RegeditData* /*old*/,RegeditData*/*new*/)  > m_sig[REGEDIT_STATUS_MAX];

	std::map<std::string /*path*/,RegeditData /*val*/> m_RegeditList;
};
#endif