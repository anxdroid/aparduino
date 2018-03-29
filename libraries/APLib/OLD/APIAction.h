#ifndef API_CONTROLLER_H
#define API_CONTROLLER_H

#include <SPFD5408_TouchScreen.h>
#include <SPFD5408_Adafruit_TFTLCD.h>
#include <GraphicController.h>

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

namespace APLib {
	class APIAction {
		public :  
			inline APIAction(Adafruit_TFTLCD* tftPtr, APIController **apiList, GraphicController* graphicCtr, Print &printer)
			{
			  _tftPtr = tftPtr;
			  _apiList = apiList;
			  _graphicCtr = graphicCtr;
			  _printer = &printer;
			}

			virtual int drawPage() = 0;
			virtual int mapUserCmd(TSPoint*) = 0;
			virtual int consumeMasterData() = 0;
			
			inline int getCtrId() {
				return _ctrId;
			}
			
			inline void setCtrId(int ctrId) {
				_ctrId = ctrId;
			}
			
			inline int getActId() {
				return _actId;
			}
			
			inline void setActId(int actId) {
				_actId = actId;
			}

			inline bool getRecvMasterData() {
				return _recvMasterData;
			}
			
			inline void setRecvMasterData(bool recvMasterData) {
				_recvMasterData = recvMasterData;
			}		
		protected :
			Adafruit_TFTLCD* _tftPtr = NULL;
			APIController **_apiList = NULL;
			GraphicController* _graphicCtr = NULL;
			Print* _printer;
			//char _ctrName[50];
		private:
			int _ctrId = 0;
			int _actId = 0;
			bool _recvMasterData = false;		
	};	
}
#endif