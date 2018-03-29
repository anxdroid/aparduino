#ifndef EMONCMS_MODEL_H
#define EMONCMS_MODEL_H

#include "Model.h"
#include <ArduinoJson.h>

namespace APLib {
	class EmonCMSModel : public Model {
		public:
			EmonCMSModel (void) {
				
			}
			JsonObject & EmonCMSModel::fetchData(String cmd);
			JsonObject & emonCmsFeed(int feedId);
			String getWiFiStatus();
		private:
			JsonObject& _jsonResult;
			StaticJsonBuffer<4096> _jsonBuffer;
			String _lastCmd = "";
			bool _lock = false;
			bool _waiting = false;
			
		
	};
}
#endif
