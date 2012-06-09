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
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include "QQDebug.h"
#define CURLPP_ALLOW_NOT_AVAILABLE

#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>

int StrToInt(const std::string &str);

int StrToInt(const std::string &str)
{
	std::istringstream strm(str);
	int i = 0;
	if (!(strm >> i)) {
		throw curlpp::RuntimeError("Unable to convert string '" + str + "' to integer!");
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
    cookies.clear();
    //request = new curlpp::Easy();
}

HttpClient::~HttpClient()
{
    cookies.clear();
}

void HttpClient::setOptions(std::vector<curlpp::OptionBase *> options) throw(curlpp::RuntimeError)
{
    request->setOpt(options.begin(), options.end());
}

void HttpClient::addCookies(std::list<std::string> cookies)
{

    (this->cookies).insert(this->cookies.end(), cookies.begin(),cookies.end());

}
void HttpClient::perform()
{
    try
    {
        curlpp::Cleanup myCleanup;
        request->setOpt(new curlpp::options::Verbose(true));
        if ( request)
        {
            if ( !cookies.empty())
            {
                for (std::list<std::string>::iterator it = cookies.begin();
                     it != cookies.end();  ++it)
                {
                    request->setOpt(curlpp::options::CookieList(*it));
                }
            }
            else{
                request->setOpt(curlpp::options::CookieList(""));
            }
        }
        else {
            std::cerr<<"Invalid Http Client"<<std::endl;
        }

        request->perform();
        cookies.clear();
        curlpp::infos::CookieList::get(*request, cookies);

    }
    catch(curlpp::RuntimeError & e)
    {
        std::cout << e.what() << std::endl;
    }
    catch(curlpp::LogicError & e)
    {
        std::cout << e.what() << std::endl;
    }
}

void HttpClient::reset()
{
    request = new curlpp::Easy();
    //request -> setOpt(new curlpp::options::Verbose(true));
}

std::string HttpClient::requestServer(const std::string & uri, const std::string body )
{
    try{

        WriterMemoryClass mWriterChunk;

        if ( request)
        {
	    debug_info("Uri:%s", uri.c_str());
            request->setOpt( new curlpp::options::Url(uri));
        }

        if ( body != std::string(""))
        {
	    std::cout<<body<<std::endl;
	    request->setOpt(new curlpp::options::PostFields(body));
	    request->setOpt(new curlpp::options::PostFieldSize(body.size()));
            //std::istringstream myStream(body);
            //request->setOpt(new curlpp::options::ReadStream(&myStream));
            //request->setOpt(new curlpp::options::InfileSize(body.size()));
            //request->setOpt(new curlpp::options::Upload(true));
        }
        curlpp::types::WriteFunctionFunctor functor(&mWriterChunk,
                                                    &WriterMemoryClass::WriteMemoryCallback);
        curlpp::options::WriteFunction *test = new curlpp::options::WriteFunction(functor);
        request->setOpt(test);
        delete test;

        perform();
        return mWriterChunk.getContent();
    }
    catch(curlpp::RuntimeError & e)
    {
        std::cout << e.what() << std::endl;
    }
    catch(curlpp::LogicError & e)
    {
        std::cout << e.what() << std::endl;
    }

}

bool HttpClient::getValueFromCookie( const std::string &key, std::string &value)
{
    for (std::list<std::string>::iterator it = cookies.begin();
         it != cookies.end();  ++it)
    {
        Cookie cookie = Cookie(*it);
        //std::cout<<cookie<<std::endl;
        if ( cookie.name == key)
        {
            value =cookie.value;
            return true;
        }
    }
    return false;
}
