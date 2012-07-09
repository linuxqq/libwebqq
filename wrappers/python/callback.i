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
            PyObject *pyarglist = Py_BuildValue("(s)", str.c_str());
            PyObject_CallObject(_pycallable, pyarglist);
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
    typedef struct {
        PyObeject_HEAD
        ternaryfunc tp_call;
        boost::function<void (std::string)> * cb;
    } boostFunction_BoostFunction_S_retV_Object;

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
        "libwebqqpython.BoostFunction_S_retV",   /*tp_name*/
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

%typemap(in,fragment="Functor,BoostFunction_S_retV") const EventListener & {
    
    if (PyType_IsSubtype($input->ob_type, &boostFunction_BoostFunction_S_retV_Type)) {
        $1 = new EventListener();
        $1 = ((boostFunction_BoostFunction_S_retV_Object*)$input)->cb;

    } else {
        /* check the input is a callable */
        if (!PyCallable_Check($input)) {
            PyErr_SetString(PyExc_TypeError, "argument must be callable");
            return NULL;
        }
      
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

%typemap(out,fragment="Functor,BoostFunction_S_retV") EventListener & {
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

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const EventListener & {
    $1 = PyCallable_Check($input) ? 1 : 0;
 }
