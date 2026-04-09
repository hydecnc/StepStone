class AssistantNotInitialize(Exception):
    def __init__(self):
        super().__init__('Assitant is not initialized')
        
class UnuseableModule(Exception):
    def __init__(self, mod):
        super().__init__('{} module is base module which should never be used directly'.format(mod))

class ChatTypeError(Exception):
    def __init__(self):
        super().__init__('--chat-type must be [assistant|conversation]')
        
class ErrorInResponse(Exception):
    def __init__(self):
        super().__init__('LLM does not respond your prompt properly')