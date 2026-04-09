from openai import OpenAI

import time
import logging
from generator.llm import *
from .conversation import Conversation
from .assistant import ChatGPTAssistant

class ChatGPT(LLM):
    OP_INIT = 0
    OP_SERLZ = 1
    OP_FOLUP = 2
    OP_CRTFMT = 3
    def __init__(self, engine='gpt-4-turbo', temperature=1, max_tokens=None):
        super().__init__()
        self._engine = engine
        self._temperature = temperature
        self._max_tokens = max_tokens
        self._chat_history = None
        self.client = None
        #logging.basicConfig(level=logging.DEBUG)

    def __str__(self) -> str:
        return "chatgpt"
    
    def set_api_key(self, api_key):
        super().set_api_key(api_key=api_key)
        self.client = OpenAI(api_key=self._api_key)
    
    def verify_api(self) -> bool:
        return self.client != None

    def start_new_assistant_session(self, engine=None, temperature=None, max_tokens=None) -> ChatGPTAssistant:
        return self.start_new_session(type='assistant', engine=engine, temperature=temperature, max_tokens=max_tokens)
    
    def start_new_conversation_session(self, engine=None, temperature=None, max_tokens=None) -> Conversation:
        return self.start_new_session(type='conversation', engine=engine, temperature=temperature, max_tokens=max_tokens)

    def start_new_session(self, type: str, console, logger, engine=None, temperature=None, max_tokens=None) -> LLM:
        self.lock_chat_session()
        if engine == None:
            engine = self._engine
        if temperature == None:
            temperature = self._temperature
        if max_tokens == None:
            max_tokens = self._max_tokens
        if type == "conversation":
            self._chat_session[self._chat_index] = Conversation(self.client, console, logger, engine, temperature, max_tokens)
        if type == "assistant":
            self._chat_session[self._chat_index] = ChatGPTAssistant(self.client, console, logger, engine, temperature, max_tokens)
        try:
            session = self._chat_session[self._chat_index]
        except:
            raise ChatTypeError()
        self._chat_index += 1
        self.unlock_chat_session()
        return session