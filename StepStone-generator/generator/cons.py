import json
import os

from rich.console import Console

from generator.cli import Cli
from generator.error import *

MODE_INTERACTIVE = 0
MODE_CLI = 1


class CusConsole(Cli):
    CHATGPT = 1
    BARD = 2
    GEMINI = 3
    CHATGPT_ASSISTANT = 2
    GEMINI_ASSISTANT = 3

    def __init__(self, mode, debug=False, **kwargs) -> None:
        super().__init__(debug, **kwargs)
        self.debug = debug
        if mode == MODE_INTERACTIVE or self.verbose:
            self.console = Console(record=True)
        if mode == MODE_CLI and not self.verbose:
            f = open(os.path.join(self._output, "chat_history.log"), "wt")
            self.console = Console(record=True, file=f)

    # def __del__(self):
    #     now = datetime.datetime.now()
    #     dt_string = now.strftime("%d-%m-%Y-%H-%M-%S")
    #     log_file = os.getcwd() + "/chat_history-" + dt_string + ".log"
    #     self.console.save_text(log_file)

    def display(self):
        self.console.clear()
        self.print("Setup LLM", style="bold yellow")
        while True:
            self.print("Please choose an option from below:", style="bold cyan")
            self.print("{}. ChatGPT".format(self.CHATGPT))
            self.print("{}. Google Bard".format(self.BARD))
            self.print("{}. Google Gemini ".format(self.GEMINI))

            option = int(self.user_input("option> "))
            self.set_api_key(option)
            if option == self.CHATGPT:
                self.create_assistant()
            break

    def set_api_key(self, option):
        api_key = None
        if option == self.CHATGPT:
            self._llm_name = "chatgpt"
        if option == self.BARD:
            self._llm_name = "bard"
        if option == self.GEMINI:
            self._llm_name = "gemini"

        self.cfg.set_llm_name(self._llm_name)
        api_key = self.cfg.find_api_key()
        if api_key != None:
            reset = self.user_input("Reset api_key> (N/y)")
            if reset == "y":
                api_key == None
        if api_key is None:
            self.chatgpt_response_print("Please enter your API key", style="bold red")
            api_key = self.user_input("api_key> ")
            self.cfg.save_api_key(api_key)
        self.llm = self._init_llm()
        if not self.llm.verify_api():
            try:
                self.llm.set_api_key(api_key)
            except API_KEY_IS_MISSING:
                api_key = self.user_input("api_key> ")
                self.cfg.save_api_key(api_key)
                self.llm.set_api_key(api_key)
        return

    def create_assistant(self):
        session = self.llm.start_new_assistant_session(
            console=self.console, logger=self.case_logger
        )
        while True:
            self.print("All existing assistants were listed below:")
            assistants = session.list_assistants()
            for i in range(len(assistants)):
                assistant = assistants[i]
                self.console.rule("")
                self.print(
                    "\t{}. [bold red]{}[/bold red]\t[ID: [bold green]{}[/bold green]  model: [bold green]{}[/bold green]  temp: [bold green]{}[/bold green]]".format(
                        i,
                        assistant.name,
                        assistant.id,
                        assistant.model,
                        assistant.temperature,
                    )
                )
                # self.print("\tInstructions: {}".format(assistant.instructions))
                # self.print("\tDescription{}".format(assistant.description))
                # self.print("\tTools{}".format(assistant.tools))
            selected = self.user_input(
                "Select an existing assistant or enter 'n' to create a new one> "
            )
            if selected == "n":
                self.print("Creating a new assistant")
                name = self.user_input("Name> ")
                instructions = self.user_input("Instructions> ")
                enable_file_search = self.user_input("Enable file search> (N/y)")
                if enable_file_search == "y":
                    enable_file_search = True
                else:
                    enable_file_search = False
                enable_code_interpreter = self.user_input(
                    "Enable code interpreter> (N/y)"
                )
                if enable_code_interpreter == "y":
                    enable_code_interpreter = True
                else:
                    enable_code_interpreter = False
                session.create_assistant(
                    name=name,
                    instructions=instructions,
                    file_search=enable_file_search,
                    code_interpreter=enable_code_interpreter,
                )
                self.console.clear()
                continue
            try:
                selected = int(selected)
            except ValueError(selected):
                self.print("Invalid option {}".format(selected))
                continue
            selected_assistant = assistants[selected]
            self.cfg.save_selected_assistant(
                selected_assistant.name, selected_assistant.id
            )
            break

    def cli_run(self, cuda_api_text: list):
        self.generate_syzlange(cuda_api_text)

    def pick_app_type(self):
        self.print("Select app", style="bold cyan")
        self.print("1. ChatGPT")
        self.print("2. Assistant")
        app = self.user_input("app> ")
        if app == "1":
            return self.CHATGPT
        if app == "2":
            return self.CHATGPT_ASSISTANT
        raise ValueError(app)

    def user_input(self, prompt):
        self.console.rule("[bold red]User")
        return self.console.input(prompt)

    def user_print(self, message, **kwargs):
        self.console.rule("[bold red]User")
        self.print("user> " + message, **kwargs)

    def chatgpt_response_print(self, response, **kwargs):
        self.console.rule("[bold green]ChatGPT")
        self.print("chatgpt> " + response, **kwargs)

    def console_print(self, msg, **kwargs):
        self.console.rule("[bold orange1]Console")
        self.print(msg, **kwargs)

    def print(self, msg, **kwargs):
        self.console.print(msg, **kwargs)
        if self.case_logger != None:
            self.case_logger.info(msg)

    def save_file(self, data, file_name):
        file_path = os.path.join(self._output, file_name)
        if os.path.exists(file_path):
            self.print('File path "{}" already exist'.format(file_path))
            return
        with open(file_path, "w") as fp:
            fp.write(data)

    def dump_json(self, data, file_name):
        file_path = os.path.join(self._output, file_name)
        with open(file_path, "w") as fp:
            json.dump(data, fp)
