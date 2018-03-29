#include <MainController.h>

namespace APLib {
	int MainController::drawPage() {
		return MainController::index();
	}

	int MainController::mapUserCmd(TSPoint* p) {
		// Map user request to API
		int newApiToRequest = 0;
		//int color = RED;
		if (p->x < _tftPtr->width()/2) {
			//color = BLUE;
			newApiToRequest = 0;
			if (p->y < _tftPtr->width()/2) {
				//color = WHITE;
				newApiToRequest = 3;
			}else{
				if (p->y < _tftPtr->width()/2) {
					//color = YELLOW;
					newApiToRequest = 0;
				}
			}
		}
		return newApiToRequest;
	}

	int MainController::consumeMasterData(void) {		
	  return 0;
	}
	
	int MainController::index(void) {
		_tftPtr->reset();
		_tftPtr->begin(0x9341); // SDFP5408
		_tftPtr->setRotation(3); // Need for the Mega, please changed for your choice or rotation initial 

		_tftPtr->fillScreen(BLACK);
		_tftPtr->drawLine(_tftPtr->width()/2, 0, _tftPtr->width()/2, _tftPtr->height(), WHITE);
		_tftPtr->drawLine(0, _tftPtr->height()/2, _tftPtr->width(), _tftPtr->height()/2, WHITE);
		int labelSize = 2;
		String label = "Solar";
		_tftPtr->setCursor((3*_tftPtr->width()/4)-(label.length()*labelSize*3), (_tftPtr->height()/4)-(labelSize*5));
		_tftPtr->setTextSize(2);
		_tftPtr->setTextColor(YELLOW);
		_tftPtr->println(label);
		return 0;
	}
	
	void MainController::actionCalled(JsonVariant* jsonMasterDataPayloadPtr, long jsonMasterTSStart, String lastAction) {
		Serial.println("ok thanks !");
	}
}
