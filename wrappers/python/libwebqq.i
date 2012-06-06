%module (directors="1") libwebqqpython
%{
#include <stdio.h>
#include <Action.h>
#include <QQTypes.h>
%}

%feature("director") Action;
%include "Action.h"
%include "QQTypes.h"
