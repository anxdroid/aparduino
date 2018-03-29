#include <TempController2.h>
#include <EmonCMSModel.h>

namespace APLib {
	int TempController2::drawPage() {
		_tftPtr->fillScreen(RED);
		_tftPtr->drawLine(_tftPtr->width()/2, 0, _tftPtr->width()/2, _tftPtr->height(), WHITE);
		_tftPtr->drawLine(0, _tftPtr->height()/2, _tftPtr->width(), _tftPtr->height()/2, WHITE);
		int labelSize = 2;
		String label = "";
		// Alto Sinistra (85,175)
		label = "Temp";
		_tftPtr->setCursor((_tftPtr->width()/4)-(label.length()*labelSize*3), (_tftPtr->height()/4)-(labelSize*5));
		_tftPtr->setTextSize(2);
		_tftPtr->setTextColor(WHITE);
		_tftPtr->println(label);
		// Alto Destra (69,44)
		label = "Solar";
		_tftPtr->setCursor((3*_tftPtr->width()/4)-(label.length()*labelSize*3), (_tftPtr->height()/4)-(labelSize*5));
		_tftPtr->setTextSize(2);
		_tftPtr->setTextColor(WHITE);
		_tftPtr->println(label);
		// Basso Sinistra (239, 156)
		label = "Settings";
		_tftPtr->setCursor((_tftPtr->width()/4)-(label.length()*labelSize*3), (3*_tftPtr->height()/4)-(labelSize*5));
		_tftPtr->setTextSize(2);
		_tftPtr->setTextColor(WHITE);
		_tftPtr->println(label);
		// Basso Destra (237, 56)
		label = "Main";
		_tftPtr->setCursor((3*_tftPtr->width()/4)-(label.length()*labelSize*3), (3*_tftPtr->height()/4)-(labelSize*5));
		_tftPtr->setTextSize(2);
		_tftPtr->setTextColor(WHITE);
		_tftPtr->println(label);
		return 0;
	}

	int TempController2::mapUserCmd(TSPoint* p, char* ctrToRequestBuf) {
		// Map user request to API
		APIController2::mapUserCmd(p, ctrToRequestBuf);
		String newApiToRequest = "MainController2";
		// Alto
		if (p->x < _tftPtr->width()/2) {
			// DX
			if (p->y < _tftPtr->height()/2) {
				newApiToRequest = "SolarController2";
				//newApiToRequest = "1";
			// SX
			}else{
				newApiToRequest = "TempController2";
			}
		// Basso
		}else{
			// DX
			if (p->y < _tftPtr->height()/2) {
				//newApiToRequest = "3";
				newApiToRequest = "MainController2";
			// SX
			}else{
				//newApiToRequest = "4";
				newApiToRequest = "SettingsController2";
			}
		}
		memset(ctrToRequestBuf, 0, CTRNAMESIZE); 
		newApiToRequest.toCharArray(ctrToRequestBuf, CTRNAMESIZE);
		return 0;
	}
	
	int TempController2::index(void) {
		APIController2::index();
		/*
		Model* modelPtr = getModel();
		EmonCMSModel* emonCMSModelPtr = static_cast<EmonCMSModel*>(modelPtr);
		JsonObject& result = emonCMSModelPtr->emonCmsFeed(2);
		long time = result["time"];
		long value = result["value"];
		//Serial.println(time);
		//Serial.println(value);
		*/
		
		TempController2::drawPage();
		return 0;
	}
}
