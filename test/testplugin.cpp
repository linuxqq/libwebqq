/**
 * @file   testplugin.cpp
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed Jun 13 13:02:45 2012
 *
 * @brief
 *
 *
 */

#include "QQPlugin.h"

int main()
{
    QQPlugin * plugin = Singleton<QQPlugin>::getInstance();
    plugin->webqq_login("1421032531", "1234567890");

    plugin->send_buddy_message("1446372341","test");
    delete plugin;
}
