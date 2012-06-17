/**
 * @file   QQUtil.cpp
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Mon Jun 11 14:53:42 2012
 *
 * @brief
 *
 *
 */

#include "QQUtil.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

namespace QQUtil{

    void urlInPlaceDecode(std::string& s);

    void Tokenize(const std::string& str,
                  std::vector<std::string>& tokens,
                  const std::string& delimiters = " ")
    {
        // Skip delimiters at beginning.
        std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
        // Find first "non-delimiter".
        std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

        while (std::string::npos != pos || std::string::npos != lastPos)
        {
            // Found a token, add it to the vector.
            tokens.push_back(str.substr(lastPos, pos - lastPos));
            // Skip delimiters.  Note the "not_of"
            lastPos = str.find_first_not_of(delimiters, pos);
            // Find next "non-delimiter"
            pos = str.find_first_of(delimiters, lastPos);
        }
    }

    std::string char2hex( char dec )
    {
        char dig1 = (dec&0xF0)>>4;
        char dig2 = (dec&0x0F);
        if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48 in ascii
        if (10<= dig1 && dig1<=15) dig1+=65-10; //A,65 in ascii
        if ( 0<= dig2 && dig2<= 9) dig2+=48;
        if (10<= dig2 && dig2<=15) dig2+=65-10;

        std::string r;
        r.append( &dig1, 1);
        r.append( &dig2, 1);
        return r;
    }

    std::string urlencode( const std::string &c )
    {

        std::string escaped;
        int max = c.length();
        for(int i=0; i<max; i++)
        {
            if ( (48 <= c[i] && c[i] <= 57) ||
                 (65 <= c[i] && c[i] <= 90) ||
                 (97 <= c[i] && c[i] <= 122) ||
                 (c[i]=='~' || c[i]=='-' || c[i]=='_' || c[i]=='.')
                )
            {
                escaped.append( &c[i], 1);
            }
            else
            {
                escaped.append("%");
                escaped.append( char2hex(c[i]) );
            }
        }
        return escaped;
    }


    std::string urldecode(const std::string& s)
    {
        std::string result(s);
        urlInPlaceDecode(result);
        return result;
    }

    void urlInPlaceDecode(std::string& s)
    {
        std::string::size_type w=0;
        for (std::string::size_type p=0; p < s.size(); p++,w++) {
            if (s[p] == '+'){
                s[w]= ' ';  // translate '+' to ' ':
            } else if ((s[p] == '%') && (p + 2 < s.size())) {  // check length
                char str[]={s[p+1],s[p+2],0};
                char *pp;
                unsigned char c=(unsigned char)strtoul(str,&pp,16);
                if(pp==str+2){
                    s[w]=c;
                    p+=2;
                } else {
                    s[w]=s[p];
                }
            }else {
                s[w]=s[p];
            }
        }
        s.erase(w);
    }



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

    std::string IntToStr(int number)
    {
        std::stringstream ss;//create a stringstream
        ss << number;//add number to the stream
        return ss.str();//return a string with the contents of the stream
    }
};
