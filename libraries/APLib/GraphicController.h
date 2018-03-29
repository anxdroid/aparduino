#ifndef GRAPHIC_CONTROLLER_H
#define GRAPHIC_CONTROLLER_H

#include <SPFD5408_TouchScreen.h>
#include <SPFD5408_Adafruit_TFTLCD.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

namespace APLib {
	class GraphicController {
	  public :
	  	inline GraphicController(Print &printer) {

		}
		void drawChart(Adafruit_TFTLCD* tftPtr, JsonVariant* jsonMasterDataPayloadPtr, long jsonMasterTSStart, int chartWidth, int chartHeight, int topMargin, int leftMargin, int bottomMargin, int rightMargin, int labelSize) {
		  
		  tftPtr->fillScreen(BLACK);

		  if (chartWidth+leftMargin+rightMargin > tftPtr->width()) {
		      chartWidth = tftPtr->width() - leftMargin - rightMargin;
		  }
		  if (chartHeight+topMargin+bottomMargin > tftPtr->height()) {
		      chartHeight = tftPtr->height() - topMargin - bottomMargin;
		  }
		  
		  //Serial.print("Chart size: ");
		  Serial.print(chartWidth);
		  Serial.print("x");
		  Serial.println(chartHeight);
		  
		  TSPoint chartTopLeft(leftMargin, topMargin, 0);
		  TSPoint chartBottomRight(leftMargin+chartWidth, topMargin+chartHeight, 0);
		  
		  tftPtr->drawRect(chartTopLeft.x, chartTopLeft.y, chartBottomRight.x-10, chartBottomRight.y-10, WHITE);
		  long minTS = NULL;
		  long maxTS = NULL;
		  double minVal = NULL;
		  double maxVal = NULL;
		  
		  for( JsonVariant& element : jsonMasterDataPayloadPtr->as<JsonArray>() ) {
		    JsonArray* measurement = &element.as<JsonArray>();
		    double value = (*measurement)[1];
		    long offset = (*measurement)[0];
		    time_t timestamp = offset + jsonMasterTSStart;
		    if (minTS == NULL || timestamp < minTS ) {
		      minTS = timestamp;
		    }
		    if (maxTS == NULL || timestamp > maxTS ) {
		      maxTS = timestamp;
		    }    
		    if (minVal == NULL || value < minVal ) {
		      minVal = value;
		    }
		    if (maxVal == NULL || value > maxVal ) {
		      maxVal = value;
		    }
		  }

		  tftPtr->setTextSize(labelSize);
		  tftPtr->setTextColor(WHITE);
		  String timeLabel = "";
		  timeLabel += hour(minTS);
		  timeLabel += ":";
		  timeLabel += minute(minTS);
		  tftPtr->setCursor(chartTopLeft.x, chartBottomRight.y+10);
		  tftPtr->println(timeLabel);

		  timeLabel = "";
		  timeLabel += hour(maxTS);
		  timeLabel += ":";
		  timeLabel += minute(maxTS);
		  int labelWidth = labelSize * 6 * timeLabel.length();
		  tftPtr->setCursor(chartBottomRight.x-labelWidth, chartBottomRight.y+10);  
		  tftPtr->println(timeLabel);
		  
		  int meanX = (chartTopLeft.x+chartBottomRight.x) / 2;
		  double meanTS = (minTS+maxTS) / 2;
		  timeLabel = "";
		  timeLabel += hour(meanTS);
		  timeLabel += ":";
		  timeLabel += minute(meanTS);
		  labelWidth = labelSize * 6 * timeLabel.length();
		  tftPtr->setCursor(meanX-(labelWidth/2), chartBottomRight.y+10);
		  tftPtr->println(timeLabel);

		  tftPtr->setCursor(chartTopLeft.x+5, chartBottomRight.y-20);
		  tftPtr->println(minVal);  

		  tftPtr->setCursor(chartTopLeft.x+5, chartTopLeft.y+5);
		  tftPtr->println(maxVal); 
		  
		  int meanY = (chartBottomRight.y-20+chartTopLeft.y+5) / 2;
		  double meanVal = (minVal+maxVal) / 2;
		  
		  tftPtr->setCursor(chartTopLeft.x+5, meanY);
		  tftPtr->println(meanVal);
		  

		  TSPoint previousPoint(0, 0, 0);
		  long previousDataColor = 0;
		  int pointCount = 0;
		  for( JsonVariant& element : jsonMasterDataPayloadPtr->as<JsonArray>() ) {
		    JsonArray* measurement = &element.as<JsonArray>();
		    char* color = (*measurement)[2];
		    double value = (*measurement)[1];
		    long offset = (*measurement)[0];
		    
		    time_t timestamp = offset + jsonMasterTSStart;
		    
		    long dataColor = 0;
		    //Serial.println(color);
		    if (*color == 'Y') {
		      dataColor = YELLOW;
		    }
		    if (*color == 'B') {
		      dataColor = BLUE;
		    }
		    if (*color == 'W') {
		      dataColor = WHITE;
		    }
		    if (*color == 'R') {
		      dataColor = RED;
		    }
		    //Serial.println(dataColor);
		    
		    double diff = timestamp-minTS;
		    double ratio = diff/(maxTS-minTS);
		    int x = chartTopLeft.x+((chartBottomRight.x-chartTopLeft.x)*ratio);
		        
		    diff = value-minVal;
		    //Serial.println(diff);
		    ratio = diff/(maxVal-minVal);
		    //Serial.println(ratio);
		    
		    int y = chartBottomRight.y - ((chartBottomRight.y-chartTopLeft.y)*ratio);
		    //tftPtr->drawLine(x, y, x, chartBottomRight.y, dataColor);
		    if (pointCount > 0 && dataColor == previousDataColor) {
		      tftPtr->drawLine(x, y, previousPoint.x, previousPoint.y, dataColor);
		    }
		    previousPoint.x = x;
		    previousPoint.y = y;
		    previousDataColor = dataColor;
		    pointCount++;
		  }	  
		}
	};
}
#endif