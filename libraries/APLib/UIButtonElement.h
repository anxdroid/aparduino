#ifndef UI_BUTTON_ELEMENT_H
#define UI_BUTTON_ELEMENT_H
#include <APIController2.h>

namespace APLib {
	class UIButtonElement : public UIElement {
		public : 		
			UIButtonElement(Adafruit_TFTLCD* tftPtr, int tlX, int tlY, int width, int height, long bckgColor, const char* label, long labelColor, int labelSize, long hoverColor, const char* controller, String action) : UIElement(tftPtr, tlX, tlY, width, height, bckgColor, label, labelColor, labelSize, hoverColor, controller, action)
			{
				
			}
			
			virtual int UIButtonElement::drawElement(bool clicked);
	};
}
#endif;
