#include "ProductEvent.h"
//socket ManuallyCheckEvent
class ManuallyCheckEvent:public ProductEvent
{

public:
	ManuallyCheckEvent(int port);

protected:
	virtual void ProcessStatus(uint32_t status,boost::shared_ptr<Product> processProduct);

};