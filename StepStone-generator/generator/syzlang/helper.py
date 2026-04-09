from .auxiliary import *

class SyzlangInvalidSize(Exception):
    def __init__(self, size):
        super().__init__('Size must be one of [int8, int16, int32, int64, intptr], but receive {}'.format(size))
        
class SyzlangMismatchEnum(Exception):
    def __init__(self, name):
        super().__init__('Length of enum {} and its value does not match'.format(name))

class SyzlangHelper():
    def __init__(self, header) -> None:
        self.header = header + "\n" + "meta arches[\"amd64\"]"
        self.init_helper()
    
    def init_helper(self):
        self._resource = ["# Resource Definition"]
        self._enum = ["# Enum Definition"]
        self._structure = ["# Structure Definition"]
        self._api = ["# API Definition"]
        self._def = ["arches = 386, amd64, arm, arm64, mips64le, ppc64le, riscv64, s390x"]
        
    def parse_resource_section(self, res_bulk: ResourceBulk) -> None:
        self._resource.append(str(res_bulk))
        return

    def parse_enum_section(self, enum_bulk: EnumBulk) -> None:
        self._enum.append(str(enum_bulk))
        self._def.append(enum_bulk.definition())
        return
    
    def parse_structure_section(self, struct_bulk: StructureBulk) -> None:
        self._structure.append(str(struct_bulk))
        return
    
    def parse_api_section(self, api_bulk: APIBulk):
        self._api.append(str(api_bulk))
        return
        
    def merge_object(self, old_object: dict, new_object: dict)-> dict:
        self._add_object_key('resource', old_object, new_object)
        self._add_object_key('enum', old_object, new_object)
        self._add_object_key('structure', old_object, new_object)
        self._add_object_key('api', old_object, new_object)
        return old_object
    
    def _add_object_key(self, key, old_object: dict, new_object: dict):
        old_properties = [x for x in old_object[key]]
        for each in new_object[key]:
            new_key = True
            for property in old_properties:
                if each['{}_name'.format(key)] == property['{}_name'.format(key)]:
                    new_key = False
                    break
            if new_key:
                old_object[key].append(each)
    
    def generate_description(self, j: dict, j_congregate=None):
        self.init_helper()
        res_bulk = ResourceBulk(j["resource"])
        enum_bulk = EnumBulk(j["enum"])
        struct_bulk = StructureBulk(j["structure"])
        api_bulk = APIBulk(j["api"], res_bulk, enum_bulk, struct_bulk)
        self.parse_resource_section(res_bulk)
        self.parse_enum_section(enum_bulk)
        self.parse_structure_section(struct_bulk)
        self.parse_api_section(api_bulk)
        return self._generate_description()
    
    def validate_syzlang_json(self, j: dict):
        self.init_helper()
        errs = []
        try:
            res_bulk = ResourceBulk(j["resource"])
            res_err = res_bulk.validate()
            if res_err != None:
                errs.append(res_err)
        except SyzlangObjInitError as e:
            errs.append(str(e))
        except SyzlangObjStrError as e:
            errs.append(str(e))
        
        try:
            enum_bulk = EnumBulk(j["enum"])
            enum_err = enum_bulk.validate()
            if enum_err != None:
                errs.append(enum_err)
        except SyzlangObjInitError as e:
            errs.append(str(e))
        except SyzlangObjStrError as e:
            errs.append(str(e))
            
        try:
            struct_bulk = StructureBulk(j["structure"])
            struct_err = struct_bulk.validate()
            if struct_err != None:
                errs.append(struct_err)
        except SyzlangObjInitError as e:
            errs.append(str(e))
        except SyzlangObjStrError as e:
            errs.append(str(e))
        
        try:
            api_bulk = APIBulk(j["api"], res_bulk, enum_bulk, struct_bulk)
            api_err = api_bulk.validate()
            if api_err != None:
                errs.append(api_err)
        except SyzlangObjInitError as e:
            errs.append(str(e))
        except SyzlangObjStrError as e:
            errs.append(str(e))
        return self._print_error(errs)
    
    def get_structbulk(self, j: dict):
        return StructureBulk(j["structure"])
    
    def generate_const(self, j: dict):
        self.init_helper()
        enum_bulk = EnumBulk(j["enum"])
        self.parse_enum_section(enum_bulk)
        return "\n".join(self._def)
    
    def _generate_description(self):
        text = []
        text.append(self.header)
        text.append('')
        text.extend(self._resource)
        text.append('')
        text.extend(self._enum)
        text.append('')
        text.extend(self._structure)
        text.append('')
        text.extend(self._api)
        return "\n".join(text)

    def _print_error(self, err_list):
        ret = []
        for errs in err_list:
            for each_err in errs:
                ret.append('- {}'.format(each_err))
        return "\n".join(ret)