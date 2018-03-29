#ifndef SOLAR_CONTROLLER_H
#define SOLAR_CONTROLLER_H

#include <APIController.h>

namespace APLib {
	class SolarController : public APIController {
	  public:
		inline SolarController(Adafruit_TFTLCD* tftPtr, APIController** apiList, GraphicController* graphicCtr, Print &printer) : APIController(tftPtr, apiList, graphicCtr, printer)
		{
		  setCtrId(3);
		  setRecvMasterData(true);
		  setCtrName(String("solar"));
		}

		int drawPage(void);
		
		int mapUserCmd(TSPoint*);

		int consumeMasterData(void);
		
		int index(void);

		void actionCalled(JsonVariant*, long, String);		
	};

}
#endif
