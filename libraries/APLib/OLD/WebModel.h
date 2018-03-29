#ifndef WEBMODEL_H
#define WEBMODEL_H

#include <Model.h>

namespace APLib {
	class WebModel : public Model {
		public:
			WebModel(String url) {
				WebModel::setUrl(url);
			}
			WebModel();
			
			inline void setUrl() {
				_url = url;
			}
			
			inline String getUrl() {
				return _url;
			}
			virtual String fetchData(String cmd) = 0;
		private:
			String _url = "";
	
	};
}
#endif
