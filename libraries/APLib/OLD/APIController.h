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
#define API_COUNT 256
#define ACTION_COUNT 256

namespace APLib {
	class APIController {
		public : 
			APIController(Adafruit_TFTLCD* tftPtr, APIController **apiList, GraphicController* graphicCtr, Print &printer)
			{
			  _tftPtr = tftPtr;
			  _apiList = apiList;
			  _graphicCtr = graphicCtr;
			  _printer = &printer;
			}

			virtual int drawPage() = 0;
			virtual int mapUserCmd(TSPoint*) = 0;
			virtual int consumeMasterData() = 0;
			virtual int index() = 0;
			virtual void actionCalled(JsonVariant*, long, String) = 0;
			
			int getCtrId() {
				return _ctrId;
			}
			
			void setCtrId(int ctrId) {
				_ctrId = ctrId;
			}
		
			char* getCtrName() {
				return _ctrName;
			}
			
			void setCtrName(String ctrName) {
				ctrName.toCharArray(APIController::_ctrName, 50);
			}
	
			bool getRecvMasterData() {
				return _recvMasterData;
			}
			
			void setRecvMasterData(bool recvMasterData) {
				_recvMasterData = recvMasterData;
			}

			char* getActionToRequest() {
				return APIController::_actionToRequest;
			}
		protected :
			Adafruit_TFTLCD* _tftPtr = NULL;
			GraphicController* _graphicCtr = NULL;
			APIController **_apiList = NULL;
			Print* _printer;
			bool _actionRequested = false;
/*		
			APIController* getCtrByName(String ctrName) {
				for (int ctrId = 1; ctrId < API_COUNT; ctrId++) {
					APIController* ctr = _apiList[ctrId];
					if (ctr != NULL) {
						if (ctr->getCtrName() == ctrName) {
							return ctr;
						}
					}
				}
				return _apiList[0];
			}
*/		
			void callAction(const char* newActionToRequest) {
				strcpy(APIController::_actionToRequest, newActionToRequest);
			}
		private:
			int _ctrId = 0;
			bool _recvMasterData = false;
			char _ctrName[50] = {};
			char _actionToRequest[50] = {};
	};

	class NOOPController : public APIController {
	  public:
		NOOPController(Adafruit_TFTLCD* tftPtr, APIController** apiList, GraphicController* graphicCtr, Print &printer) : APIController(tftPtr, apiList, graphicCtr, printer)
		{
		  setCtrId(0);
		}

		int drawPage(void) {
			return 0;
		}

		int mapUserCmd(TSPoint*) {
			return 0;
		}

		int consumeMasterData(void) {
			return 0;
		}
		
		int index(void) {
			return 0;
		}
		
		void actionCalled(JsonVariant*, long, String) {
			
		}
	};	
}
#endif