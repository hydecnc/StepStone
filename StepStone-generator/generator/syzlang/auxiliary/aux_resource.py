from .aux_helper import *
        
class AuxResource():
    TYPE='resource'
    @report_init_error
    def __init__(self, data: dict) -> None:
        self.res_name = data['resource_name']
        self.res_type = data['resource_type']
        
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_str_error
    def __str__(self) -> str:
        res = "resource {}[{}]".format(self.res_name, self.res_type)
        return res
    
    def validate(self) -> list | None:
        ret = []
        if self.res_type not in ['int8', 'int16', 'int32', 'int64', 'intptr']:
            ret.append("Resource {}'s type should be one of ['int8', 'int16', 'int32', 'int64', 'intptr'] but received {}".format(self.res_name, self.res_type))
        if self.res_name in ['const', 'int8', 'int16', 'int32', 'int64', 'intptr', 'array', 'ptr', 'string', 'stringnoz', 'glob', 'fmt', 'len', 'bytesize', 'bitsize', 'offsetof', 'vma', 'proc', 'compressed_image', 'text', 'void']:
            ret.append("Resource name '{}' is illegit, resource name cannot be one of ['const', 'int8', 'int16', 'int32', 'int64', 'intptr', 'array', 'ptr', 'string', 'stringnoz', 'glob', 'fmt', 'len', 'bytesize', 'bitsize', 'offsetof', 'vma', 'proc', 'compressed_image', 'text', 'void']")
        if len(ret) == 0:
            return ret
        return None
    
class ResourceBulk():
    TYPE='resource'
    def __init__(self, resource_json: dict) -> None:
        self._aux_res = []
        self._res_map = {}
        for data in resource_json:
            r = AuxResource(data)
            self._aux_res.append(r)
            self._res_map[r.res_name] = r
            
    def find_resource(self, resource_name) -> AuxResource | None:
        if resource_name in self._res_map:
            return self._res_map[resource_name]
        return None
    
    def get_all_resource(self) -> list:
        return self._aux_res
    
    def has_resource(self, resource_name) -> bool:
        return resource_name in self._res_map
    
    def validate(self):
        ret = []
        for each_res in self._aux_res:
            err = each_res.validate()
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
        for each_res in self._aux_res:
            ret.append(str(each_res))
        return "\n".join(ret)