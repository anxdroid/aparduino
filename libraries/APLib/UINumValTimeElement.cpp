#include <UINumValTimeElement.h>

namespace APLib {
	int UINumValTimeElement::drawElement(bool clicked) {
		UIElement::drawElement(clicked);
		if (_boundVar != NULL) {
			_label = String(*_boundVar);
			_label += _unit;
			_timeLabel = String(*_boundTimeVar);
			//Serial.println(_label);
			int radius = _height / 10;
			_tftPtr->fillRoundRect(_tlX, _tlY, _width, _height, radius, (!clicked ? _bckgColor : _hoverColor));
			int labelX = _tlX+(_width/2);
			int labelY = _tlY+(_height/2)-2;
			_tftPtr->setCursor(labelX-(_label.length()*_labelSize*3), (labelY)-(_labelSize*5));
			_tftPtr->setTextSize(_labelSize);
			_tftPtr->setTextColor(_labelColor);
			_tftPtr->println(_label);
			
			labelY = _tlY+(_height/2)+2;
			_tftPtr->setCursor(labelX-(_timeLabel.length()*_labelSize*3), (labelY)-(_labelSize*5));
			_tftPtr->setTextSize(_labelSize);
			_tftPtr->setTextColor(_labelColor);
			_tftPtr->println(_timeLabel);
		}
		return 0;		
	}
}
