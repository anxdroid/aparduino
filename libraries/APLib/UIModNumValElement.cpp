#include <UIModNumValElement.h>
#include <UIButtonElement.h>

namespace APLib {
	int UIModNumValElement::drawElement(bool clicked) {
		
		cleanUIElements();
		
		int width = _width/5;
		int height = _height;
		
		UIButtonElement* buttonMinus = new UIButtonElement(_tftPtr, _tlX, _tlY, width, height, WHITE, "-", BLUE, 2, RED, "", _decreaseAction);
		buttonMinus->drawElement(false);
		_uiElements[0] = buttonMinus;
		
		UIButtonElement* buttonPlus = new UIButtonElement(_tftPtr, _tlX+_width-width, _tlY, width, height, WHITE, "+", BLUE, 2, RED, "", _increaseAction);
		buttonPlus->drawElement(false);
		_uiElements[1] = buttonPlus;
		
		int numWidth = 3 * (_width) / 5;
		
		UINumValElement* numVal = new UINumValElement(_tftPtr, _tlX+width, _tlY, numWidth, height, BLACK, "", WHITE, 2, RED, "", "");
		numVal->setBoundVar(_boundVar);
		numVal->drawElement(false);
		_uiElements[2] = numVal;

		UIElement::drawElement(clicked);
		return 0;		
	}
}
