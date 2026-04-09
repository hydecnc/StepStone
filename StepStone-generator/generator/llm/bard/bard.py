import requests

from generator.llm import LLM
from bardapi import Bard
from generator.error import *

class LLMBard(LLM):
    URL = "https://bard.googleapis.com/v1/generate"
    def __init__(self) -> None:
        super().__init__()
        self.bard = None
        
    def __str__(self) -> str:
        return "bard"
    
    def set_api_key(self, api_key):
        super().set_api_key(api_key=api_key)
        try:
            self.bard = Bard(token=api_key)
        except:
            raise API_KEY_IS_MISSING("bard token")
    
    def verify_api(self):
        return self.bard != None
    
    def start_new_session(self):
        index = self._chat_index
        self.lock_chat_session()
        session = requests.Session()
        session.headers = {
                    "Host": "bard.google.com",
                    "X-Same-Domain": "1",
                    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.114 Safari/537.36",
                    "Content-Type": "application/x-www-form-urlencoded;charset=UTF-8",
                    "Origin": "https://bard.google.com",
                    "Referer": "https://bard.google.com/",
                }
        session.cookies.set("__Secure-1PSID", self.get_api_key()) 

        bard = Bard(token=self.get_api_key(), session=session, timeout=30)
        self._chat_session[self._chat_index] = bard
        self._chat_index += 1
        self.unlock_chat_session()
        return index
    
    def chat(self, session_id, message=None, system=None, assistant=None, **kwargs):
        bard: Bard = self.chat_session(session_id)
        response_text = bard.get_answer(message)['content']
        return response_text 
    
    def append_message(self, session_id, message=None, system=None, assistant=None):
        bard: Bard = self.chat_session(session_id=session_id)
        bard.ask(text=message)