from .aux_helper import *
from .aux_type import *

class AuxField(TypeBuilder):
    TYPE='field'
    @report_init_error
    def __init__(self, data: dict) -> None:
        self.field_name = data['field_name']
        self._obj = data['field_type']
        super().__init__(self._obj, show_size=True)
        
class AuxStructure():
    TYPE='structure'
    @report_init_error
    def __init__(self, data: dict) -> None:
        self.struct_name = data['structure_name']
        self._init_fields(data)
        
    def _init_fields(self, data: dict) -> None:
        self._fields: list[AuxField] = []
        for field in data['fields']:
            self._fields.append(AuxField(field))
        self.type_cast = data['type_cast']
        self.arg_of = data['arg_of']
    
    def __repr__(self) -> str:
        return self.__str__()
    
    @report_str_error
    def __str__(self) -> str:
        res = []
        args = self.get_args()
        if len(args) > 0:
            header = "type {}[{}] ".format(self.struct_name, ", ".join(args))
        elif self.type_cast != None:
            header = "type {} ".format(self.struct_name)
        else:
            header = "{} ".format(self.struct_name)
            
        if self.type_cast != None:
            if type(self.type_cast) == str:
                header += self.type_cast
            elif type(self.type_cast) == dict:
                header += str(TypeBuilder(self.type_cast, False))
            return header
        else:
            header += '{'
        
        res.append(header)
        for field in self._fields:
            res.append("\t{}\t{}".format(field.field_name, field))
        res.append('}')
        return "\n".join(res)
    
    def get_args(self) -> list[str]:
        res = []
        for field in self._fields:
            if field.type == 'arg':
                # arg_name is LLM generated name
                # It may be lower case
                # use str() to get correct upper case arg name
                res.append(str(field))
        return res
    
    def get_fields(self) -> list[AuxField]:
        return self._fields
    
    def validate(self) -> list | None:
        ret = []
        if self.struct_name in ['const', 'int8', 'int16', 'int32', 'int64', 'intptr', 'array', 'ptr', 'string', 'stringnoz', 'glob', 'fmt', 'len', 'bytesize', 'bitsize', 'offsetof', 'vma', 'proc', 'compressed_image', 'text', 'void']:
            ret.append("Structure name '{}' is illegit, structure name cannot be one of ['const', 'int8', 'int16', 'int32', 'int64', 'intptr'', 'array', 'ptr', 'string', 'stringnoz', 'glob', 'fmt', 'len', 'bytesize', 'bitsize', 'offsetof', 'vma', 'proc', 'compressed_image', 'text', 'void']")
        for field in self._fields:
            err = field.validate()
            if err != None:
                ret.append("Inside struct {}, {} {}: {}".format(self.struct_name, field.type, field.field_name, err))
        if type(self.type_cast) == dict:
            err = TypeBuilder(self.type_cast, False).validate()
            ret.append("Structure {} has invalid typecast value, it need to be string but received {}".format(self.struct_name, self.type_cast))
        if len(ret) == 0:
            return ret
        return None
        
class StructureBulk():
    TYPE='structure'
    def __init__(self, struct_json):
        self._aux_struct = []
        self._struct_map = {}
        for data in struct_json:
            s = AuxStructure(data)
            self._aux_struct.append(s)
            self._struct_map[s.struct_name] = s
            
    def find_struct(self, struct_name) -> AuxStructure | None:
        if struct_name in self._struct_map:
            return self._struct_map[struct_name]
        return None
    
    def get_all_struct(self) -> list[AuxStructure]:
        return self._aux_struct
    
    def has_struct(self, struct_name) -> bool:
        return struct_name in self._struct_map
    
    def validate(self):
        ret = []
        for each_struct in self._aux_struct:
            err = each_struct.validate()
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
        for each_struct in self._aux_struct:
            ret.append(str(each_struct))
            ret.append('')
        return "\n".join(ret)