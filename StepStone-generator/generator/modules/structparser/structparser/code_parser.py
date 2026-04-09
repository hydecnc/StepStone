import re
import os
import json
import queue

from .basic_type import *

class CodeParser(StrCrtl):
    def __init__(self, inline_mode = False) -> None:
        super().__init__()
        self.text = []
        self._base_index = 0
        self._text_index = 0
        self._file_path = None
        self._ready = False
        self._inline_mode = inline_mode
        self.object = {'base': {}, 'type': {}, 'base_index': {}}
        self._regex_new_type = r'([a-z0-9A-Z_\(\), ]+)'
        self._regex_oneline_typedef = r'^typedef ([a-z0-9A-Z_\(\), ]+ (\*)?(( )+)?)' + self._regex_new_type + r';'
        self._regex_struct_typedef = r'^typedef (struct|enum|union) ([a-z0-9A-Z _]+)?( {)?'
        self._regex_struct = r'(struct|enum|union) ([a-z0-9A-Z _]+)?( {)?'
        self._regex_struct_new_type = r'^} ' + self._regex_new_type + r';'
        self._regex_define = r'^#define ([a-z0-9A-Z_\(\)-]+) (.*)'
    
    def init_db(self, db_path):
        self.object = json.load(open(db_path, 'r'))
        self._ready = True
        
    def list_objects(self, type_name, ignore):
        ret = set()
        if not self._ready:
            print("Use --select-db to specify a database")
            return ''
        if type_name not in self.object['type']:
            return ''
        if ignore != None:
            seen_type = set(ignore.split(','))
        else:
            seen_type = set()
        q = queue.Queue()
        q.put(type_name)
        while not q.empty():
            type_name = q.get(block=False)
            seen_type.add(type_name)
            ret.add(type_name)
            for each in self._get_nested_type(type_name):
                if each not in seen_type:
                    q.put(each)
        return list(ret)
        
    def find(self, type_name, ignore, iterate=True):
        text = []
        if not self._ready:
            print("Use --select-db to specify a database")
            return ''
        if type_name not in self.object['type']:
            return ''
        if ignore != None:
            seen_type = set(ignore.split(','))
        else:
            seen_type = set()
        q = queue.Queue()
        q.put(type_name)
        while not q.empty():
            type_name = q.get(block=False)
            seen_type.add(type_name)
            res = self._print_type_info(type_name)
            if res != '':
                text.append(res)
            if iterate:
                for each in self._get_nested_type(type_name):
                    if each not in seen_type:
                        q.put(each)
        return "\n".join(text)
    
    def _print_type_info(self, type_name):
        if type_name in self.object['type']:
            index = self.object['type'][type_name]
            type_data = self.object['base'][str(index)]
            if not self._inline_mode:
                print(type_data['type_content'])
            return type_data['type_content']
        return ''
    
    def _get_nested_type(self, type_name):
        res = []
        if type_name in self.object['type']:
            index = self.object['type'][type_name]
            type_data = self.object['base'][str(index)]
            type_name = type_data['type_name']
            res.append(type_name)
            res.extend(type_data['refer_type'])
        return res
    def parse_define(self, index):
        line = self.text[index]
        line = self._remove_noisy_ending(line)
        
        base_type = self.regex_get(self._regex_define, line, 0)
        value = self.regex_get(self._regex_define, line, 1)
        if line[-1] == '\\':
            new_index = index
            while True:
                new_index += 1
                line += self.text[new_index]
                line = self._remove_noisy_ending(line)
                if line[-1] != '\\':
                    break
            if base_type == None or value == None:
                return new_index + 1
            obj = Oneline()
            obj.type_cast = ""
            obj.type_from = base_type
            obj.raw_data = value + '\n' + self.text[index+1: new_index+1]
            self.add_object(base_type, obj, ignore_type_cast=True)
            return new_index + 1
        else:
            obj = Oneline()
            obj.type_cast = value
            obj.type_from = base_type
            obj.raw_data = line
            self.add_object(base_type, obj, ignore_type_cast=True)
            return index + 1
        
    def parse_typedef(self, index):
        line = self.text[index]
        line = self._remove_noisy_ending(line)
        # ending with ';'
        if self.regex_match(self._regex_oneline_typedef, line):
            m = self.regex_getall(self._regex_oneline_typedef, line)
            try:
                base_type = m[0][0]
                new_type = m[0][4]
                obj = Oneline()
                obj.type_cast = new_type
                obj.type_from = base_type
                obj.raw_data = line
            except:
                return -1
            self.add_object(base_type, obj)
            return index + 1
        # ending with '}'
        if self.regex_match(self._regex_struct_typedef, line):
            base_type = self.regex_get(self._regex_struct_typedef, line, 1)
            if base_type == None:
                base_type = self.name_empty_base_type(line)
            [struct_obj, new_index] = self.extract_struct(index)
            self.add_object(base_type, struct_obj)
            return new_index
        self.report_error_index("new typedef")
        return index + 1
            
    def extract_struct(self, index):
        line = self.text[index]
        key = self.regex_get(self._regex_struct, line, 0)
        skip_comment = False
        if key == 'struct':
            obj = Struct()
        if key == 'enum':
            obj = Enum()
        if key == 'union':
            obj = Union()
        for i in range(index, len(self.text)):
            line = self.text[i]
            no_space_line = self._remove_noisy_begining(line)
            if '*/' in line and skip_comment:
                skip_comment = False
                continue
            if skip_comment:
                continue
            if line.startswith('}'):
                new_type = self.regex_get(self._regex_struct_new_type, line, 0)
                data = "".join(self.text[index: i+1])
                obj.type_cast = new_type
                obj.raw_data = data
                return [obj, i+1]
            if not no_space_line.startswith('/*') or no_space_line.startswith('//'):
                obj.parse_field(line)
            if '/*' in line and '*/' not in line:
                skip_comment = True
        return [None, -1]
    
    def name_empty_base_type(self, line):
        key = self.regex_get(self._regex_struct_typedef, line, 0)
        return "{}_{}".format(key, self._text_index+1)
    
    def add_object(self, base_type, struct_obj: Struct, ignore_type_cast = False):
        new_type = struct_obj.type_cast
        text = struct_obj.raw_data
        if new_type == None:
            return
        base_type = self.clean_str(base_type)
        new_type = self.clean_str(new_type)
        if ignore_type_cast:
            type_cast = base_type
        else:
            type_cast = new_type
        if type_cast not in self.object['type']:
            self.object['type'][type_cast] = self._base_index
            self._base_index += 1
        else:
            self.report_error_index("duplicate type {}".format(type_cast))
        
        base_index = self.object['type'][type_cast]
        if base_index not in self.object['base']:
            self.object['base'][base_index] = {'type_name': base_type, 'type_content': text, 'refer_type': []}
            if struct_obj.type != 'enum':
                field_type = set()
                for field in struct_obj.fields:
                    field_type.add(field['field_type'])
                self.object['base'][base_index]['refer_type'] = list(field_type)
        
    def build_db(self, file_path, db_path):
        fp = open(file_path, 'r')
        self._file_path = file_path
        self.text = fp.readlines()
        self._text_index = 0
        while self._text_index < len(self.text):
            line = self.text[self._text_index]
            if line.startswith('typedef'):
                new_index = self.parse_typedef(self._text_index)
                if new_index != -1:
                    self._text_index = new_index
                    continue
                else:
                    self.report_error_index("unhandled type")
            if line.startswith('#define'):
                new_index = self.parse_define(self._text_index)
                if new_index != -1:
                    self._text_index = new_index
                    continue
                else:
                    self.report_error_index("unhandled define")
            self._text_index += 1
        self.dump_db(db_path)
        
    def dump_db(self, db_path):
        if os.path.exists(db_path):
            print("\"{}\" already exist, fail to create database".format(db_path))
            return
        fp = open(db_path, 'w')
        json.dump(self.object, fp)
                
    def report_error_index(self, msg):
        print("Error ({}): fail to parse object at {}:{}".format(msg, self._file_path, self._text_index+1))
                
    def regex_get(self, regex, line, index):
        m = re.search(regex, line)
        if m != None and len(m.groups()) > index:
            return m.groups()[index]
        return None
    
    def regex_getall(self, regex, line):
        m = re.findall(regex, line, re.MULTILINE)
        return m
    
    def regex_match(self, regex, line):
        m = re.search(regex, line)
        if m != None and len(m.group()) != 0:
            return True
        return False