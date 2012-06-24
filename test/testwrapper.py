#!/usr/bin/python

from libwebqqpython import *

class OnMessage(Action):

    func = None
    
    def __init__( self, func):
        #Action.__init__(self)
        self.func = func
        
    def run(self):
        print self.data

class Worker:
    plugin = None
    res = None
    
    def __init__(self):
        
        self.plugin = SingletonQQPlugin_getInstance()
        self.res = SingletonResourceManager_getInstance()
        self.res.lock()
        self.register_handler(ON_RECEIVE_MESSAGE, self.on_message)
        self.res.ulock()
        if not self.res.event_adapter.is_event_registered(ON_RECEIVE_MESSAGE) :
            print "Fail to register event"

    def login(self):
        self.plugin.webqq_login("1421032531","1234567890")
    def register_handler(self, event, func):
        
        p_action =  OnMessage(func)
        action = ActionPtr(id(p_action))
        adapter = self.res.event_adapter
        print adapter
        adapter.register_event_handler(event, action)
    def on_message(self):
        print "test on_message"

worker = Worker()
worker.login()
