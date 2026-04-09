import traceback

from .error import *

def report_type_init_error(func):
    def inner(self, obj: dict, *args, **kwargs):
        try:
            func(self, obj, *args, **kwargs)
        except SyzlangTypeInitError as e:
            raise e
        except SyzlangTypeStrError as e:
            raise e
        except IncompatiableBuilderType as e:
            traceback.print_exc()
            raise e
        except Exception:
            traceback.print_exc()
            raise SyzlangTypeInitError(obj, self.type)
    return inner 

def report_type_str_error(func):
    def inner(self):
        try:
            return func(self)
        except SyzlangTypeInitError as e:
            raise SyzlangTypeStrError(self._obj, self.type, e)
        except SyzlangTypeStrError as e:
            raise e
        except Exception as e:
            traceback.print_exc()
            raise SyzlangTypeStrError(self._obj, self.type, e)
    return inner

class Basic():
    def __init__(self, obj, show_size) -> None:
        self._obj = obj
        self._show_size = show_size
        self.type = None
    
    def validate(self):
        pass
    
# {"type_name": "const", "const_value": "", "const_size": ""}
class ConstBuilder(Basic):
    
    @report_type_init_error
    def __init__(self, obj: dict, show_size: bool=False) -> None:
        super().__init__(obj, show_size)
        self.type = 'const'
        self.value = self._obj['const_value']
        self.size = self._obj['const_size']
        
    def __repr__(self) -> str:
        return self.__str__()
    
    @report_type_str_error
    def __str__(self) -> str:
        res = self.type
        if self.value == "":
            raise ConstValueNotSpecified(self._obj)
        if not self._show_size:
            res += '[{}]'.format(self.value)
        elif self.size != "":
            res += '[{}, {}]'.format(self.value, self.size)
        else:
            raise SizeIsEmpty(self._obj)
        return res
    
    def validate(self):
        if self.size not in ['int8', 'int16', 'int32', 'int64']:
            return "Const size should be one of ['int8', 'int16', 'int32', 'int64'] but received {}".format(self.size)
        return None
        
# {"type_name": "int", "int_bound": {"min_val": "", "max_val": ""}, "int_size": ""}   
class IntBuilder(Basic):
    @report_type_init_error
    def __init__(self, obj: dict, show_size: bool=False) -> None:
        super().__init__(obj, show_size)
        self.type = 'int'
        self.size = self._obj['int_size']
        self.min_val = self._obj['int_bound']["min_val"]
        self.max_val = self._obj['int_bound']["max_val"]
    
    def __repr__(self) -> str:
        return self.__str__()
    
    @report_type_str_error
    def __str__(self) -> str:
        res = self.size
        if self.min_val != "" and self.max_val != "":
            if self.min_val == self.max_val:
                res += "[{}]".format(self.min_val)
            else:
                res += "[{}:{}]".format(self.min_val, self.max_val)
        return res
    
    def validate(self):
        if self.size not in ['int8', 'int16', 'int32', 'int64', 'intptr']:
            return "Int size should be one of ['int8', 'int16', 'int32', 'int64', 'intptr'] but received {}".format(self.size)
        if not self._show_size and self.size == 'int64':
            return "The maximum size for int as argument type is int32"
        return None

# {"type_name": "flags", "flags_enum": "", "flags_element_size": ""}
class FlagsBuilder(Basic):
    @report_type_init_error
    def __init__(self, obj: dict, show_size: bool=False) -> None:
        super().__init__(obj, show_size)
        self.type = 'flags'
        self.enum = obj['flags_enum']
        self.ele_size = obj['flags_element_size']
        
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_type_str_error
    def __str__(self) -> str:
        res = self.type
        if not self._show_size:
            res += '[{}]'.format(self.enum)
        elif self.ele_size != "":
            res += '[{}, {}]'.format(self.enum, self.ele_size)
        else:
            raise SizeIsEmpty(self._obj)
        return res
  
# {"type_name": "array", "array_element": "", "array_length": ""}  
class ArrayBuilder(Basic):
    @report_type_init_error
    def __init__(self, obj, show_size) -> None:
        super().__init__(obj, show_size)
        self.type = 'array'
        self.array_ele = obj['array_element']
        self.array_len = obj['array_length']
        
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_type_str_error
    def __str__(self) -> str:
        res = self.type
        try:
            self.array_len = int(self.array_len)
        except:
            self.array_len = ""
        if self.array_len != "":
            res += "[{}, {}]".format(TypeBuilder(self.array_ele, self._show_size), self.array_len)
        else:
            res += "[{}]".format(TypeBuilder(self.array_ele, True))
        return res
    
# {"type_name": "ptr", "ptr_method": "", "ptr_object": ""}
class PtrBuilder(Basic):
    @report_type_init_error
    def __init__(self, obj, show_size) -> None:
        super().__init__(obj, show_size)
        self.type = 'ptr'
        self.method = obj['ptr_method']
        self.object = obj['ptr_object']
        
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_type_str_error
    def __str__(self) -> str:
        res = self.type
        if self.method == 'in':
            res += '[{}, {}]'.format(self.method, TypeBuilder(self.object, True))
        else:
            if self.object['type_name'] in ['resource', 'ptr', 'int', 'array']:
                res += '[{}, {}]'.format(self.method, TypeBuilder(self.object, True))
            elif self.object['type_name'] in ['flags']:
                res += '[{}, {}]'.format(self.method, FlagsBuilder(self.object, True).ele_size)
            elif self.object['type_name'] in ['string']:
                res += '[{}, {}]'.format(self.method, 'intptr')
            else:
                res += '[{}, {}]'.format(self.method, 'int64')
        return res
    
# {"type_name": "string", "string_value": ""}
class StringBuilder(Basic):
    @report_type_init_error
    def __init__(self, obj, show_size) -> None:
        super().__init__(obj, show_size)
        self.type = 'string'
        self.value = obj['string_value']
    
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_type_str_error
    def __str__(self) -> str:
        res = self.type
        if self.value != "":
            res += '[{}]'.format(self.value)
        return res
    
# {"type_name": "len", "len_target": "", "len_size": ""}
class LenBuilder(Basic):
    @report_type_init_error
    def __init__(self, obj, show_size) -> None:
        super().__init__(obj, show_size)
        self.type = 'len'
        self.target = obj['len_target']
        self.size = obj['len_size']
        
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_type_str_error
    def __str__(self) -> str:
        res = self.type
        if self.target == "":
            raise LenTargetIsEmpty(self._obj)
        if self._show_size:
            res += '[{}, {}]'.format(self.target, self.size)
        else:
            res += '[{}]'.format(self.target)
        return res
    
# {"type_name": "bytesize", "bytesize_target": "", "bytesize_size": ""}
class ByteSizeBuilder(Basic):
    @report_type_init_error
    def __init__(self, obj, show_size) -> None:
        super().__init__(obj, show_size)
        self.type = 'bytesize'
        self.target = obj['bytesize_target']
        self.size = obj['bytesize_size']
        
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_type_str_error
    def __str__(self) -> str:
        res = self.type
        if self.target == "":
            raise LenTargetIsEmpty(self._obj)
        if self._show_size:
            res += '[{}, {}]'.format(self.target, self.size)
        else:
            res += '[{}]'.format(self.target)
        return res

# {"type_name": "bitsize", "bitsize_target": "", "bitsize_size": ""}
class BitSizeBuilder(Basic):
    @report_type_init_error
    def __init__(self, obj, show_size) -> None:
        super().__init__(obj, show_size)
        self.type = 'bitsize'
        self.target = obj['bitsize_target']
        self.size = obj['bitsize_size']
        
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_type_str_error
    def __str__(self) -> str:
        res = self.type
        if self.target == "":
            raise LenTargetIsEmpty(self._obj)
        if self._show_size:
            res += '[{}, {}]'.format(self.target, self.size)
        else:
            res += '[{}]'.format(self.target)
        return res
    
# {"type_name": "void"}
class VoidBuilder(Basic):
    @report_type_init_error
    def __init__(self, obj, show_size) -> None:
        super().__init__(obj, show_size)
        self.type = 'void'
        
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_type_str_error
    def __str__(self) -> str:
        return self.type
    
# {"type_name": "structure", "structure_name": ""}
class StructBuilder(Basic):
    @report_type_init_error
    def __init__(self, obj, show_size) -> None:
        super().__init__(obj, show_size)
        self.type = 'struct'
        self.struct_name = obj['structure_name']
        self.args = obj['args']
        
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_type_str_error
    def __str__(self) -> str:
        res = self.struct_name
        # show_size indicates it's a field in struct
        args = self.args
        if len(self.args) > 0 and self._show_size:
            if type(self.args[0]) == dict:
                args = []
                for each in self.args:
                    args.append(str(ArgBuilder(each, self._show_size)))
            res += '[{}]'.format(', '.join(args))
        return res
    
# {"type_name": "resource", "resource_name": ""}
class ResourceBuilder(Basic):
    @report_type_init_error
    def __init__(self, obj, show_size) -> None:
        super().__init__(obj, show_size)
        self.type = 'resource'
        self.resource_name = obj['resource_name']
    
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_type_str_error
    def __str__(self) -> str:
        return self.resource_name
    
# {"type_name": "arg", "arg_name": ""}
class ArgBuilder(Basic):
    @report_type_init_error
    def __init__(self, obj, show_size) -> None:
        super().__init__(obj, show_size)
        self.type = 'arg'
        self.arg_name: str = obj['arg_name']
    
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_type_str_error
    def __str__(self) -> str:
        return self.arg_name.upper()
    
class TypeBuilder():
    @report_type_init_error
    def __init__(self, obj, show_size) -> None:
        if 'type_name' not in obj:
            raise IncompatiableBuilderType(obj)
        self._builder = None
        if obj['type_name'] == 'const':
            self._builder = ConstBuilder(obj, show_size)
        if obj['type_name'] == 'int':
            self._builder = IntBuilder(obj, show_size)
        if obj['type_name'] == 'flags':
            self._builder = FlagsBuilder(obj, show_size)
        if obj['type_name'] == 'array':
            self._builder = ArrayBuilder(obj, show_size)
        if obj['type_name'] == 'ptr':
            self._builder = PtrBuilder(obj, show_size)
        if obj['type_name'] == 'string':
            self._builder = StringBuilder(obj, show_size)
        if obj['type_name'] == 'len':
            self._builder = LenBuilder(obj, show_size)
        if obj['type_name'] == 'bytesize':
            self._builder = ByteSizeBuilder(obj, show_size)
        if obj['type_name'] == 'bitsize':
            self._builder = BitSizeBuilder(obj, show_size)
        if obj['type_name'] == 'void':
            self._builder = VoidBuilder(obj, show_size)
        if obj['type_name'] == 'structure':
            self._builder = StructBuilder(obj, show_size)
        if obj['type_name'] == 'resource':
            self._builder = ResourceBuilder(obj, show_size)
        if obj['type_name'] == 'arg':
            self._builder = ArgBuilder(obj, show_size)
        if self._builder == None:
            raise IncompatiableBuilderType(obj['type_name'])
    
    def __getattr__(self, attr):
        return getattr(self._builder, attr)
    
    def __repr__(self) -> str:
        return self._builder.__repr__()

    @report_type_str_error
    def __str__(self) -> str:
        return self._builder.__str__()