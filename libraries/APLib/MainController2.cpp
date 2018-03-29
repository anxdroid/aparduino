#include <MainController2.h>
#include <UIModNumValElement.h>
#include <UINumValTimeElement.h>
#include <UIButtonElement.h>
#include <EmonCMSModel.h>

namespace APLib {
	int MainController2::drawPage() {
		_tftPtr->fillScreen(BLACK);
		int width = (_tftPtr->width()/2);
		int height = (_tftPtr->height()/2);
		UIButtonElement* button1 = new UIButtonElement(_tftPtr, 0, 0, width-10, height-10, WHITE, "Main", BLACK, 2, RED, "MainController2", "index");
		button1->drawElement(false);
		_uiElements[0] = button1;
		
		UIButtonElement* button2 = new UIButtonElement(_tftPtr, 0, height+5, width-10, height-10, YELLOW, "Solar", BLACK, 2, RED, "SolarController2", "index");
		button2->drawElement(false);
		_uiElements[1] = button2;
		
		/*
		UIButtonElement* button3 = new UIButtonElement(_tftPtr, width+5, 0, width-10, height-10, GREEN, "Temp", BLACK, 2, RED, "TempController2", "index");
		button3->drawElement(false);
		_uiElements[2] = button3;
		*/
		
		UINumValTimeElement* button3 = new UINumValTimeElement(_tftPtr, width+5, 0, width-10, height-10, GREEN, "Temp", BLACK, 2, RED, "", "");
		button3->setBoundVar(&(this->_tempDisimpegno));
		button3->setBoundTimeVar(&(this->_timeTempDisimpegno));
		button3->setUnit(String((char)248));
		button3->drawElement(false);
		_uiElements[2] = button3;		
		
		UIModNumValElement* button4 = new UIModNumValElement(_tftPtr, width+5, height+5, width-10, height-10, BLUE, "Settings", BLACK, 2, RED, "", "");
		button4->setBoundVar(&(this->_testValue));
		button4->setStep(0.1);
		button4->setDecreaseAction("decreaseVal");
		button4->setIncreaseAction("increaseVal");
		button4->drawElement(false);		
		_uiElements[3] = button4;
		return 0;
	}
	
	int MainController2::background(void) {
		long now = millis();
		if (now - this->_lastBackgroundRun > this->_backgroundRunInterval) {
			//Serial.print(this->_ctrName);
			//Serial.println(" doing background stuff...");
			EmonCMSModel* emonCMSModel = (EmonCMSModel*) _model;
			Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
			JsonObject & result = emonCMSModel->emonCmsFeed(10);
			float value = result["value"];
			long time = result["time"];
			Serial.println(time);
			Serial.println(value);
			Serial.println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
			this->_lastBackgroundRun = now;
			
			if (time > 0) {
				this->_tempDisimpegno = value;
				this->_timeTempDisimpegno = time;
				this->refresh();
			}
			
		}
		return 0;
	}	
	
	int MainController2::proxyAction() {
		//Serial.print("Calling action ");
		String action = _clickedElement->getAction();
		//Serial.println(action);
		
		if (action == "decreaseVal") {
			UIModNumValElement* elmPtr = (UIModNumValElement*) _mostExternalClickedElement;
			float step = elmPtr->getStep();
			float* boundVar = elmPtr->getBoundVar();
			//Serial.println(String(*boundVar));
			decreaseVal(boundVar, step);
			_mostExternalClickedElement->drawElement(false);
		}else if (action == "increaseVal") {
			UIModNumValElement* elmPtr = (UIModNumValElement*) _mostExternalClickedElement;
			float step = elmPtr->getStep();
			float* boundVar = elmPtr->getBoundVar();
			//Serial.println(String(*boundVar));
			increaseVal(boundVar, step);
			_mostExternalClickedElement->drawElement(false);
		}else{
			Serial.println("Unrecognized action !");
		}
	}
	
	// ACTIONS	
	int MainController2::decreaseVal(float* val, float step) {
		*val = *val - step;
		//Serial.println(*val);
	}

	int MainController2::increaseVal(float* val, float step) {
		*val = *val + step;
		//Serial.println(*val);
	}	

	int MainController2::mapUserCmd(TSPoint* p, char* ctrToRequestBuf) {
		return APIController2::mapUserCmd(p, ctrToRequestBuf);
	}
	
	int MainController2::index(void) {
		APIController2::index();
		MainController2::drawPage();
		return 0;
	}
}
