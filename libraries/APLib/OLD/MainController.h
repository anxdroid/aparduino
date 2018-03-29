#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include <APIController.h>

namespace APLib {
	class MainController : public APIController {
	  public:
		inline MainController(Adafruit_TFTLCD* tftPtr, APIController** apiList, GraphicController* graphicCtr, Print &printer) : APIController(tftPtr, apiList, graphicCtr, printer)
		{
		  setCtrId(1);
		  setCtrName(String("main"));
		}

		int drawPage(void);

		int mapUserCmd(TSPoint*);

		int consumeMasterData(void);
		
		int index(void);
		
		void actionCalled(JsonVariant*, long, String);
	};

}
#endif
