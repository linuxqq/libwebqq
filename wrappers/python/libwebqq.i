%module (directors="1") libwebqqpython
%{
#include <stdio.h>
#include <Action.h>
#include <QQTypes.h>
#include <Singleton.h>
%}

%feature("director") Action;
%include "Singleton.h"
%include "Action.h"
%include "QQTypes.h"
%template(SingletonQQConfig) Singleton<QQConfig>;
