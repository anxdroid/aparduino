#ifndef MAIN_CONTROLLER2_H
#define MAIN_CONTROLLER2_H

#include <APIController2.h>

namespace APLib {
	class MainController2 : public APIController2 {
		public:
			inline MainController2(Adafruit_TFTLCD* tftPtr, GraphicController* graphicCtr) : APIController2(tftPtr, graphicCtr)
			{
			  APIController2::setCtrName("MainController2");
			  setInBackground(true);
			}

			int drawPage(void);

			int mapUserCmd(TSPoint* p, char* ctrToRequestBuf);
			
			int index(void);
			
			int proxyAction();
			
			int background(void);
		private:
			float _testValue = 0.35;		
			int decreaseVal(float*, float);
			int increaseVal(float*, float);
			float _tempDisimpegno = 0.0;
			long _timeTempDisimpegno = 0;
			float _consumoCorrente = 0.0;
			long _timeConsumoCorrente = 0;
			
	};
}
#endif
