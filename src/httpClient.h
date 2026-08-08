#ifndef _HTTPCLIENT_H_
#define _HTTPCLIENT_H_

#include <curl/curl.h>
#include <memory>
#include <string>

class curlHandler {
private:
	using CurlHandle = std::unique_ptr<CURL, decltype(&curl_easy_cleanup)>;
	class global {
		public:
			global() {
				auto result = curl_global_init(CURL_GLOBAL_ALL);
				if (result != CURLE_OK) {
					throw std::runtime_error("Failed to create client");
				}

			};
			~global() {
				curl_global_cleanup();
		};
	};
	// ważna kolejność: najpierw global, później curl!
	global global;
	CurlHandle curl;
	std::array<char, CURL_ERROR_SIZE> error;

public:
	curlHandler() : curl(CurlHandle(curl_easy_init(), &curl_easy_cleanup)) {
		auto result = curl_easy_setopt(curl.get(), CURLOPT_ERRORBUFFER, error);
		if (result != CURLE_OK) {
			throw std::runtime_error("Failed to set error buffer");
		}

	}
	CURL *get() const {return curl.get();}
	std::string getErrorInfo() {
		return static_cast<std::string>(error.data());
	}
};

class httpClient {
public:
	httpClient(const std::string &url);
	std::string getPage();
private:
	using curlHeaders = std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)>;
	curlHeaders headers{
		curl_slist_append(
			nullptr,
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"),
		    &curl_slist_free_all
	};
	static std::size_t writer(char *data, size_t size, size_t nmemb, void *writerData);
	curlHandler curl;
};

#endif // _HTTPCLIENT_H_
