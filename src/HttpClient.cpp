/**
 * @file   HttpClient.cpp
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed May 30 17:05:14 2012
 *
 * @brief
 *
 *
 */

#include "HttpClient.h"
#include <iostream>
#include <string>
#include "QQDebug.h"
#include <vector>
#include <sstream>
#include "QQDebug.h"

int StrToInt(const std::string &str);

int StrToInt(const std::string &str)
{
	std::istringstream strm(str);
	int i = 0;
	if(!(strm >> i))
	{
		std::cerr<<"Unable to convert string '" + str + "' to integer!"<<
		    std::endl;
	}
	return i;
}

std::ostream & operator<<(std::ostream &strm, const Cookie &cook)
{
	strm << "Cookie: '" << cook.name << "' (secure: " << YesNo(cook.secure) << ", tail: "
         << YesNo(cook.tail) << ") for domain: '" << cook.domain << "', "
         << "path: '" << cook.path << "'.\n";
	strm << "Value: '" << cook.value << "'.\n";
	strm << "Expires: '" << ctime(&cook.expires) << "'.\n";
	return strm;
}


Cookie :: Cookie(const std::string &str_cookie)
{
	std::vector<std::string> vC = this->splitCookieStr(str_cookie);
	domain = vC[0];
	tail = vC[1] == "TRUE";
	path = vC[2];
	secure = vC[3] == "TRUE";
	expires = StrToInt(vC[4]);
	name = vC[5];
	if ( vC.size()> 6 )
		value = vC[6];
}

std::vector<std::string> & Cookie::split_cookie_str(const std::string &str, std::vector<std::string> &in)
{
	std::string part;

	std::istringstream strm(str);
	while (getline(strm, part, '\t'))
		in.push_back(part);

	return in;
}

std::vector<std::string> Cookie::splitCookieStr(const std::string &str)
{
	std::vector<std::string> split;
	this->split_cookie_str(str, split);
	return split;
}

std::vector<std::string> & Cookie::splitCookieStr(const std::string &str, std::vector<std::string> &in)
{
	return this->split_cookie_str(str, in);
}


HttpClient::HttpClient():request(NULL)
{
    curl_global_init(CURL_GLOBAL_ALL);
    cookies.clear();
    if ( request == NULL)
    {
        request = curl_easy_init();
        curl_easy_setopt(request, CURLOPT_NOSIGNAL, 1);
    }
    headers_slist = NULL;
}

HttpClient::HttpClient( std::list<std::string> cookie_list):request(NULL)
{
    curl_global_init(CURL_GLOBAL_ALL);
    cookies = cookie_list;
    if( request == NULL)
    {
        request = curl_easy_init();
        curl_easy_setopt(request, CURLOPT_NOSIGNAL, 1);
    }
    headers_slist = NULL;
}

HttpClient::~HttpClient()
{
    cookies.clear();
    if(request!=NULL)
        curl_easy_cleanup(request);
    curl_global_cleanup();
    if(headers_slist!=NULL)
        curl_slist_free_all(headers_slist);
}

void HttpClient::setHttpHeaders(std::list<std::string> &headers)
{
    for(std::list<std::string>::iterator it = headers.begin();
        it != headers.end();  ++it)
    {
        headers_slist = curl_slist_append(headers_slist, (*it).c_str());
    }
    curl_easy_setopt(request, CURLOPT_HTTPHEADER, headers_slist);
}

void HttpClient::addCookies(std::list<std::string> cookies)
{
    (this->cookies).insert(this->cookies.end(), cookies.begin(),cookies.end());
}

void HttpClient::addCookie(const std::string &cookie)
{
    cookies.push_back(cookie);
}

void HttpClient::perform()
{
    CURLcode ret = CURLE_OK;
    do
    {
        ret = curl_easy_setopt(request, CURLOPT_VERBOSE, 0);
        if(request==NULL)
        {
            std::cerr<<"Invalid Http Client"<<std::endl;
            break;
        }
        if(!cookies.empty())
        {
            for(std::list<std::string>::iterator it = cookies.begin();
                it != cookies.end();  ++it)
            {
                curl_easy_setopt(request, CURLOPT_COOKIELIST, (*it).c_str());
                //std::cout<<"Cookie to set: "<<(*it)<<std::endl;
            }
        }
        else
        {
            curl_easy_setopt(request, CURLOPT_COOKIELIST, "");
        }

        ret = curl_easy_perform(request);
        cookies.clear();
        if(ret!=CURLE_OK) break;
        if(headers_slist!=NULL)
        {
            curl_slist_free_all(headers_slist);
            headers_slist = NULL;
        }
        struct curl_slist *cookie_slist = NULL, *foreach_slist;
        ret = curl_easy_getinfo(request, CURLINFO_COOKIELIST, &cookie_slist);
        if(ret!=CURLE_OK) break;
        if(cookie_slist!=NULL)
        {
            for(foreach_slist=cookie_slist;foreach_slist!=NULL;
                foreach_slist=foreach_slist->next)
            {
                std::string cookie_str(foreach_slist->data);
                //std::cout<<"Cookie: "<<cookie_str<<std::endl;
                cookies.push_back(cookie_str);
            }
            curl_slist_free_all(cookie_slist);
        }
    }
    while(0);
    if(ret!=CURLE_OK)
        std::cout << curl_easy_strerror(ret) << std::endl;
}

void HttpClient::reset()
{
    curl_easy_reset(request);
}

std::string HttpClient::requestServer(const std::string & uri, const std::string body )
{
    std::string result;
    do
    {

        WriterMemoryClass mWriterChunk;

        if(request!=NULL)
        {
            debug_info("Uri: %s", uri.c_str());
            curl_easy_setopt(request, CURLOPT_URL, uri.c_str());
            //request->setOpt( new curlpp::options::Url(uri));
        }

        if ( body != std::string(""))
        {
            //std::cout<<body<<std::endl;
            curl_easy_setopt(request, CURLOPT_POSTFIELDS, body.c_str());
            curl_easy_setopt(request, CURLOPT_POSTFIELDSIZE, body.size());
        }
        
        curl_easy_setopt(request, CURLOPT_WRITEFUNCTION,
            &WriterMemoryClass::WriteMemoryCCallback);
        curl_easy_setopt(request, CURLOPT_WRITEDATA, &mWriterChunk);

        perform();
        result = mWriterChunk.getContent();
        debug_info("Result:%s", result.c_str());
        return result;
    }
    while(0);
}

bool HttpClient::getValueFromCookie( const std::string &key, std::string &value)
{
    for (std::list<std::string>::iterator it = cookies.begin();
         it != cookies.end();  ++it)
    {
        Cookie cookie = Cookie(*it);
        if ( cookie.name == key)
        {
            value =cookie.value;
            return true;
        }
    }
    return false;
}

std::list<std::string> HttpClient::dumpCookies()
{
    std::list<std::string> cookielist;
    cookielist = cookies;
    return cookielist;
}
