#include <APIController2.h>

namespace APLib {
	bool UIElement::isClicked(TSPoint *p, UIElement** clickedElement) {
		*clickedElement = NULL;
		for (int i = 0; i < UIELEMENTSSIZE; i++) {
			if (_uiElements[i] != NULL) {
				if (_uiElements[i]->isClicked(p, clickedElement)) {
					//Serial.print("Found clicked subelement: ");
					//Serial.println(_uiElements[i]->getLabel());
					*clickedElement = _uiElements[i];
					return true;
				}
			}
		}
		if (
			*clickedElement == NULL && 
			p->x >= _tlX && p->x <= (_tlX+_width) && 
			p->y >= _tlY && p->y <= (_tlY+_height)
			) 
		{
			*clickedElement = this;
			return true;
		}
		return false;
	}
	
	int UIElement::drawElement(bool clicked) {
		// Draw subelements
		for (int i = 0; i < UIELEMENTSSIZE; i++) {
			if (_uiElements[i] != NULL) {
				_uiElements[i]->drawElement(clicked);
			}
		}
	}
}
