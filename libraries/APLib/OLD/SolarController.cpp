#include <SolarController.h>

namespace APLib {
	int SolarController::drawPage() {
		return SolarController::index();
	}

	int SolarController::mapUserCmd(TSPoint* p) {
		// Map user request to API
		int newApiToRequest = 1;
		//int color = RED;
		if (p->x < _tftPtr->width()/2) {
			//color = BLUE;
			newApiToRequest = 1;
			if (p->y < _tftPtr->width()/2) {
				//color = WHITE;
				newApiToRequest = 3;
			}else{
				if (p->y < _tftPtr->width()/2) {
					//color = YELLOW;
					newApiToRequest = 1;
				}
			}
		}
		return newApiToRequest;
	}

	int SolarController::consumeMasterData(void) {
		return 0;
	}
	
	int SolarController::index() {
		//Serial.println("index");
		APIController::callAction("getSolarData");
	}
	
	void SolarController::actionCalled(JsonVariant* jsonMasterDataPayloadPtr, long jsonMasterTSStart, String lastAction) {
		
		//char lastActionCharArray[50] = {};
		//lastAction.toCharArray(lastActionCharArray, 50);
		//if (strcmp(lastActionCharArray, SolarController::getActionToRequest()) == 0) {
			APIController::callAction("");
			if (lastAction == "getSolarData") {
				Serial.print("ok thanks for ");
				Serial.println(lastAction);
				_tftPtr->reset();
				_tftPtr->begin(0x9341); // SDFP5408
				_tftPtr->setRotation(3); // Need for the Mega, please changed for your choice or rotation initial 
				_tftPtr->fillScreen(BLACK);
				int chartWidth = 320;
				int chartHeight = 240;
				int leftMargin = 10;
				int topMargin = 10;
				int bottomMargin = 40;
				int rightMargin = 20;
				int labelSize = 2;
				Serial.println("Drawing graph");
				_graphicCtr->drawChart(_tftPtr, jsonMasterDataPayloadPtr, jsonMasterTSStart, chartWidth, chartHeight, topMargin, leftMargin, bottomMargin, rightMargin, labelSize);
			}
		//}else{
		//	Serial.println("Something wrong happened !!!");
		//}
		
	}

}
