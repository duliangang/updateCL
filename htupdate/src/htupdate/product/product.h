#ifndef _PRODUCT_H_
#define _PRODUCT_H_
#include <string>
#include <stdint.h>
#include <boost/thread.hpp>
#include <map>
#include "pstdint.h"
struct ProductData
{
	int64_t progid;
	std::string name;
	std::string path;
	std::string version;
	std::string userid;
};
class ServerSeesion;
enum DownloadLever
{
	STATE_NOT_SET,
	STATE_SILENT_INSTALL,
	STATE_REQUEST_INSTALL,
	STATE_PROMPT_INSTALLL,
};
typedef std::map<std::string/*filename*/,std::string/*md5*/> FileInformatList;

class Product:boost::noncopyable
{
public:

	friend class ProductManage;

	Product(const ProductData& data);

	const ProductData* getData()const;

	const std::string& GetFilelistTxtUrl()const;

	bool setServerFileListTxt(const char* file_path);

	bool SetServerProductInformat(const std::string& xmlString);

	const FileInformatList& getLocalFileInfoList()const;

	const FileInformatList& getServerFileInfoList()const;

	DownloadLever getDownloadLevel()const;

	int64_t getWaitUntilWhenTime()const;

	const std::string& getServerVersion()const;

	void setLoacalVersion(const char* newVersion);

	uint32_t GetStatus()const;

	ServerSeesion* GetServerSession();

protected:

	uint32_t SetStatus(uint32_t change_status);

	bool LoadLocalFileList();

	uint32_t m_status;

	DownloadLever m_DownloadLevel;

	FileInformatList m_LocalFileList;

	FileInformatList m_ServerFileList;

	ProductData m_ProductData;

	std::string m_ServerVersion;

	int64_t m_WaitUntileWhenTime;

	bool m_IsDestory;

	ServerSeesion* m_ServerSeesion;
};
#endif