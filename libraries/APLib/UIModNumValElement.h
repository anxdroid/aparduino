#ifndef UI_MOD_NUMVAL_ELEMENT_H
#define UI_MOD_NUMVAL_ELEMENT_H
#include <UINumValElement.h>

namespace APLib {
	class UIModNumValElement : public UINumValElement {
		public : 		
			UIModNumValElement(Adafruit_TFTLCD* tftPtr, int tlX, int tlY, int width, int height, long bckgColor, const char* label, long labelColor, int labelSize, long hoverColor, const char* controller, String action) : UINumValElement(tftPtr, tlX, tlY, width, height, bckgColor, label, labelColor, labelSize, hoverColor, controller, action)
			{
				
			}
			
			virtual int drawElement(bool clicked);
			
			void setDecreaseAction(String decreaseAction) {
				_decreaseAction = decreaseAction;
			}
			
			String getDecreaseAction() {
				return _decreaseAction;
			}
			
			void setIncreaseAction(String increaseAction) {
				_increaseAction = increaseAction;
			}
			
			String getIncreaseAction() {
				return _increaseAction;
			}

			void setStep(float step) {
				_step = step;
			}
			
			float getStep() {
				return _step;
			}			
		private :
			String _decreaseAction = "";
			String _increaseAction = "";
			float _step = 0.1;
	};
}
#endif;
