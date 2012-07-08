%module (directors="1") libwebqqpython
%{
#define SWIG_FILE_WITH_INIT
#include <boost/shared_ptr.hpp>
#include <sstream>
#include <stdio.h>
#include <Action.h>
#include <QQTypes.h>
#include <Singleton.h>
#include <QQPlugin.h>
#include <json/json.h>
#include <SmartPtr.h>
%}
%include "Action.h"
%include stl.i
%include "std_string.i"
%include "std_map.i"
%include "std_pair.i"
%include "Singleton.h"
%include "QQTypes.h"
%include "QQPlugin.h"
%include "json/json.h"
%include "SmartPtr.h"

%feature("director") Action;
%pythonprepend Action::setAction %{
        args[1].thisown=0
%}

%template(SingletonQQConfig) Singleton<QQConfig>;
%template(SingletonQQPlugin) Singleton<QQPlugin>;
%template(SingletonResourceManager) Singleton<ResourceManager>;
namespace std{
        %template(map_string_qqgroup) map<std::string , QQGroup>; 
        %template(map_string_qqbuddy) map<std::string , QQBuddy>;
}
%template() std::pair<swig::SwigPtr_PyObject, swig::SwigPtr_PyObject>;
%template(pymap) std::map<swig::SwigPtr_PyObject, swig::SwigPtr_PyObject>;

%template(ActionPtr) SmartPtr<Action>;
%include "callbackXYZ.i"
