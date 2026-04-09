class StrCrtl():
    def __init__(self) -> None:
        pass
    
    def _remove_noisy_ending(self, line):
        i = len(line) - 1
        while i >= 0:
            if line[i] not in ['\n', '\r', '\t', ' ']:
                break
            i -= 1
        return line[:i+1]
    
    def _remove_noisy_begining(self, line):
        i = 0
        while i < len(line):
            if line[i] not in ['\n', '\r', '\t', ' ']:
                break
            i += 1
        return line[i:]
    
    def _remove_pointer(self, line):
        i = len(line) - 1
        while i >= 0:
            if line[i] != '*':
                break
            i -= 1
        return line[:i+1]
    
    def clean_str(self, st):
        st = self._remove_noisy_begining(st)
        st = self._remove_noisy_ending(st)
        return st

class Struct(StrCrtl):
    def __init__(self) -> None:
        super().__init__()
        self.type = 'struct'
        self.name = None
        self.fields = []
        self.type_cast = None
        self.raw_data = None
        
    def parse_field(self, line):
        try:
            end = line.index(';')
        except ValueError:
            return
        arg = line[:end]
        arg_list = arg.split(' ')
        arg_type = self.clean_str(' '.join(arg_list[:-1]))
        if arg_type.startswith('const '):
            arg_type = arg_type[6:]
        if arg_type.endswith('*'):
            arg_type = self._remove_pointer(arg_type)
        arg_name = self.clean_str(arg_list[-1])
        if arg_type != '}':
            self.add_field(arg_type, arg_name)
                
    def add_field(self, type, name):
        self.fields.append({'field_type': type, 'field_name': name})
        
class Enum(StrCrtl):
    def __init__(self) -> None:
        super().__init__()
        self.type = 'enum'
        self.name = None
        self.fields = []
        self.type_cast = None
        self.raw_data = None
        
    def parse_field(self, line):
        pass
                
    def add_field(self, name, value):
        pass
        
class Union(StrCrtl):
    def __init__(self) -> None:
        super().__init__()
        self.type = 'union'
        self.name = None
        self.fields = []
        self.type_cast = None
        self.raw_data = None
        
    def parse_field(self, line):
        try:
            end = line.index(';')
        except ValueError:
            return
        arg = line[:end]
        arg_list = arg.split(' ')
        arg_type = self.clean_str(' '.join(arg_list[:-1]))
        if arg_type.startswith('const '):
            arg_type = arg_type[6:]
        if arg_type.endswith('*'):
            arg_type = self._remove_pointer(arg_type)
        arg_name = self.clean_str(arg_list[-1])
        if arg_type != '}':
            self.add_field(arg_type, arg_name)
                
    def add_field(self, type, name):
        self.fields.append({'field_type': type, 'field_name': name})
        
class Oneline(StrCrtl):
    def __init__(self) -> None:
        super().__init__()
        self.type = 'oneline'
        self.type_from = None
        self.type_cast = None
        self.raw_data = None
        self.fields = []