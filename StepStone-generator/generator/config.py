import json

class Config():
    def __init__(self, config_path, llm_name) -> None:
        self._config_path = config_path
        self._cfg = self._load_config(config_path)
        self._llm = llm_name
    
    def set_llm_name(self, llm_name):
        self._llm = llm_name

    def find_api_key(self):
        return self.find_cfg_key("llm", self._llm, "api_key")
    
    def find_assistant(self, name):
        return self.find_cfg_key("llm", self._llm, "asst_" + name)

    def find_library_header(self):
        return self.find_cfg_key("library", "header")
    
    def find_library_db(self):
        return self.find_cfg_key("library", "code_db")
    
    def save_assistant(self, name, id):
        self.save_llm_key("asst_" + name, id)
    
    def save_api_key(self, api_key):
        self.save_llm_key("api_key", api_key)
    
    def find_cfg_key(self, *args):
        if self._cfg == None:
            return None
        
        cfg = self._cfg
        for key in args:
            if key not in cfg:
                return None
            cfg = cfg[key]
        return cfg

    def _load_config(self, config_path):
        try:
            cfg = json.load(open(config_path, "r"))
            return cfg
        except:
            return None
    
    def save_llm_key(self, key, value):
        cfg = self._cfg
        if cfg == None:
            cfg = {}
        if self._llm not in cfg:
            cfg[self._llm] = {}
        cfg[self._llm][key] = value
        self._cfg = cfg
        json.dump(cfg, open(self._config_path, "w"), indent=4)