#ifndef SETTINGS_CONTROLLER2_H
#define SETTINGS_CONTROLLER2_H

#include <APIController2.h>

namespace APLib {
	class SettingsController2 : public APIController2 {
	  public:
		inline SettingsController2(Adafruit_TFTLCD* tftPtr, GraphicController* graphicCtr) : APIController2(tftPtr, graphicCtr)
		{
		  setCtrId(2);
		  APIController2::setCtrName("SettingsController2");
		}

		int drawPage(void);

		int mapUserCmd(TSPoint*, char* ctrToRequestBuf);
		
		int index(void);		
	};

}
#endif
