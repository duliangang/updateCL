#ifndef _PRODUCT_MANANGE_H_
#define _PRODUCT_MANANGE_H_
#include "pstdint.h"
#include <string>
#include <boost/signals2.hpp>
#include <boost/function.hpp>

enum ProductStatus
{
	PRODUCT_STATU_CREATE=0x00,
	PRODUCT_STATU_CREATED,
	PRODUCT_STATU_CREATE_FAIL,
	
	PRODUCT_STATU_LOCAL_LOAD,
	PRODUCT_STATU_LOCAL_LOADING,
	PRODUCT_STATU_LOCAL_LOADED,
	PRODUCT_STATU_LOCAL_LOAD_FAIL,

	PRODUCT_STATU_SERVER_LOAD,
	PRODUCT_STATU_SERVER_LOADING,
	PRODUCT_STATU_SERVER_LOADED,
	PRODUCT_STATU_SERVER_LOAD_FAIL,

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

	PRODUCT_STATU_DESTORY,

	PRODUCT_STATU£ßMANUALLY_CHECK,

	PRODUCT_STATU_END,
};
class Product;
struct RegeditData;
class RegeditMonitor;
class TaskPool;
typedef boost::function<void (uint32_t status,boost::shared_ptr<Product>) >  RegisterProductStatuChangeFunType;

class ProductManage:boost::noncopyable
{
public:
	ProductManage();

	~ProductManage();

	bool Start();

	int RegisterTask(uint32_t status,RegisterProductStatuChangeFunType TaskFun,const std::string& directions="");

	int UnregisterTask(uint32_t status);

	int SetProductStatus(uint32_t status,Product* product,bool DealWithChange=true,const std::string& directions="");
	
protected:

	virtual void OnProductAdd( RegeditData* oldRegedit, RegeditData*  newRegedit);

	virtual void OnProductDel( RegeditData* oldRegedit, RegeditData*  newRegedit);

	virtual void OnProductChange(RegeditData* oldRegedit, RegeditData*  newRegedit);

	TaskPool* GetTaskPool();

private:

	std::map<std::string/*productName*/,boost::shared_ptr<Product> > m_ProductList;
	

	
	boost::signals2::signal<RegisterProductStatuChangeFunType > m_sigList[PRODUCT_STATU_END];

public:

	RegeditMonitor* m_pRegeditMonitor;
	
	TaskPool* m_TaskPool;
};
#endif