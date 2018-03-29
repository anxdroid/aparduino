#ifndef API_CONTROLLER2_H
#define API_CONTROLLER2_H

#include <SPFD5408_TouchScreen.h>
#include <SPFD5408_Adafruit_TFTLCD.h>
#include <GraphicController.h>
#include "Model.h"
#include "APDefinitions.h"


#define CTRNAMESIZE 20
#define UIELEMENTSSIZE 5
#define CALL_MEMBER_FN(object,ptrToMember)  ((object)->*(ptrToMember))

namespace APLib {
	
	class APIController2;
	typedef int (APIController2::*actionPtr)(String, long, double);
	
	class UIElement {
		public : 
			//
			UIElement(Adafruit_TFTLCD* tftPtr, int tlX, int tlY, int width, int height, long bckgColor, const char* label, long labelColor, int labelSize, long hoverColor, const char* controller, String action)
			{
				_tftPtr = tftPtr;
				_tlX = tlX;
				_tlY = tlY;
				_width = width;
				_height = height;
				_bckgColor = bckgColor;
				_label = label;
				_labelColor = labelColor;
				_labelSize = labelSize;
				_hoverColor = hoverColor;
				_controller = controller;
				_action = action;
				for (int i = 0; i < UIELEMENTSSIZE; i++) {
					_uiElements[i] = NULL;
				}
			  
			}
			
			~UIElement() {
				//Serial.println("Getting destroyed !!");
				cleanUIElements();
			}
			
			virtual bool isClicked(TSPoint *p, UIElement** clickedElement);
			
			virtual int drawElement(bool);
			
			String getLabel() {
				return _label;
			}
			
			String getController() {
				return _controller;
			}
			
			String getAction() {
				return _action;
			}
			
			bool isRefresheable() {
				return _refresheable;
			}
			
			void setRefresheable(bool refresheable) {
				_refresheable = refresheable;
			}
		protected :
			Adafruit_TFTLCD* _tftPtr = NULL;
			int _tlX;
			int _tlY;
			int _width;
			int _height;
			long _bckgColor;
			String _label;
			long _labelColor;
			int _labelSize;
			long _hoverColor;
			String _controller;
			String _action;
			bool _refresheable = false;
			UIElement* _uiElements[UIELEMENTSSIZE];
			
			void cleanUIElements() {
				for (int i = 0; i < UIELEMENTSSIZE; i++) {
					if (_uiElements[i] != NULL) {
						delete(_uiElements[i]);
					}
				}
			}			
	};
	
	class APIController2 {
		public : 
			APIController2(Adafruit_TFTLCD* tftPtr, GraphicController* graphicCtr)
			{
			  _tftPtr = tftPtr;
			  _graphicCtr = graphicCtr;
			  for (int i = 0; i < UIELEMENTSSIZE; i++) {
				_uiElements[i] = NULL;
			  }
			}

			int waitPage() {
				int labelSize = 2;
				_tftPtr->fillScreen(BLACK);
				String label = "Wait..";
				_tftPtr->setCursor((_tftPtr->width()/2)-(label.length()*labelSize*3), (_tftPtr->height()/2)-(labelSize*5));
				_tftPtr->setTextSize(labelSize);
				_tftPtr->setTextColor(WHITE);
				_tftPtr->println(label);
			}
			
			virtual int drawPage() = 0;
			
			virtual int proxyAction() {
				//Serial.print("Calling action ");
				//Serial.println(_clickedElement->getAction());
			}
			
			virtual int mapUserCmd(TSPoint* p, char* ctrToRequestBuf) {
				/*
				Serial.print(p->x);
				Serial.print(",");
				Serial.println(p->y);
				*/
				memset(ctrToRequestBuf, 0, CTRNAMESIZE);
				_mostExternalClickedElement = NULL;
				_clickedElement = NULL;
				for (int i = 0; i < UIELEMENTSSIZE; i++) {
					if (_uiElements[i] != NULL) {
						if (_uiElements[i]->isClicked(p, &(this->_clickedElement))) {
							_mostExternalClickedElement = _uiElements[i];
							// Action taken from subelement (if existant)
							String action = _clickedElement->getAction();
							_uiElements[i]->getController().toCharArray(ctrToRequestBuf, CTRNAMESIZE);
							//Serial.println(action);
							if (action == "index" && (strcmp(ctrToRequestBuf, "") != 0)) {
								// New controller
								_uiElements[i]->drawElement(true);
								return 1;
							}else{
								// Internal action
								long interTime = millis() - _lastActRequestTime;
								if (_lastRequestedAct != action || interTime > 150) {
									this->proxyAction();
									_lastActRequestTime = millis();
									_lastRequestedAct = action;
								}
							}
						}
					}
				}
				return 0;
			}
			
			virtual int index() {
				APIController2::waitPage();
				_lastRefreshTime = millis();
				
				//Serial.print("Hi I am ");
				//Serial.println(APIController2::_ctrName);
				return 0;
			}
			
			int refresh() {
				_lastRefreshTime = millis();
				for (int i = 0; i < UIELEMENTSSIZE; i++) {
					if (_uiElements[i] != NULL) {
						if (_uiElements[i]->isRefresheable()) {
							_uiElements[i]->drawElement(false);
						}
					}
				}

				return 0;
			}
				
			virtual int background() {
				return 0;
			}
			
			int getCtrId() {
				return _ctrId;
			}
			
			void setCtrId(int ctrId) {
				_ctrId = ctrId;
			}
		
			int getCtrName(const char* ctrNameBuf) {
				memset(ctrNameBuf, 0, CTRNAMESIZE); 
				strcpy(ctrNameBuf, APIController2::_ctrName);			
				return 0;
			}
			
			void cleanUIElements() {
				for (int i = 0; i < UIELEMENTSSIZE; i++) {
					if (_uiElements[i] != NULL) {
						delete(_uiElements[i]);
					}
				}
			}

			bool isInBackground() {
				return _inBackground;
			}
			
			bool setInBackground(bool inBackground) {
				_inBackground = inBackground;
			}
			
			void setCtrName(const char* ctrName) {
				strcpy(APIController2::_ctrName, ctrName);
			}
			
			Model* getModel() {
				return _model;
			}
			
			inline void setModel(Model* model) {
				_model = model;
			}
			
			inline long getRefreshTime() {
				return _refreshTime;
			}
			
			inline void setRefreshTime(long refreshTime) {
				_refreshTime = refreshTime;
			}
			
			inline bool shouldRefresh() {
				long now = millis();
				if (now - _lastRefreshTime > _refreshTime) {
					return true;
				}
				return false;
			}
	
		protected :
			Adafruit_TFTLCD* _tftPtr = NULL;
			GraphicController* _graphicCtr = NULL;
			Model* _model;
			long _refreshTime = 60000;
			long _lastRefreshTime = millis();
			UIElement* _uiElements[UIELEMENTSSIZE];
			long _lastActRequestTime = 0;
			String _lastRequestedAct = "";
			char _ctrName[CTRNAMESIZE];
			int _ctrId = 0;
			bool _inBackground = false;

			int _backgroundRunInterval = 5000;
			long _lastBackgroundRun = 0;
			
			UIElement* _clickedElement = NULL;
			UIElement* _mostExternalClickedElement = NULL;
			
			
	};	
}
#endif