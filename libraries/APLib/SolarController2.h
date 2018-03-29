#ifndef SOLAR_CONTROLLER2_H
#define SOLAR_CONTROLLER2_H

#include <APIController2.h>
#include <UINumValElement.h>
#include <UIButtonElement.h>

namespace APLib {
	class SolarController2 : public APIController2 {
		public:
			inline SolarController2(Adafruit_TFTLCD* tftPtr, GraphicController* graphicCtr) : APIController2(tftPtr, graphicCtr)
			{
			  APIController2::setCtrName("SolarController2");
			}

			int drawPage(void);

			int mapUserCmd(TSPoint* p, char* ctrToRequestBuf);
			
			int index(void);
			
			int proxyAction();
			
		private:
			float _testValue = 0.37;		
			int decreaseVal(float*, float);
			int increaseVal(float*, float);
	};

}
#endif
