#ifndef TEMP_CONTROLLER2_H
#define TEMP_CONTROLLER2_H

#include <APIController2.h>

namespace APLib {
	class TempController2 : public APIController2 {
	  public:
		inline TempController2(Adafruit_TFTLCD* tftPtr, GraphicController* graphicCtr) : APIController2(tftPtr, graphicCtr)
		{
		  setCtrId(2);
		  APIController2::setCtrName("TempController2");
		}

		int drawPage(void);

		int mapUserCmd(TSPoint*, char* ctrToRequestBuf);
		
		int index(void);		
	};

}
#endif
