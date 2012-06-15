%module (directors="1") libwebqqpython
%{
#include <stdio.h>
#include <Action.h>
#include <QQTypes.h>
#include <Singleton.h>
#include <QQPlugin.h>
#include <json/json.h>
%}
%include stl.i
%include "std_string.i"
%feature("director") Action;
%include "Singleton.h"
%include "Action.h"
%include "QQTypes.h"
%include "QQPlugin.h"
%include "json/json.h"
%template(SingletonQQConfig) Singleton<QQConfig>;
%template(SingletonQQPlugin) Singleton<QQPlugin>;
