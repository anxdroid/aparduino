#ifndef BACKGROUND_CONTROLLER2_H
#define BACKGROUND_CONTROLLER2_H

#include <APIController2.h>

namespace APLib {
	class BackgroundController2 : public APIController2 {
		public:
			inline BackgroundController2(Adafruit_TFTLCD* tftPtr, GraphicController* graphicCtr) : APIController2(tftPtr, graphicCtr)
			{
			  APIController2::setCtrName("BackgroundController2");
			  setInBackground(true);
			}

			int drawPage(void);

			int mapUserCmd(TSPoint* p, char* ctrToRequestBuf);
			
			virtual int index(void);
			
			virtual int background(void);
		private: 
			int testAction(long p1, double p2);
			
	};

}
#endif
