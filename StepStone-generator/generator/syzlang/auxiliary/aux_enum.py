from .aux_helper import *

class AuxEnum():
    TPYE='enum'
    @report_init_error
    def __init__(self, data: dict) -> None:
        self.enum_name = data['enum_name']
        self.enum_eles = data['enum_elements']
        self.enum_vals = data['enum_values']
        
    def __repr__(self) -> str:
        return self.__str__()
        
    @report_str_error
    def __str__(self) -> str:
        ret = "{} = {}".format(self.enum_name, ", ".join(self.enum_eles))
        return ret
    
    def definition(self) -> str:
        ret = []
        line = "{}\t\t= {}"
        for i in range(len(self.enum_eles)):
            if i >= len(self.enum_eles) or i >= len(self.enum_vals):
                break
            ret.append(line.format(self.enum_eles[i], self.enum_vals[i]))
        return "\n".join(ret)
    
    def validate(self) -> str | None:
        ret = []
        if len(self.enum_eles) != len(self.enum_vals):
            ret.append("Enum {} should have the same number of enum elements and values, however, it has {} elements and {} values.".format(self.enum_name, len(self.enum_eles), len(self.enum_vals)))
        if len(self.enum_eles) == 0:
            ret.append("Enum {} has zero element.".format(self.enum_name))
        if len(self.enum_vals) == 0:
            ret.append("Enum {} has zero value.".format(self.enum_name))
        if len(ret) == 0:
            return ret
        return None
    
class EnumBulk():
    TPYE='enum'
    def __init__(self, enum_json):
        self._aux_enum = []
        self._enum_map = {}
        for data in enum_json:
            e = AuxEnum(data)
            self._aux_enum.append(e)
            self._enum_map[e.enum_name] = e
        
    def find_enum(self, enum_name) -> AuxEnum | None:
        if enum_name in self._enum_map:
            return self._enum_map[enum_name]
        return None
    
    def get_all_enum(self) -> list:
        return self._aux_enum
    
    def has_enum(self, enum_name) -> bool:
        return enum_name in self._enum_map
    
    def validate(self) -> None | list:
        ret = []
        for each_enum in self._aux_enum:
            err = each_enum.validate()
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
        for each_enum in self._aux_enum:
            ret.append(str(each_enum))
            ret.append('')
        return "\n".join(ret)
    
    def definition(self) -> str:
        ret = []
        for each_enum in self._aux_enum:
            ret.append(each_enum.definition())
            ret.append('')
        return "\n".join(ret)