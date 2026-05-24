import requests
import json
import os
import copy
import logging, sys
import multiprocessing
import generator.resources.prompt_flag_extract as prompt_flag_extract
import generator.resources.prompt_dependency_extract as prompt_dependency_extract


from generator.llm.error import ErrorInResponse
from generator.controller import Controller
from .error import *
from generator.llm.chatgpt import ChatGPT
from generator.llm import LLM
# from generator.resources.prompt_direct import *
from generator.resources.prompt_main import *
from generator.modules.structparser.structparser import CodeParser
from generator.syzlang import SyzlangHelper

FILE_HANDLER = 0
STREAM_HANDLER = 1

class LibraryController(Controller):
    def __init__(self, console, debug) -> None:
        super().__init__(console)
        self.llm: ChatGPT = None
        self.logger = self._init_logger(__name__, debug=debug, handler_type=STREAM_HANDLER)
        manager = multiprocessing.Manager()
        self.cmd_queue = manager.Queue()
        self.syzlang_helper = SyzlangHelper(self.console.cfg.find_library_header())
        self.debug = debug
        self.init_shown_objs()
        self.init_llm_hint()
        
    def init_shown_objs(self):
        self._shown_objs = set()
        
    def init_llm_hint(self):
        self._llm_hint = []
    
    def install_llm(self, llm_inst):
        self.llm = llm_inst
        
    def prepare_assistant_session(self, asst_name):
        session: LLM = self.llm.start_new_session(type="assistant", engine='gpt-4-turbo', console=self.console, logger=self.logger)
        selected_assistant = self.console.cfg.find_assistant(asst_name)
        if selected_assistant == None:
            raise ChatGPTAssistantNotSelected()
        session.retrieve_assistant(selected_assistant)
        return session
    
    def parsed_api_name(self, api: list):
        hash_table = set()
        for each in api:
            func_name = each['api_name']
            hash_table.add(func_name)
        return hash_table
        
    def analyze(self, api_text, chat_type, join_syzlang_json, refer_syzlang_context):
        flag_extract = self.prepare_assistant_session("flag_extract")
        self.learn_examples_flag(flag_extract)
        depend_extract = self.prepare_assistant_session("dependency_extract")
        self.learn_examples_dependency(depend_extract)
        
        session: LLM = self.llm.start_new_session(type=chat_type, console=self.console, logger=self.logger)
        session.append_assistant_message(prompt_instructions)
        self.learn_examples(session)
        
        unknow_func_index = 0
        if join_syzlang_json == None:
            if refer_syzlang_context == None:
                syzlang_object = {"resource": [], "enum": [], "structure": [], "api": []}
            else:
                try:
                    syzlang_object = json.load(open(refer_syzlang_context, 'r'))
                except:
                    print("Fail to parse syzlang json file: {}".format(refer_syzlang_context))
                    syzlang_object = {"resource": [], "enum": [], "structure": [], "api": []}
                parsed_api_hash_table = self.parsed_api_name(syzlang_object['api'])
        else:
            try:
                syzlang_object = json.load(open(join_syzlang_json, 'r'))
            except:
                print("Fail to parse syzlang json file: {}".format(join_syzlang_json))
                syzlang_object = {"resource": [], "enum": [], "structure": [], "api": []}
            parsed_api_hash_table = self.parsed_api_name(syzlang_object['api'])
        for api in api_text:
            fault_n = 0
            skipped_api = []
            func_name = self.regex_get(r'^[a-z0-9A-Z_]+ (\*)?([a-z0-9A-Z_]+)', api, 1)
            if func_name == None:
                func_name = "UNKNOWN_FUNC {}".format(unknow_func_index) 
                unknow_func_index += 1
            if func_name in parsed_api_hash_table:
                continue
            while fault_n < 3:
                try: 
                    self.update_session_logger(func_name, session, flag_extract, depend_extract)
                    self.init_llm_hint()
                    self.init_shown_objs()
                    plain_context = self.find_struct_context(api)
                    extra_enum_types = self.find_extra_flags(api, flag_extract)
                    if plain_context != "":
                        extra_enum_types.extend(self.find_extra_flags(plain_context, flag_extract))
                    context = plain_context + '\n' + self.find_struct_context(api, extra_enum_types)
                    message = prompt_input_prompt.format(api, context)
                    self.provide_resource_hint(syzlang_object)
                    message = self.merge_hint_to_message(message)
                    response = session.chat(message=message)
                    self.console.chatgpt_response_print(response)
                    done_depend = False
                    break
                except Exception as e:
                    fault_n += 1
                    self.logger.warning("Retry {} due to error: {}".format(fault_n, e))
                    continue
            
            if fault_n >= 3:
                self.logger.error("Failed after 3 retries, skip {}".format(api))
                skipped_api.append(api)
            
            fault_n = 0
            while fault_n < 3:
                try:
                    try:
                        response = self._remove_code_marker(response)
                        new_object = json.loads(response)
                    except Exception as e:
                        fault_n += 1
                        response = session.chat(prompt_fix_json_error.format(e))
                        self.console.chatgpt_response_print(response)
                        continue
                    err = self.syzlang_helper.validate_syzlang_json(new_object)
                    if err != '':
                        fault_n += 1
                        response = session.chat(prompt_fix_type_error.format(err))
                        self.console.chatgpt_response_print(response)
                        continue
                    if not done_depend:
                        depend_stat = self.find_dependency(api, depend_extract, new_object)
                        if depend_stat != '':
                            response = session.chat(prompt_dependency_extract.prompt_input_revise.format(depend_stat))
                            self.console.chatgpt_response_print(response)
                            done_depend = True
                            continue
                    leftover = self.dangling_structs(new_object)
                    if leftover != []:
                        response = session.chat(prompt_fix_missing_struct.format("\n".join(context)))
                        self.console.chatgpt_response_print(response)
                        continue
                    session.pop_message(len(session.messages) - example_num * 2 - 2)
                    break
                except Exception as e:
                    self.console.save_file("\n".join(skipped_api), "skipped_api")
                    self.console.dump_json(syzlang_object, "syzlang_crash.json")
                    self.logger.warning("Error: {}".format(e))
                    session.pop_message(len(session.messages) - example_num * 2 - 2)
                    break
            if fault_n < 3:
                syzlang_object = self.syzlang_helper.merge_object(syzlang_object, new_object)
        congregate_object = copy.deepcopy(syzlang_object)
        if refer_syzlang_context != None:
            self.console.dump_json(syzlang_object, "syzlang_congregate.json")
            self.remove_referee_context(syzlang_object, refer_syzlang_context)
        self.console.dump_json(syzlang_object, "syzlang.json")
        text = self.syzlang_helper.generate_description(syzlang_object, congregate_object)
        self.console.save_file(text, "cuda.txt")
        text = self.syzlang_helper.generate_const(syzlang_object)
        self.console.save_file(text, "cuda.txt.const")
        
    def dangling_structs(self, object):
        structs = []
        context = []
        structbulk = self.syzlang_helper.get_structbulk(object)
        for obj in self._shown_objs:
            if structbulk.find_struct(obj) == None:
                structs.append(obj)
        
        for obj in structs:
            res = self.find_type_def(obj, ignore="VkStructureType,VkFormat")
            if res != '':
                context.append(res)
        return context
    
    def remove_referee_context(self, syzlang_object, json_path):
        try:
            object = json.load(open(json_path, 'r'))
        except:
            object = {"resource": [], "enum": [], "structure": [], "api": []}
        
        keys = ['resource', 'enum', 'structure', 'api']
        for key in keys:
            for each in object[key]:
                if each in syzlang_object[key]:
                    syzlang_object[key].remove(each)
        
    def provide_resource_hint(self, object):
        for resource in object['resource']:
            name = resource['resource_name']
            self._llm_hint.append('- Type `{}` is a resource'.format(name))
        return 
    
    def merge_hint_to_message(self, message):
        self._llm_hint.append(message)
        return "\n".join(self._llm_hint)
    
    def find_extra_flags(self, api, session: LLM) -> list:
        res = []
        message = prompt_flag_extract.prompt_input_prompt.format(api)
        err_n = 0
        response = session.chat(message=message)
        while err_n <= 3:
            try:
                response = self._remove_code_marker(response)
                new_object = json.loads(response)
                break
            except ErrorInResponse as e:
                err_n += 1
                if err_n == 3:
                    raise e
                continue
            except json.JSONDecodeError as e:
                err_n += 1
                response = session.chat(prompt_fix_json_error.format(e))
                self.console.chatgpt_response_print(response)
                continue
        session.pop_message(len(session.messages) - prompt_flag_extract.example_num * 2 - 1)
        
        for old_flag in new_object:
            new_flags = new_object[old_flag]
            if type(new_flags) is not dict:
                raise LLMRandomError("flag should be a dict but got {}: {}".format(type(new_flags), new_flags))
            res.extend(new_flags['type'])
            self._llm_hint.append('- {}'.format(new_flags['explain']))
        return res
    
    def find_dependency(self, api, session: LLM, new_object: dict):
        header = []
        for each_res in new_object['resource']:
            if self._is_common_resource(each_res['resource_name']):
                header.append('- skip type {}'.format(each_res['resource_name']))
        message = prompt_dependency_extract.prompt_input_prompt.format(api)
        message += '\n' + "\n".join(header)
        err_n = 0
        while err_n <= 3:
            try:
                response = session.chat(message=message)
                break
            except ErrorInResponse as e:
                err_n += 1
                continue
        if err_n > 3:
            raise e
        session.pop_message(len(session.messages) - prompt_dependency_extract.example_num * 2 - 1)
        if response == "NO DEPENDENCY":
            return ''
        return response
        
    def find_struct_context(self, api, extra_enum_types=[]):
        regex_func = r'\w+ (\*)?\w+( )?\((.*)\)'
        args = self.regex_get(regex_func, api, 2)
        if args == None:
            raise InvalidAPIFormat(api)
        arg_list = args.split(',')
        arg_type_set = set()
        for each in extra_enum_types:
            if each == '':
                continue
            arg_type_set.add(each)
        text = []
        for arg in arg_list:
            arg = self.clean_str(arg)
            t = arg.split(' ')
            arg_type = ' '.join(t[:-1])
            arg_name = t[-1]
            arg_type = self.clean_str(arg_type)
            if arg_type.startswith('const '):
                arg_type = arg_type[6:]
            if arg_type.endswith('*'):
                arg_type = self._remove_pointer(arg_type)
                arg_type = self.clean_str(arg_type)
            arg_type_set.add(arg_type)
        for arg_type in arg_type_set:
            res = self.find_type_def(arg_type, ignore="VkStructureType,VkFormat")
            if res != '':
                text.append(res)
            extra_def = self.find_extra_bitmask(res)
            if extra_def != None:
               text.append(extra_def)
        return "\n".join(text)

    def find_extra_bitmask(self, text):
        extra_def = []
        for line in text.split('\n'):
            if self.regex_match(r'typedef VkFlags ([a-zA-Z0-9_]+);', line):
                vk_type = self.regex_get(r'typedef VkFlags ([a-zA-Z0-9_]+);', line, 0)
                vk_bit_type = self.find_vk_bitmap_name(vk_type)
                if vk_bit_type != None:
                    self._llm_hint.append('- Type `{}` is a bitmask, replacing it with the bitmap enum `{}` in every occurrence'.format(vk_type, vk_bit_type))
                    self._shown_objs.remove(vk_type)
                    self._shown_objs.add(vk_bit_type)
                res = self.find_type_def(vk_bit_type, ignore="VkStructureType,VkFormat")
                if res != '':
                    extra_def.append(res)
        if len(extra_def) == 0:
            return None
        return "\n".join(extra_def)
    
    def find_vk_bitmap_name(self, vk_type):
        if self.regex_match(r'^(Vk[a-zA-Z0-9_]+Flag)s([a-zA-Z0-9_]+)?$', vk_type):
            p1 = self.regex_get(r'^(Vk[a-zA-Z0-9_]+Flag)s([a-zA-Z0-9_]+)?$', vk_type, 0)
            p2 = self.regex_get(r'^(Vk[a-zA-Z0-9_]+Flag)s([a-zA-Z0-9_]+)?$', vk_type, 1)
            if p2 == None:
                return p1 + 'Bits'
            return p1 + 'Bits' + p2
        return None
    
    def find_type_def(self, arg_type, ignore=None):
        ret = []
        codep = CodeParser(inline_mode=True)
        db_name = self.console.cfg.find_library_db()
        db_path = os.path.join(os.getcwd(), 'generator/resources', db_name)
        codep.init_db(db_path)
        for obj in codep.list_objects(arg_type, ignore=ignore):
            if obj not in self._shown_objs:
                self._shown_objs.add(obj)
                ret.append(codep.find(obj, ignore=ignore, iterate=False))
        return "\n".join(ret)
        
    def learn_examples_flag(self, session: ChatGPT):
        session.append_user_message(prompt_flag_extract.prompt_example_header)
        session.append_user_message(prompt_flag_extract.prompt_example1)
        session.append_assistant_message(prompt_flag_extract.prompt_example1_response)
        session.append_user_message(prompt_flag_extract.prompt_example2)
        session.append_assistant_message(prompt_flag_extract.prompt_example2_response)
        session.append_user_message(prompt_flag_extract.prompt_example3)
        session.append_assistant_message(prompt_flag_extract.prompt_example3_response)
        session.append_user_message(prompt_flag_extract.prompt_example4)
        session.append_assistant_message(prompt_flag_extract.prompt_example4_response)
        session.append_user_message(prompt_flag_extract.prompt_example5)
        session.append_assistant_message(prompt_flag_extract.prompt_example5_response)
        session.append_user_message(prompt_flag_extract.prompt_example6)
        session.append_assistant_message(prompt_flag_extract.prompt_example6_response)
        
    def learn_examples_dependency(self, session: ChatGPT):
        session.append_user_message(prompt_dependency_extract.prompt_example_header)
        session.append_user_message(prompt_dependency_extract.prompt_example1)
        session.append_assistant_message(prompt_dependency_extract.prompt_example1_response)
        session.append_user_message(prompt_dependency_extract.prompt_example2)
        session.append_assistant_message(prompt_dependency_extract.prompt_example2_response)
        
    def learn_examples(self, session: ChatGPT):
        session.append_user_message(prompt_example_header)
        session.append_user_message(prompt_example1)
        session.append_assistant_message(prompt_example1_response)
        session.append_user_message(prompt_example2)
        session.append_assistant_message(prompt_example2_response)
        session.append_user_message(prompt_example3)
        session.append_assistant_message(prompt_example3_response)
    
    def insert_annotation(self, text):
        res = []
        code_begin = 0
        record_flag = 0
        for line in text.split('\n'):
            line: str = line.strip('\n')
            if line.startswith('```'):
                code_begin = 1
                if record_flag:
                    break
            if code_begin and line == "include <cuda.h>":
                record_flag = 1
            if record_flag:
                res.append(line)
                if line in ["# Resource Definition", "# Enum Definition", "# API Definition", "# Structure Definition"]:
                    res.append("[APPEND_NEW_CONTENT_HERE]")
        return "\n".join(res)

    def update_session_logger(self, event_name, *args) -> None:
        logger = self.create_event_logger(event_name, debug=self.debug)
        for session in args:
            if not isinstance(session, LLM):
                continue
            session.set_logger(logger)
    
    def _format_error_message(self, err):
        ret = ""
        for e in err:
            ret += '- ' + e + '\n'
        return ret
    
    def create_event_logger(self, event_name, cus_format='%(asctime)s %(message)s', debug=False):
        logger_id = os.path.join(self.console._output, event_name)
        logger = self.logger.getChild(logger_id)
        handler = logging.FileHandler(logger_id)
        h_format = logging.Formatter(cus_format)
        handler.setFormatter(h_format)
        logger.addHandler(handler)
        logger.setLevel(self.logger.level)
        if debug:
            logger.setLevel(logging.DEBUG)
        logger.propagate = True
        return logger

    def _is_common_resource(self, res_name: str) -> bool:
        return res_name.startswith('CU') or res_name.startswith('cl_') or res_name.startswith('Vk')
            
    def _init_logger(self, logger_id, cus_format='%(asctime)s %(message)s', debug=False, propagate=False, handler_type = FILE_HANDLER) -> logging.Logger:
        if (handler_type == FILE_HANDLER):
            handler = logging.FileHandler(logger_id)
            format = logging.Formatter(cus_format)
            handler.setFormatter(format)
        if (handler_type == STREAM_HANDLER):
            handler = logging.StreamHandler(sys.stdout)
            format = logging.Formatter(cus_format)
            handler.setFormatter(format)
        logger = logging.getLogger(logger_id)
        for each_handler in logger.handlers:
            logger.removeHandler(each_handler)
        logger.addHandler(handler)
        logger.setLevel(logging.INFO)
        logger.propagate = propagate
        if debug:
            logger.setLevel(logging.DEBUG)
        return logger
        
    def _log_subprocess_output(self, pipe):
        try:
            for line in iter(pipe.readline, b''):
                try:
                    line = line.decode("utf-8").strip('\n').strip('\r')
                except:
                    continue
                self.console.print(line)
        except ValueError:
            pass
        except EOFError:
            pass
        return
