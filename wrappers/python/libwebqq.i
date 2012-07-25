%module (directors="1") libwebqqpython
%{
#define SWIG_FILE_WITH_INIT
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <sstream>
#include <stdio.h>
#include <QQAction.h>
#include <QQTypes.h>
#include <Singleton.h>
#include <QQPlugin.h>
#include <json/json.h>
#include <QQEventQueue.h>
#include <SmartPtr.h>
class PyCallback
{
    PyObject *func;
    PyCallback& operator=(const PyCallback&); // Not allowed
    public:
    PyCallback(const PyCallback& o) : func(o.func) {
         Py_XINCREF(func);
    }
    PyCallback(PyObject *func) : func(func) {
         Py_XINCREF(this->func);
         assert(PyCallable_Check(this->func));
    }
    ~PyCallback() {
         Py_XDECREF(func);
    }
    void operator()(const std::string& s) {
         if (!func || Py_None == func || !PyCallable_Check(func))
              return;
         PyObject *args = Py_BuildValue("(s)", s.c_str());
         PyObject *result = PyObject_Call(func,args,0);
         Py_DECREF(args);
         Py_XDECREF(result);
    }
};
%}

%include "QQAction.h"
%include "callback.i"
%include stl.i
%include "std_list.i"
%include "std_string.i"
%include "std_map.i"
%include "std_pair.i"
%include "Singleton.h"
%include "QQTypes.h"
%include "QQPlugin.h"
%include "QQEventQueue.h"
%include "json/json.h"
%include "SmartPtr.h"

%feature("director") Action;
%pythonprepend Action::setAction %{
        args[1].thisown=0
%}
%extend Action {
  void setCallback(PyObject *callback) {
      $self->setCallback(PyCallback(callback));
      }
}
%extend Adapter {
void register_event_handler(const QQEvent& e, PyObject *callback) {
     $self->register_event_handler(e, PyCallback(callback));
     }
}

%template(SingletonQQConfig) Singleton<QQConfig>;
%template(SingletonQQPlugin) Singleton<QQPlugin>;
%template(SingletonResourceManager) Singleton<ResourceManager>;
namespace std{
    %template(map_int_qqcategory) map<int,QQCategory>;
    %template(map_string_qqgroup) map<std::string , QQGroup>; 
    %template(map_string_qqbuddy) map<std::string , QQBuddy>;
    %template(pair_int_string) pair<int, std::string>;
    %template(list_pair_event_string) list<std::pair<int, std::string>  >;
    %template(pair_event_string) pair<QQEvent, std::string>;
}
%template() std::pair<swig::SwigPtr_PyObject, swig::SwigPtr_PyObject>;
%template(pymap) std::map<swig::SwigPtr_PyObject, swig::SwigPtr_PyObject>;
%template(pylist) std::list< swig::SwigPtr_PyObject >;

