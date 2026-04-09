import threading
from generator.error import *
from .error import *

def check_api(func):
    def inner(self, **kwargs):
        if self._api_key == None:
            raise API_KEY_IS_MISSING()
        return func(self, **kwargs)
    return inner

def parse_formatted_messages(formatted_messages, field='content'):
    res = ''
    for msg in formatted_messages:
        res += msg[field] + '\n'
    return res

def log_message(func):
    def inner(self, message, role):
        if self.console.verbose:
            if role == 'user':
                self.console.user_print(message)
            else:
                self.console.chatgpt_response_print(message)
        return func(self, message, role)
    return inner 
    
class LLM():
    def __init__(self, logger=None) -> None:
        self.logger = logger
        self.messages = []
        self._api_key = None
        self._chat_session = {}
        self._chat_index = 0
        self._msg_header = None
        self._chat_lock = threading.Lock()
    
    def set_api_key(self, api_key):
        self._api_key = api_key
        
    @check_api
    def get_api_key(self):
        return self._api_key
    
    def set_logger(self, logger):
        self.logger = logger
    
    def chat_session(self, session_id) -> list:
        return self._chat_session[session_id]
    
    def lock_chat_session(self):
        self._chat_lock.acquire()
    
    def unlock_chat_session(self):
        self._chat_lock.release()
        
    def append_user_message(self, message):
        if self._msg_header != None:
            message = "{}\n{}".format(self._msg_header, message)
        self._append_message(message=message, role='user')
        
    def append_assistant_message(self, message):
        if self._msg_header != None:
            message = "{}\n{}".format(self._msg_header, message)
        self._append_message(message=message, role='assistant')
    
    def append_system_message(self, message):
        if self._msg_header != None:
            message = "{}\n{}".format(self._msg_header, message)
        self._append_message(message=message, role='system')
    
    def create_message_header(self, header):
        self._msg_header = header
        
    def pop_message(self, num):
        for _ in range(num):
            self.messages.pop()
    
    def chat(self, message, **kwargs):
        raise UnuseableModule("LLM")