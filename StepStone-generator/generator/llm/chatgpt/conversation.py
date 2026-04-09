from openai import OpenAI

import time
import logging
from generator.llm import *

class Conversation(LLM):
    def __init__(self, client, console, logger, engine, temperature, max_tokens):
        super().__init__(logger)
        self._engine = engine
        self._temperature = temperature
        self._max_tokens = max_tokens
        self._chat_history = None
        self.console = console
        self.client: OpenAI = client
        super().set_api_key(self.client.api_key)
    
    def chat(self, message, **kwargs):
        self.append_user_message(message)
        self.logger.info("> ChatGPT: {}".format(message))
        response = self._send_request(formatted_messages=self.messages, **kwargs)
        response_text = response.choices[0].message.content
        self.logger.info("> Received: {}".format(response_text))
        self.append_assistant_message(response_text)
        return response_text
        
    @log_message
    def _append_message(self, message, role):
        self.messages.append({"role": role, "content": message})
    
    @check_api
    def _send_request(self, formatted_messages, model=None, temperature=None):
        time.sleep(0.2) # avoid rate limit
        
        if model == None:
            model = self._engine
        if temperature == None:
            temperature = self._temperature
        self.logger.debug("[engine: {}] SENDING: {}".format(model, '\n'.join([e['content'] for e in formatted_messages])))
        try:
            response = self.client.chat.completions.create(model=model,
            messages=formatted_messages,
            temperature=temperature,
            top_p=1,
            frequency_penalty=0,
            presence_penalty=0)
        except Exception as e:
            logging.error(e)
            return None

        self.logger.debug("RECEIVING: {}".format(response.choices[0].message.content))
        return response