#include "httpClient.h"

#include <stdexcept>


namespace {
	void checkCURLresult(const CURLcode result, const std::string_view description) {
		if (result != CURLE_OK) {
		    throw std::runtime_error{
			std::string(description) +
			std::to_string(static_cast<int>(result)) +
			std::string(", error = ") +
			curl_easy_strerror(result)
		    };
		}

	};
};

httpClient::httpClient(const std::string &url) {
	checkCURLresult(
		curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str()),
		"Failed to set page address"
	);

	checkCURLresult(
		curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L),
		"Failed to set option :"
	);

	const std::string agent =
		"Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
		"AppleWebKit/537.36 (KHTML, like Gecko) "
		"Chrome/117.0.0.0 Safari/537.36";
	curl_easy_setopt(curl.get(), CURLOPT_USERAGENT, agent.data());
	curl_slist_append(headers.get(), "Accept-Language: pl-PL,pl;q=0.9,en-US;q=0.8,en;q=0.7");

	checkCURLresult(
		curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headers.get()),
		"Failed to set  headers:" + curl.getErrorInfo()
	);

	checkCURLresult(
		curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, &httpClient::writer),
		"Failed to set writer :" + curl.getErrorInfo()
	);
}

std::string httpClient::getPage() {
	std::string buffer;
	CURLcode result;
	result = curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &buffer);
	if (result != CURLE_OK) {
	    throw std::runtime_error("Failed to set write data :" + curl.getErrorInfo());
	}
	result = curl_easy_perform(curl.get());
	if (result != CURLE_OK) {
	    throw std::runtime_error{
		"Failed to get page: " +
		std::to_string(static_cast<int>(result)) +
		", error = " +
		curl_easy_strerror(result)
	    };
	}

	return buffer;
}

std::size_t httpClient::writer(char *data,  size_t size,  size_t nmemb,  void *toWrite) {
	if (!toWrite) return 0;
	std::string *tmp = reinterpret_cast<std::string *>(toWrite);
	tmp->append(data, size * nmemb);
	return size * nmemb;
}
