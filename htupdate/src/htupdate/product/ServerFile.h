#ifndef _SERVER_FILE_H_
#define _SERVER_FILE_H_
#include <string>
#include <boost/shared_ptr.hpp>
class Product;	

class ServerSeesion
{
public:
	ServerFileDown(const std::string& ServerUrl);
	int GetProductServerData(std::string& serverDataXml);
	int DownloadFile(const std::string& url,const std::string& filename);
	int PostDataToUrl(const std::string& url,const std::string& postData);
	void CloseSession();
};
#endif