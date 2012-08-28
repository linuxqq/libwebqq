/**
 * @file   libwebqqboost.cpp
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Fri Aug 10 09:47:19 2012
 *
 * @brief
 *
 *
 */


#include <boost/python.hpp>
using namespace boost::python;
#define BOOST_PYTHON_STATIC_LIB
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <utility>
#include <iterator>
#include <boost/python.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/call.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
using namespace boost::python;
using namespace std;
#include "QQTypes.h"
#include "QQPlugin.h"
#include "Singleton.h"
#include "QQEventQueue.h"

template<class T1, class T2>
struct PairToTupleConverter {
    static PyObject* convert(const std::pair<T1, T2>& pair) {
        return incref(make_tuple(pair.first, pair.second).ptr());
    }
};


class SingletonInstance
{
public:

    ResourceManager & getResManagerSingletonInstance()
    {
        return *(Singleton<ResourceManager>::getInstance());
    }

    QQPlugin & getQQPluginSingletonInstance()
    {
        return * (Singleton<QQPlugin>::getInstance());
    }
};


BOOST_PYTHON_MODULE(libwebqqboost)
{

    enum_<QQEvent>("QQEvent")
        .value("ON_BUDDY_MESSAGE", ON_BUDDY_MESSAGE)
        .value("ON_GROUP_MESSAGE", ON_GROUP_MESSAGE)
        .value("ON_SEND_MESSAGE", ON_SEND_MESSAGE)
        .value("ON_AVATAR_UPDATE", ON_AVATAR_UPDATE)
        .value("ON_BUDDY_STATUS_CHANGE", ON_BUDDY_STATUS_CHANGE)
        .value("ON_NICK_CHANGE" , ON_NICK_CHANGE)
        .value("ON_SHAKE_MESSAGE" , ON_SHAKE_MESSAGE);

    class_<Birthday>("Birthday")
        .def(init<>())
        .def_readwrite("year", & Birthday::year)
        .def_readwrite("month", & Birthday::month)
        .def_readwrite("day", & Birthday::day);

    class_<QQBuddy>("QQBuddy")
        .def(init<>())
        .def_readwrite("uin",&QQBuddy::uin)               //the uin. Change every login
        .def_readwrite("qqnumber", &QQBuddy::qqnumber)
        .def_readwrite("status" , &QQBuddy::status)
        .def_readwrite("is_vip", & QQBuddy::is_vip)
        .def_readwrite("vip_level", & QQBuddy::vip_level)
        .def_readwrite("nick", & QQBuddy::nick)
        .def_readwrite("markname", &QQBuddy::markname)
        .def_readwrite("country", & QQBuddy::country)
        .def_readwrite("province", & QQBuddy::province)
        .def_readwrite("city", &QQBuddy::city)

        .def_readwrite("gender", &QQBuddy::gender)
        .def_readwrite("face", & QQBuddy::face)
        .def_readwrite("flag", & QQBuddy::flag)

        .def_readwrite("birthday", &QQBuddy::birthday)
        .def_readwrite("blood", &QQBuddy::blood)
        .def_readwrite("shengxiao", & QQBuddy::shengxiao)

        .def_readwrite("constel", &QQBuddy::constel)
        .def_readwrite("phone" , & QQBuddy::phone)
        .def_readwrite("mobile" , &QQBuddy::mobile)
        .def_readwrite("email" , & QQBuddy::email)
        .def_readwrite("occupation" , & QQBuddy::occupation)
        .def_readwrite("college", &QQBuddy::college)
        .def_readwrite("homepage", & QQBuddy::homepage)
        .def_readwrite("personal", &QQBuddy::personal)
        .def_readwrite("lnick" , &QQBuddy::lnick)
        .def_readwrite("allow" , &QQBuddy::allow)
        .def_readwrite("cate_index", &QQBuddy::cate_index)
        .def_readwrite("client_type", &QQBuddy::client_type);

    class_<QQCategory>("QQCategory")
        .def(init<>())
        .def_readwrite("name", &QQCategory::name)
        .def_readwrite("index", &QQCategory::index);

    class_<QQGroup>("QQGroup")
        .def(init<>())
        .def_readwrite("name", &QQGroup::name)
        .def_readwrite("gid", &QQGroup::gid)
        .def_readwrite("gnumber", &QQGroup::gnumber)
        .def_readwrite("code", &QQGroup::code)
        .def_readwrite("flag", &QQGroup::flag)
        .def_readwrite("onwner", &QQGroup::owner)
        .def_readwrite("mark", &QQGroup::mark)
        .def_readwrite("mask", &QQGroup::mask)
        .def_readwrite("option", &QQGroup::option)
        .def_readwrite("createtime" , &QQGroup::createtime)
        .def_readwrite("gclass", &QQGroup::gclass)
        .def_readwrite("level", &QQGroup::level)
        .def_readwrite("face", &QQGroup::face)
        .def_readwrite("memo", &QQGroup::memo)
        .def_readwrite("fingermemo", &QQGroup::fingermemo);


    class_<QQEventQueue>("QQEventQueue")
        .def(init<size_t>())
        .def("push", &QQEventQueue::push)
        .def("pop", &QQEventQueue::pop)
        .def("size", &QQEventQueue::size)
        .def("empty", &QQEventQueue::empty);

    class_<std::map<std::string, QQBuddy> > ("map_contacts")
        .def(boost::python::map_indexing_suite<std::map<std::string, QQBuddy> >());

    class_<std::map<std::string, QQGroup> > ("map_groups")
        .def(boost::python::map_indexing_suite<std::map<std::string, QQGroup > >());

    class_<std::map<int, QQCategory> > ("map_categories")
        .def(boost::python::map_indexing_suite<std::map<int , QQCategory> >());

    class_<std::map<std::string , std::map<std::string , QQBuddy> > > ("map_group_contact")
        .def(boost::python::map_indexing_suite< std::map< std::string, std::map<std::string, QQBuddy> > >() );

    to_python_converter<std::pair<QQEvent, std::string>, PairToTupleConverter<QQEvent, std::string> >();

    class_<ResourceManager, boost::noncopyable>("ResourceManager")
        .def(init<>())
        .def_readwrite("contacts", &ResourceManager::contacts)
        .def_readwrite("groups", & ResourceManager::groups)
        .def_readwrite("categories", &ResourceManager::categories)
        .def_readwrite("group_contacts", &ResourceManager::group_contacts)
        .def("lock", & ResourceManager::lock)
        .def("ulock" , & ResourceManager::ulock)
        .def_readwrite("event_queue", &ResourceManager::event_queue);

    class_<QQPlugin, boost::noncopyable>("QQPlugin")
        .def("webqq_login", &QQPlugin::webqq_login)
        .def("send_buddy_message", &QQPlugin::send_buddy_message)
        .def("send_group_message", &QQPlugin::send_group_message)
        .def("send_buddy_nudge", &QQPlugin::send_buddy_nudge)
        .def("set_long_nick", &QQPlugin::set_long_nick)
	.def("change_status", &QQPlugin::change_status)
        ;

    class_<SingletonInstance, boost::noncopyable> ("SingletonInstance")
        .def("getResManagerSingletonInstance", &SingletonInstance::getResManagerSingletonInstance,
             boost::python::return_internal_reference<>())
        .def("getQQPluginSingletonInstance", &SingletonInstance::getQQPluginSingletonInstance,
             boost::python::return_internal_reference<>());
}
