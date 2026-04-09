from .aux_helper import *
from .aux_type import *
from .aux_resource import *
from .aux_enum import *
from .aux_structure import *

class AuxAPI():
    TYPE='api'
    @report_init_error
    def __init__(self, data: dict, res_bulk: ResourceBulk, enum_bulk: EnumBulk, struct_bulk: StructureBulk) -> None:
        self.api_name = data['api_name']
        self.ret_type = data['ret_type']
        self.res_bulk = res_bulk
        self.enum_bulk = enum_bulk
        self.struct_bulk = struct_bulk
        self._initparams(data)
        
    def _initparams(self, data: dict) -> None:
        self.params: list[AuxParam] = []
        for param in data['params']:
            self.params.append(AuxParam(param))
        
    def __repr__(self) -> str:
        return self.__str__()
    
    @report_str_error
    def __str__(self) -> str:
        api_name = "syz_" + self.api_name
        
        params_list = []
        for param in self.params:
            cur_param = []
            referred_struct = self.get_param_structs(param)
            if referred_struct != "":
                aux_struct_obj = self.struct_bulk.find_struct(referred_struct)
                if aux_struct_obj != None:
                    args = aux_struct_obj.get_args()
                    if len(args) > 0:
                        arg_cand = self.find_arg_cand(aux_struct_obj)
                        arg_comb = self.combine_arg(arg_cand)
                        new_param_format = self._create_param_arg_format(str(param), referred_struct)
                        for each_comb in arg_comb:
                            cur_param.append("{} {}".format(param.param_name, new_param_format.format(", ".join(each_comb))))
                        params_list.append(cur_param)
                        continue
            cur_param.append("{} {}".format(param.param_name, param))
            params_list.append(cur_param)
        apiparams = self.combine_param(params_list)
        return self._generate_api(api_name, apiparams)
    
    def validate(self) -> list | None:
        ret = []
        try:
            self.__str__()
        except Exception as e:
            ret.append(str(e))
        if type(self.ret_type) != str:
            ret.append('API {} return type need to be a string, but received {}'.format(self.api_name, self.ret_type))
        for param in self.params:
            if param.param_name in ['const', 'int8', 'int16', 'int32', 'int64', 'intptr', 'array', 'ptr', 'string', 'stringnoz', 'glob', 'fmt', 'len', 'bytesize', 'bitsize', 'offsetof', 'vma', 'proc', 'compressed_image', 'text', 'void']:
                ret.append("API {} parameter name '{}' is illegit, parameter name cannot be one of ['const', 'int8', 'int16', 'int32', 'int64', 'intptr', 'array', 'ptr', 'string', 'stringnoz', 'glob', 'fmt', 'len', 'bytesize', 'bitsize', 'offsetof', 'vma', 'proc', 'compressed_image', 'text', 'void']".format(self.api_name, param.param_name))
        return ret
    
    def is_resource(self, obj_name):
        return self.res_bulk.find_resource(obj_name) != None
    
    def find_arg_cand(self, aux_struct: AuxStructure):
        arg_cand = {}
        top_lvl_args = {}
        aux_args = aux_struct.get_args()
        for i in range(len(aux_args)):
            arg = aux_args[i]
            nested_args = self._find_arg_cand(aux_struct, i)
            for each in nested_args:
                top_lvl_args[each] = arg
            arg_cand[arg] = []
            
        for aux_struct in self.struct_bulk.get_all_struct():
            for arg_of in aux_struct.arg_of:
                if arg_of in top_lvl_args:
                    arg_cand[top_lvl_args[arg_of]].append(aux_struct.struct_name)
        return arg_cand
    
    def combine_arg(self, arg_cand: dict):
        return self._combine_arg(0, arg_cand, [])
    
    def combine_param(self, params_list: list):
        return self._combine_param(0, params_list, [])
    
    def get_param_structs(self, t: TypeBuilder):
        if t.type == 'struct':
            return t.struct_name
        if t.type == 'array':
            return self.get_param_structs(TypeBuilder(t.array_ele, False))
        if t.type == 'ptr':
            if t.method == 'in':
                return self.get_param_structs(TypeBuilder(t.object, False))
            else:
                return 'int64'
        return ''
    
    def get_param_structs_and_index(self, t: TypeBuilder, match_arg: str):
        if t.type == 'struct':
            for i in range(len(t.args)):
                arg = t.args[i]
                if arg == match_arg:
                    return t.struct_name, i
            return None, -1
        if t.type == 'array':
            return self.get_param_structs_and_index(TypeBuilder(t.array_ele, False), match_arg)
        if t.type == 'ptr':
            return self.get_param_structs_and_index(TypeBuilder(t.object, False), match_arg)
        if t.type == 'arg':
            return None, 0
        return None, -1
    
    def _generate_api(self, api_name: str, apiparams: list) -> str:
        res = []
        if len(apiparams) == 1:
            ret = "{}({})".format(api_name, ", ".join(apiparams[0]))
            if self.is_resource(self.ret_type):
                ret += " " + self.ret_type
            return ret
        for i in range(len(apiparams)):
            param = apiparams[i]
            ret = "{}${}({})".format(api_name, i+1, ", ".join(param))
            if self.is_resource(self.ret_type):
                ret += " " + self.ret_type
            res.append(ret)
        return "\n".join(res)
    
    def _find_arg_cand(self, struct: AuxStructure, arg_index: int) -> list[str]:
        arg = struct.get_args()[arg_index]
        res = [arg]
        for field in struct.get_fields():
            sub_struct, index = self.get_param_structs_and_index(field, arg)
            if sub_struct != None and index >= 0:
                res.extend(self._find_arg_cand(self.struct_bulk.find_struct(sub_struct), index))
        return res
        
    def _combine_arg(self, key_index, arg_cand: dict, res: list) -> list:
        final_comb = []
        keys = list(arg_cand.keys())
        if key_index >= len(keys):
            return [res.copy()]
        for arg in arg_cand[keys[key_index]]:
            res.append(arg)
            ret = self._combine_arg(key_index+1, arg_cand, res)
            res.pop()
            final_comb.extend(ret)
        return final_comb
    
    def _combine_param(self, index, params: list, res: list) -> list:
        final_comb = []
        if index >= len(params):
            return [res.copy()]
        for param in params[index]:
            res.append(param)
            ret = self._combine_param(index+1, params, res)
            res.pop()
            final_comb.extend(ret)
        return final_comb

    def _create_param_arg_format(self, old_param_format: str, referred_struct: str) -> str:
        index = old_param_format.index(referred_struct)
        if old_param_format[index+len(referred_struct)] == '[':
            offset = index + old_param_format[index:].index(']') + 1
        else:
            offset = index+len(referred_struct)
        new_param_format = old_param_format[:index+len(referred_struct)] + '[{}]' + old_param_format[offset:]
        return new_param_format
    
class AuxParam(TypeBuilder):
    TYPE='param'
    @report_init_error
    def __init__(self, data: dict) -> None:
        self.param_name = data['param_name']
        self._obj = data['param_type']
        super().__init__(self._obj, show_size=False)
        
class APIBulk():
    TYPE='api'
    def __init__(self, api_json: list, res_bulk: ResourceBulk, enum_bulk: EnumBulk, struct_bulk: StructureBulk):
        self._aux_api = []
        self._api_map = {}
        for data in api_json:
            a = AuxAPI(data, res_bulk, enum_bulk, struct_bulk)
            self._aux_api.append(a)
            self._api_map[a.api_name] = a
            
    def find_api(self, api_name) -> AuxAPI | None:
        if api_name in self._api_map:
            return self._api_map[api_name]
        return None
    
    def get_all_api(self) -> list[AuxAPI]:
        return self._aux_api
    
    def has_api(self, api_name) -> bool:
        return api_name in self._api_map
    
    def validate(self):
        ret = []
        for each_api in self._aux_api:
            err = each_api.validate()
            if err != None:
                ret.extend(err)
        if len(ret) == 0:
            return None
        return ret
    
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_str_error
    def __str__(self) -> str:
        ret = []
        for each_api in self._aux_api:
            ret.append(str(each_api))
        return "\n".join(ret)