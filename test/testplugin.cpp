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
#include "QQDebug.h"
int main()
{
    QQPlugin * plugin = Singleton<QQPlugin>::getInstance();
    plugin->webqq_login("1421032531", "1234567890");
/*
    if (plugin->send_buddy_message("553934476","fuck") )
    {
        debug_info("Success!");
    }
    else
        debug_error("Fail");
*/
    delete plugin;
}
