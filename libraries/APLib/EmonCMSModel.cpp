#include <EmonCMSModel.h>

namespace APLib {
	JsonObject & EmonCMSModel::fetchData(String cmd) {
		if (!this->_lock) {
			
			String status = this->getWiFiStatus();
			Serial.print("Status: ");
			Serial.println(status);
			if (status != "") {
				this->_lock = true;
				Serial.println(cmd);
				Serial3.println(cmd);
				this->_waiting = true;
				this->_lastCmd = cmd;
				return _jsonBuffer.parseObject("");
			}
		}
		
		if (this->_lock && this->_waiting && Serial3.available()) {
			String res = Serial3.readString();
			if (res != "connection failed") {
				Serial.println(res);
				this->_waiting = false;
				this->_lock = false;
				return _jsonBuffer.parseObject(res);
			}
		}		
	}
	
	JsonObject & EmonCMSModel::emonCmsFeed(int feedId) {
		String cmd = "emonCmsFeed;";
		cmd += feedId;
		return EmonCMSModel::fetchData(cmd);
	}
	
	String EmonCMSModel::getWiFiStatus() {
		String cmd = "getWiFiStatus;0";
		long start = millis();
		Serial3.println(cmd);
		while (!Serial3.available() && ((millis() - start) < 3000));
		String res = Serial3.readString();		
		return res;
	}
}
