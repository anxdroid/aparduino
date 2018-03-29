#ifndef UI_NUMVAL_ELEMENT_H
#define UI_NUMVAL_ELEMENT_H
#include <APIController2.h>

namespace APLib {
	class UINumValElement : public UIElement {
		public : 
			UINumValElement(Adafruit_TFTLCD* tftPtr, int tlX, int tlY, int width, int height, long bckgColor, const char* label, long labelColor, int labelSize, long hoverColor, const char* controller, String action) : UIElement(tftPtr, tlX, tlY, width, height, bckgColor, label, labelColor, labelSize, hoverColor, controller, action)
			{
				setRefresheable(true);
			}
			
			virtual int drawElement(bool);
			
			void setBoundVar(float* boundVar) {
				_boundVar = boundVar;
				_label = String(*_boundVar);
			}
			
			float* getBoundVar() {
				return _boundVar;
			}
			
			void setUnit(String unit) {
				_unit = unit;
			}
			
			String getUnit() {
				return _unit;
			}			
			
		protected :
			float* _boundVar = NULL;
			String _unit = "";
	};
}
#endif;
