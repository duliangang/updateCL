#include <string>
#include <stdint.h>
#include <boost/thread.hpp>
#include <map>
struct ProductData
{
	int64_t progid;
	std::string name;
	std::string path;
	std::string version;
	std::string userid;
};
enum ProductStatus
{
	PRODUCT_STATU_CREATE=0x00,
	PRODUCT_STATU_CREATED,
	PRODUCT_STATU_CREATE_FAIL,

	PRODUCT_STATU_SERVER_SELECT,
	PRODUCT_STATU_SERVER_SELECTING,
	PRODUCT_STATU_SERVER_SELECTED,
	PRODUCT_STATU_SERVER_SELECT_FAIL,
	
	PRODUCT_STATU_GET_SERVER_FILELIST,
	PRODUCT_STATU_GET_SERVER_FILELISTING,
	PRODUCT_STATU_GET_SERVER_FILELISTED,
	PRODUCT_STATU_GET_SERVER_FILELIST_FAIL,

	PRODUCT_STATU_CHECK_UPDATE,
	PRODUCT_STATU_CHECK_UPDATING,
	PRODUCT_STATU_CHECK_UPDATED,
	PRODUCT_STATU_CHECK_UPDATE_FAIL,

	PRODUCT_STATU_UPDATE,
	PRODUCT_STATU_UPDATING,
	PRODUCT_STATU_UPDATED,
	PRODUCT_STATU_UPDATE_FAIL,

	PRODUCT_STATU_COMPLETE,

	PRODUCT_STATU_END,
};
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

	Product(const ProductData& data);

	const ProductData* getData()const;

	int SetStatus(int cur_status,int change_status);

	int RegisiterTask(int status,boost::function<void()> TaskFun);

	const std::string& GetFilelistTxtUrl();

	bool setServerFileListTxt(const char* file_path);

	bool SetServerProductInformat(const std::string& xmlString);

	const FileInformatList& getLocalFileInfoList();

	const FileInformatList& getServerFileInfoList();

	DownloadLever getDownloadLevel();

	int64_t getWaitUntilWhenTime();

	const std::string& getServerVersion();

	void setLoacalVersion(const char* newVersion);

	bool isDestory();

protected:

	bool LoadLocalFileList();

	ProductStatus m_ProductStatus;

	DownloadLever m_DownloadLevel;

	FileInformatList m_LocalFileList;

	FileInformatList m_ServerFileList;

	ProductData m_ProductData;

	std::string m_ServerVersion;

	int64_t m_WaitUntileWhenTime;

	bool IsDestory;
};
