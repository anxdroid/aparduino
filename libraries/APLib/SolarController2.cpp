#include <SolarController2.h>
#include <UIModNumValElement.h>
#include <UIButtonElement.h>
#include <EmonCMSModel.h>

namespace APLib {
	int SolarController2::drawPage() {
		_tftPtr->fillScreen(YELLOW);
		int width = (_tftPtr->width()/2);
		int height = (_tftPtr->height()/2);
		UIButtonElement* button1 = new UIButtonElement(_tftPtr, 0, 0, width-10, height-10, WHITE, "Main", BLACK, 2, RED, "MainController2", "index");
		button1->drawElement(false);
		_uiElements[0] = button1;
		
		UIButtonElement* button2 = new UIButtonElement(_tftPtr, 0, height+5, width-10, height-10, YELLOW, "Solar", BLACK, 2, RED, "SolarController2", "index");
		button2->drawElement(false);
		_uiElements[1] = button2;
		
		UIButtonElement* button3 = new UIButtonElement(_tftPtr, width+5, 0, width-10, height-10, GREEN, "Temp", BLACK, 2, RED, "TempController2", "index");
		button3->drawElement(false);
		_uiElements[2] = button3;
		
		UIModNumValElement* button4 = new UIModNumValElement(_tftPtr, width+5, height+5, width-10, height-10, BLUE, "Settings", BLACK, 2, RED, "", "");
		button4->setBoundVar(&(this->_testValue));
		button4->setStep(0.03);
		button4->setDecreaseAction("decreaseVal");
		button4->setIncreaseAction("increaseVal");
		button4->drawElement(false);		
		_uiElements[3] = button4;
		return 0;
	}
	
	int SolarController2::proxyAction() {
		Serial.print("Calling action ");
		String action = _clickedElement->getAction();
		Serial.println(action);
		
		if (action == "decreaseVal") {
			UIModNumValElement* elmPtr = (UIModNumValElement*) _mostExternalClickedElement;
			float step = elmPtr->getStep();
			float* boundVar = elmPtr->getBoundVar();
			Serial.println(String(*boundVar));
			decreaseVal(boundVar, step);
			_mostExternalClickedElement->drawElement(false);
		}else if (action == "increaseVal") {
			UIModNumValElement* elmPtr = (UIModNumValElement*) _mostExternalClickedElement;
			float step = elmPtr->getStep();
			float* boundVar = elmPtr->getBoundVar();
			Serial.println(String(*boundVar));
			increaseVal(boundVar, step);
			_mostExternalClickedElement->drawElement(false);
		}else{
			Serial.println("Unrecognized action !");
		}
	}

	// ACTIONS	
	int SolarController2::decreaseVal(float* val, float step) {
		*val = *val - step;
		Serial.println(*val);
	}

	int SolarController2::increaseVal(float* val, float step) {
		*val = *val + step;
		Serial.println(*val);
	}	

	int SolarController2::mapUserCmd(TSPoint* p, char* ctrToRequestBuf) {
		return APIController2::mapUserCmd(p, ctrToRequestBuf);
	}
	
	int SolarController2::index(void) {
		APIController2::index();
		SolarController2::drawPage();
		return 0;
	}
}
