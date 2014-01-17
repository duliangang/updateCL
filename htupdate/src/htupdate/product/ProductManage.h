#ifndef _PRODUCT_MANANGE_H_
#define _PRODUCT_MANANGE_H_

#include "RegeditMonitor.h"
#include "product.h"
class ProductManage:boost::noncopyable
{
public:
	ProductManage();
	~ProductManage();
	bool Start();
	
protected:
	void OnProductAdd( RegeditData* oldRegedit, RegeditData*  newRegedit);
	void OnProductDel( RegeditData* oldRegedit, RegeditData*  newRegedit);
	void OnProductChange(RegeditData* oldRegedit, RegeditData*  newRegedit);
private:
	std::map<std::string/*regpath*/,boost::shared_ptr<Product> > m_ProductList;
};
#endif