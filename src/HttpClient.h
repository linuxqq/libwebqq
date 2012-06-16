/**
 * @file   HttpClient.h
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed May 30 17:05:44 2012
 *
 * @brief
 *
 *
 */
#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "Singleton.h"

#include <cstring>

#include <cstdlib>
#include <curl/curl.h>


#define MAX_FILE_LENGTH 20000

class WriterMemoryClass
{
public:
    WriterMemoryClass()
    {
        this->m_pBuffer = NULL;
        this->m_pBuffer = (char*) malloc(MAX_FILE_LENGTH * sizeof(char));
        this->m_Size = 0;
    };

    ~WriterMemoryClass()
    {
        if (this->m_pBuffer)
            free(this->m_pBuffer);
    };

    void* Realloc(void* ptr, size_t size)
    {
        if(ptr)
            return realloc(ptr, size);
        else
            return malloc(size);
    };

    size_t WriteMemoryCallback(char* ptr, size_t size, size_t nmemb)
    {
        size_t realsize = size * nmemb;

        m_pBuffer = (char*) Realloc(m_pBuffer, m_Size + realsize);

        if (m_pBuffer == NULL) {
            realsize = 0;
        }
        memcpy(&(m_pBuffer[m_Size]), ptr, realsize);
        m_Size += realsize;
        return realsize;
    };

    void debug()
    {
        std::cout << "Size: " << m_Size << std::endl;
        std::cout << "Content: " << std::endl << m_pBuffer << std::endl;
    }

    std::string getContent()
    {
        return std::string(m_pBuffer);
    }
    
    static size_t WriteMemoryCCallback(char *ptr, size_t size, size_t nmemb,
        void *instance)
    {
        if(instance==NULL) return -1;
        WriterMemoryClass *wm_instance = reinterpret_cast<WriterMemoryClass *>
            (instance);
        return wm_instance->WriteMemoryCallback(ptr, size, nmemb);
    }
    char* m_pBuffer;
    size_t m_Size;
};



class YesNo
{
public:
    explicit YesNo(bool yn) : yesno(yn) {}
    std::string operator()() const {
        return yesno ? "Yes" : "No";
    }
    friend std::ostream &operator<<(std::ostream &strm, const YesNo &yn) {
        strm << yn();
        return strm;
    }
private:
    bool yesno;
};

struct Cookie
{
	std::string name;
	std::string value;
	std::string domain;
	std::string path;
	time_t expires;
	bool tail;
	bool secure;

    friend std::ostream & operator<<(std::ostream &strm, const Cookie &cook);

    explicit Cookie(const std::string & cookies);

private:
    Cookie();
    std::vector<std::string> splitCookieStr(const std::string &str);
    std::vector<std::string> & splitCookieStr(const std::string &str, std::vector<std::string> &in);
    std::vector<std::string> & split_cookie_str(const std::string &str, std::vector<std::string> &in);
};

class HttpClient:public Singleton<HttpClient>
{
    friend class Singleton<HttpClient>;
    std::list<std::string> cookies;
    CURL *request;
    struct curl_slist *headers_slist;

public:

    HttpClient();

    HttpClient(std::list<std::string> cookie_list);

    ~HttpClient();

    //void setOptions(std::vector<curlpp::OptionBase *> options);
    void setHttpHeaders(std::list<std::string> &headers);
    void addCookie(const std::string &cookie);
    void addCookies(std::list<std::string> cookies);
    bool  getValueFromCookie(const std::string  & key , std::string & value);

    std::list<std::string> dumpCookies();

    std::string requestServer( const std::string & uri, const std::string body ="");

private:

    void reset ();

    void perform();
};
#endif
