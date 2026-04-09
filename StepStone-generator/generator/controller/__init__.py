import re

class Controller():
    def __init__(self, cons):
        self.patch = None
        self.console = cons
    
    def regex_get(self, regex, line, index) -> str:
        m = re.search(regex, line)
        if m != None and len(m.groups()) > index:
            return m.groups()[index]
        return None
    
    def regex_match(self, regex, line):
        m = re.search(regex, line)
        if m != None and len(m.group()) != 0:
            return True
        return False
    
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
    
    def _remove_code_marker(self, data):
        t = data.split('\n')
        if t[0].startswith('```'):
            t = t[1:]
        if t[-1].startswith('```'):
            t = t[:len(t)-1]
        return '\n'.join(t)
    
    def clean_str(self, st):
        st = self._remove_noisy_begining(st)
        st = self._remove_noisy_ending(st)
        return st