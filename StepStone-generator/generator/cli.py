import sys, os
import datetime
import logging
import re

from generator.controller.library_controller import LibraryController
from generator.llm.chatgpt import ChatGPT
from generator.llm.bard import LLMBard
from generator.config import Config
from generator.error import *

class Cli():
    def __init__(self, debug, args, bug_fb=False):
        self._output = args.output
        self.config_file = args.config
        self._llm_name = args.llm
        self._engine = args.engine
        self._join_syzlang_json = args.join_syzlang_json
        self._refer_syzlang_context = args.refer_syzlang_context
        self.bug_fb = bug_fb
        self.chat_type = args.chat_type
        self.cfg = Config(self.config_file, self._llm_name)
        self.llm = self._init_llm()
        self.case_logger = None
        self.verbose = args.verbose
        self._patches = []
        self.case_path = None
        self._init_workspace()
        self.controller: LibraryController = None
    
    def generate_syzlange(self, cuda_api_text):
        self.case_logger = self._init_logger()
        self._init_library_controller(self.debug)
        self.controller.analyze(cuda_api_text, self.chat_type, self._join_syzlang_json, self._refer_syzlang_context)   
        
    def user_print(self, message, **kwargs):
        self.case_logger.info(message)
    
    def chatgpt_response_print(self, response, **kwargs):
        self.case_logger.info(response)
    
    def _init_workspace(self):
        if self._output == None:
            now = datetime.datetime.now()
            dt_string = now.strftime("%d-%m-%Y-%H-%M-%S")
            self._output = os.getcwd() + "/chat_history-" + dt_string
            os.makedirs(self._output)
        else:
            os.makedirs(self._output, exist_ok=True)
    
    def _init_llm(self):
        api_key = self.cfg.find_api_key()
        llm = None
        if self._llm_name == "chatgpt":
            if self._engine != None:
                llm = ChatGPT(engine=self._engine)
            else:
                llm = ChatGPT()
        if self._llm_name == "bard":
            llm = LLMBard()
        try:
            llm.set_api_key(api_key)
        except:
            pass
        return llm

    def _init_library_controller(self, debug):
        self.controller = LibraryController(console=self, debug=debug)
        self.controller.install_llm(self.llm)
    
    def _init_logger(self, cus_format='%(asctime)s %(message)s', debug=False, propagate=False):
        log_id = os.path.join(self._output, "log")
        handler = logging.StreamHandler(sys.stdout)
        format = logging.Formatter(cus_format)
        handler.setFormatter(format)
        logger = logging.getLogger(log_id)
        for each_handler in logger.handlers:
            logger.removeHandler(each_handler)
        logger.addHandler(handler)
        logger.setLevel(logging.INFO)
        logger.propagate = propagate
        if debug:
            logger.setLevel(logging.DEBUG)
        return logger
    
