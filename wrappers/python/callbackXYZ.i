/*
Typemaps to wrap boost::function.

IN typemap (Python callable case)
----------

In order to convert a Python callable to a boost::function, we first wrapper the
Python callable into a C++ callable (we do this by instantiating a Functor and
make its operator() method call the Python callable), then create a
boost::function using this Functor.

     Python callable
            ^      
            |      
       FunctorXYZ
            ^      
            |      
     boost::function

OUT typemap
-----------

To convert a boost::function to a Python callable, we make use of a dedicated 
Python type, BoostFunction_DDD_retD. This Python object is just a callable 
(ie has a C/C++ function in its tp_call slot). BoostFunction_DDD_retD store the
boost::function in its member attribute 'cb', which is called when its tp_call
function is invoked.

       boost::function
              ^
              |
    BoostFunction_DDD_retD 


IN typemap (C++ wrapped callable case)
---------- 

If now we pass a BoostFunction_DDD_retD to a C++ function that take a
boost::fucntion as argument, we just extract the cb member which
is a boost::function and pass it to the C++ function.
*/

/*==============================================================================
Functor classes
==============================================================================*/

/*
In order to pass a Python callable to a C++ function that take a boost::function
as argument, we instantiate a C++ Functor and make it call the Python callable.
*/

%fragment("PyCallbackError","header") {
    /*
    C++ exception to be raised if call to Python callable fails.
    */
    class PyCallbackError: public exception
    {
        public:
            PyCallbackError(){};
            PyCallbackError(const char * str,const char * file,int line):
                str_(str),
                file_(file),
                line_(line)
            {};
            virtual ~PyCallbackError() throw() {};
            virtual const char* what() const throw()
            {
              ostringstream msg;
              msg << file_ << ":" << line_ << ": " << str_ ;
              return msg.str().c_str();
            };
        private:
            const char *str_;
            const char *file_;
            int line_;
    };
}

%fragment("FunctorXYZ","header",fragment="PyCallbackError") {
    /* 
    C++ Functor that wrap the Python callable.
    */
    
    class FunctorXYZ {
    
    public:
        FunctorXYZ(): _pycallable(NULL) { }
        ~FunctorXYZ(){ } 
    
        double operator() (double x, double y, double z) const {
    
            /* convert C++ args to Python arg */
            PyObject *pyarglist = Py_BuildValue("(d,d,d)", x,y,z);
    
            /* call Python function */
            PyObject *pyresult = PyObject_CallObject(_pycallable, pyarglist);
            /* If an error occurs in the Python callback, the most external C++ function
             * that has called this FunctorXYZ::operator() MUST return NULL, so
             * Python can detect there is an error.
             * To do so, we throw a C++ exception that has to be ultimatly catch
             * and converted to "return NULL" (so Python will examine its static
             * exceptions objects and will see the exception raised in the
             * Python callback.) . This can be done with the
             * %exception Swig directive in the Swig script.
             */
            if (pyresult==NULL) {
                PyCallbackError functorError("Failed to call Python callback",__FILE__,__LINE__);
                throw functorError;
            }
    
            /* convert Python result to C++ result */
            double result;
            if (PyFloat_Check(pyresult)!=0) {
                result = PyFloat_AS_DOUBLE(pyresult);
    
            } else if (PyInt_Check(pyresult)!=0) {
                result = (double) PyInt_AS_LONG(pyresult);
    
            } else {
                PyErr_SetString(PyExc_TypeError, "Python callback must return a float.");
                PyCallbackError functorError("Python callback must return a float.",__FILE__,__LINE__);
                throw functorError;
            }
    
            return result;
        };
    
        void setPyFunc(PyObject *pycallable){
            Py_XINCREF(pycallable);    /* Add a reference to the new callback */
            Py_XDECREF(_pycallable);   /* Dispose of previous callback */
            _pycallable = pycallable;  /* Remember new callback */
        };
    
    private:
        PyObject *_pycallable;
    };
}

/*==============================================================================
BoostFunction_DDD_retD Python object
==============================================================================*/

%fragment("BoostFunction_DDD_retD","header") {
    /*
    Python type to wrap boost::function

    It has a cb member that store a boost::function when getting a C++
    boost::function into Python.

    When giving this Python type as an C++ boost::function function argument,
    the cb member is just passed.

    This object has a tp_call method (the function invoked when the object is 
    called from Python) that calls the boost::function member.
    */

    typedef struct {
        PyObject_HEAD
        ternaryfunc tp_call;
        boost::function<double (double,double,double)> *cb;
    } boostFunction_BoostFunction_DDD_retD_Object;

    static PyObject *
    boostfunction_call_DDD_retD(
        boostFunction_BoostFunction_DDD_retD_Object *self,
        PyObject *args, PyObject *kw
        )
    {
        /* 
        This is the method that is invoke when the Python type is called

        It just call the boost::function member, and return the value.
        */
        double x,y,z;

        if (! PyArg_ParseTuple(args,"ddd", &x,&y,&z) )
            return NULL;

        double res;

        try {
            res = (*(self->cb))(x,y,z);
        }

        catch (PyCallbackError &e) {
           /* A exception is already raise in the Python callback */
           return NULL;
        }

        return Py_BuildValue("d", res);
    }

    static PyTypeObject boostFunction_BoostFunction_DDD_retD_Type = {
        PyObject_HEAD_INIT(NULL)
        0,                         /*ob_size*/
        "demo.BoostFunction_DDD_retD",   /*tp_name*/
        sizeof(boostFunction_BoostFunction_DDD_retD_Object), /*tp_basicsize*/
        0,                         /*tp_itemsize*/
        0,                         /*tp_dealloc*/
        0,                         /*tp_print*/
        0,                         /*tp_getattr*/
        0,                         /*tp_setattr*/
        0,                         /*tp_compare*/
        0,                         /*tp_repr*/
        0,                         /*tp_as_number*/
        0,                         /*tp_as_sequence*/
        0,                         /*tp_as_mapping*/
        0,                         /*tp_hash */
        (ternaryfunc)boostfunction_call_DDD_retD,   /*tp_call*/
        0,                         /*tp_str*/
        0,                         /*tp_getattro*/
        0,                         /*tp_setattro*/
        0,                         /*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT,        /*tp_flags*/
        "boost::function<double (double,double,double) wrapper", /* tp_doc */
    };
}


/*==============================================================================
Typemaps for IN const CallbackXYZ &
==============================================================================*/

%typemap(in,fragment="FunctorXYZ,BoostFunction_DDD_retD") const CallbackXYZ & {

   /* is the Python argument a boost::function<double (double,double,double)>
    * wrapper?
    */
    if (PyType_IsSubtype($input->ob_type, &boostFunction_BoostFunction_DDD_retD_Type)) {
       $1 = new CallbackXYZ();
       $1 = ((boostFunction_BoostFunction_DDD_retD_Object*)$input)->cb;

    } else {
       /* check the input is a callable */
       if (!PyCallable_Check($input)) {
          PyErr_SetString(PyExc_TypeError, "argument must be callable");
          return NULL;
       }
       /*
       Examples of others tests that we can NOT perform:
       - inspect.getargspec($input): works only for function, not functor.
       - calling the function with arbitrary args to detect an error sooner:
         If the call modify attributes of a functor, second call may produce wrong results.
         Anyway, Python callable could be modified inplace beetween this check and
         its really call.
       */

       /* 
        * boost::function FcuntorXYZ is responsible to delete the Python callable 
        * wrapper FunctorXYZ.
        * So we need a smart pointer.
        */
       boost::shared_ptr<FunctorXYZ> fxyz_ptr( new FunctorXYZ() );

       /*
        * Configure our C++ Functor to call Python callable
        */
       fxyz_ptr->setPyFunc($input);

       
       /*
        * Make a boost::function with our C++ Functor
        */
       $1 = new CallbackXYZ(*fxyz_ptr);
   }

}

/*==============================================================================
Typemaps for OUT (CallbackXYZ &)
==============================================================================*/

%typemap(out,fragment="FunctorXYZ,BoostFunction_DDD_retD") CallbackXYZ & {
   /* instantiate a boostFunction_BoostFunction_DDD_retD_Object and store
      the boost::function in it */

   Py_INCREF(Py_None);
   Py_INCREF(Py_None);

   if (PyType_Ready(&boostFunction_BoostFunction_DDD_retD_Type) < 0)
       return NULL;

   boostFunction_BoostFunction_DDD_retD_Object* boostfunc;
   boostfunc = (boostFunction_BoostFunction_DDD_retD_Object*) 
       PyType_GenericNew(&boostFunction_BoostFunction_DDD_retD_Type, Py_None, Py_None);

   boostfunc->cb = $1;

   $result = (PyObject*) boostfunc;
}

/*==============================================================================
Typemaps for TYPECHECK const CallbackXYZ
==============================================================================*/

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const CallbackXYZ & {
   $1 = PyCallable_Check($input) ? 1 : 0;
}
