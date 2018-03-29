#ifndef UI_NUMVAL_TIME_ELEMENT_H
#define UI_NUMVAL_TIME_ELEMENT_H
#include <UINumValElement.h>

namespace APLib {
	class UINumValTimeElement : public UINumValElement {
		public : 
			UINumValTimeElement(Adafruit_TFTLCD* tftPtr, int tlX, int tlY, int width, int height, long bckgColor, const char* label, long labelColor, int labelSize, long hoverColor, const char* controller, String action) : UINumValElement(tftPtr, tlX, tlY, width, height, bckgColor, label, labelColor, labelSize, hoverColor, controller, action)
			{
				setRefresheable(true);
			}
			
			virtual int drawElement(bool);
			
			long* getBoundTimeVar() {
				return _boundTimeVar;
			}

			void setBoundTimeVar(long* boundTimeVar) {
				_boundTimeVar = boundTimeVar;
			}			
			
		protected :
			long* _boundTimeVar = NULL;
			String _timeLabel = "";
	};
}
#endif;
