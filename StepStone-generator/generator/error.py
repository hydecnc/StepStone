class ConfigFormatError(Exception):
    def __init__(self, key):
        super().__init__('Can not find the key {} in the config'.format(key))
    
class WorkSpaceExist(Exception):
    def __init__(self, path):
        super().__init__('The workspace already {} exist'.format(path))
        
class API_KEY_IS_MISSING(Exception):
    def __init__(self, key):
        super().__init__('Can not find the key {} in the config'.format(key))