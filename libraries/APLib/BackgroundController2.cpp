#include <BackgroundController2.h>
#include <EmonCMSModel.h>

namespace APLib {
	int BackgroundController2::drawPage() {
		return 0;
	}

	int BackgroundController2::mapUserCmd(TSPoint* p, char* ctrToRequestBuf) {
		return 0;
	}
	
	int BackgroundController2::index(void) {
		return 0;
	}
	
	int BackgroundController2::background(void) {
		//Serial.print(this->_ctrName);
		//Serial.println(" doing background stuff...");
		return 0;
	}
	
}
