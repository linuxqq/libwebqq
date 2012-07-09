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

%fragment("Functor","header",fragment="PyCallbackError") {
    /* 
       C++ Functor that wrap the Python callable.
    */
    
    class Functor {
    
    public:
    Functor(): _pycallable(NULL) { }
        ~Functor(){ } 
        
        void operator() (std::string str ) const{

            PyObject *pyarglist = Py_BuildValue("(%s)", str.c_str());
            PyObject_callObject(_pycallable, pyarglist);
            return;
        }
        
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

  ==============================================================================*/

%fragment("BoostFunction_S_retV","header") {
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
        PyObeject_HEAD
        ternaryfunc tp_call;
        boost::function<void (std::string)> * cb;
    } boostFunction_BoostFunction_VS_retV_Object;

    static PyObject *
        boostfunction_call_S_retV(
            boostFunction_BoostFunction_S_retV_Object *self,
            PyObject *args, PyObject *kw
            )
    {
        char * ptr =NULL;
        if( ! PyArg_ParseTuple(args, "s", ptr))
            return ;
        try{
            (*(self->cb))(ptr);
        }catch ( PyCallbackError & e)
         {
             delete ptr;
             return;
         }
        delete ptr;
    }

    static PyTypeObject boostFunction_BoostFunction_S_retV_Type ={
        PyObject_HEAD_INIT(NULL)
        0,                         /*ob_size*/
        "demo.BoostFunction_S_retV",   /*tp_name*/
        sizeof(boostFunction_BoostFunction_S_retV_Object), /*tp_basicsize*/
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
        (ternaryfunc)boostfunction_call_S_retV,   /*tp_call*/
        0,                         /*tp_str*/
        0,                         /*tp_getattro*/
        0,                         /*tp_setattro*/
        0,                         /*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT,        /*tp_flags*/
        "boost::function<void (std::string) wrapper", /* tp_doc */
    };
 }


/*==============================================================================
  Typemaps for IN const Callback &
  ==============================================================================*/

%typemap(in,fragment="Functor,BoostFunction_S_retV") const EventListerner & {

    /* is the Python argument a boost::function<double (double,double,double)>
     * wrapper?
     */
    if (PyType_IsSubtype($input->ob_type, &boostFunction_BoostFunction_S_retV_Type)) {
        $1 = new EventListener();
        $1 = ((boostFunction_BoostFunction_S_retV_Object*)$input)->cb;

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
        boost::shared_ptr<Functor> fxyz_ptr( new Functor() );

        /*
         * Configure our C++ Functor to call Python callable
         */
        fxyz_ptr->setPyFunc($input);

       
        /*
         * Make a boost::function with our C++ Functor
         */
        $1 = new EventListener(*fxyz_ptr);
    }

 }

/*==============================================================================

  ==============================================================================*/

%typemap(out,fragment="Functor,BoostFunction_S_retV") EventListerner & {
    /* instantiate a boostFunction_BoostFunction_DDD_retD_Object and store
       the boost::function in it */

    Py_INCREF(Py_None);
    Py_INCREF(Py_None);

    if (PyType_Ready(&boostFunction_BoostFunction_S_retV_Type) < 0)
        return NULL;

    boostFunction_BoostFunction_S_retV_Object* boostfunc;
    boostfunc = (boostFunction_BoostFunction_S_retV_Object*) 
                PyType_GenericNew(&boostFunction_BoostFunction_S_retV_Type, Py_None, Py_None);
    boostfunc->cb = $1;

    $result = (PyObject*) boostfunc;
 }

/*==============================================================================

  ==============================================================================*/

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const EventListerner & {
    $1 = PyCallable_Check($input) ? 1 : 0;
 }
