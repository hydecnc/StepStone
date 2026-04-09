class SyzlangInvalidSize(Exception):
    def __init__(self, size):
        super().__init__('Size must be one of [int8, int16, int32, int64, intptr], but receive {}'.format(size))
        
class SyzlangMismatchEnum(Exception):
    def __init__(self, name):
        super().__init__('Length of enum {} and its value does not match'.format(name))
        
class ConstValueNotSpecified(Exception):
    def __init__(self, st):
        super().__init__('Const value in {} was not found'.format(st))
        
class SizeIsEmpty(Exception):
    def __init__(self, st):
        super().__init__('Size in {} is empty'.format(st))
        
class LenTargetIsEmpty(Exception):
    def __init__(self, st):
        super().__init__('Length target in {} is empty'.format(st))
        
class IncompatiableBuilderType(Exception):
    def __init__(self, type) -> None:
        super().__init__('{} is an incompatiable type'.format(type))
        
class SyzlangTypeInitError(Exception):
    def __init__(self, obj, type) -> None:
        super().__init__('Failed tp parse object `{}` to `{}` type'.format(obj, type))
        
class SyzlangTypeStrError(Exception):
    def __init__(self, obj, type, e) -> None:
        super().__init__('Parsing `{}` object `{}` encountered an error: {}'.format(type, obj, e))
        
class SyzlangObjInitError(Exception):
    def __init__(self, obj, type, e) -> None:
        super().__init__('Failed to parse obj to {}\n```\n{}\n```\n: {}'.format(type, obj, e))
        
class SyzlangObjStrError(Exception):
    def __init__(self, obj, type, e) -> None:
        if type == 'resource':
            obj_name = obj.res_name
        if type == 'enum':
            obj_name = obj.enum_name
        if type == 'struct':
            obj_name = obj.struct_name
        if type == 'api':
            obj_name = obj.api_name
        super().__init__('Parsing {} object {} encountered an error: {}'.format(type, obj_name, e))