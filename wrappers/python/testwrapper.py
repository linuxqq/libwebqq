#!/usr/bin/python

from libwebqqpython import *

class OnMessage():

    func = None
    
    def __init__( self):
        #Action.__init__(self)
        #self.func = func
        pass
        
    def __call__(self,data):
        print data

class Worker:
    plugin = None
    res = None
    
    def __init__(self):
        
        self.plugin = SingletonQQPlugin_getInstance()
        self.res = SingletonResourceManager_getInstance()
        self.res.lock()
	#f = OnMessage()
        #a= Action()
        #a.setCallback(self.on_message)
        self.register_handler(ON_RECEIVE_MESSAGE, self.on_message)
        
        self.res.ulock()
        if not self.res.event_adapter.is_event_registered(ON_RECEIVE_MESSAGE) :
            print "Fail to register event"

    def login(self):
        self.plugin.webqq_login("1421032531","1234567890")
    def register_handler(self, event, func):
        adapter = self.res.event_adapter
        print adapter
        adapter.register_event_handler(event, func)
    def on_message(self,data):
        print "test on_message " + data 

worker = Worker()
worker.login()
