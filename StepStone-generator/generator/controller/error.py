class MISSING_ANALYZOR(Exception):
    def __init__(self):
        super().__init__('Can not perform analysis because of missing handler')
        
class ChatGPTAssistantNotSelected(Exception):
    def __init__(self):
        super().__init__('ChatGPT assistant need to be selected before using')
        
class InvalidAPIFormat(Exception):
    def __init__(self, api):
        super().__init__('{} is an invalid API'.format(api))
        
class LLMRandomError(Exception):
    def __init__(self, error):
        super().__init__('LLM random error: {}'.format(error))