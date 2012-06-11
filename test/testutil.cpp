#include "QQUtil.h"
using namespace QQUtil;
#include <iostream>
#include <string>
#include "json/json.h"

int main()
{
    std::string str = "{\"h\":\"hello\",\"vfwebqq\":\"8be9f1d174467cd18921082b31d734571cf894ea21210a679665cfae3fb29b8f482f463f6048fb\"}";
    std::string result = urlencode(str);
    std::cout<<result<<std::endl;
    std::cout<<urldecode(result)<<std::endl;
    Json::Value root;
    root["h"]= "hello";
    root["vfwebqq"]="8be9f1d174467cd18921082b31d734571cf894ea21210a679665cfae3fb29b8f482f463f6048fb";
    std::cout<<root<<std::endl;
    Json::FastWriter writer;
    result = writer.write(root);
    std::cout<<urlencode(result)<<std::endl;
}
