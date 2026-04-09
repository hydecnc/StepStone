from openai.pagination import SyncCursorPage
from openai.types.beta.assistant import Assistant
from typing_extensions import override
from openai import AssistantEventHandler, OpenAI

import os
import random
import time
import logging
from generator.llm import *
from generator.llm.error import AssistantNotInitialize

def check_assistant(func):
    def inner(self, **kwargs):
        if self._assistant == None:
            raise AssistantNotInitialize()
        return func(self, **kwargs)
    return inner
class EventHandler(AssistantEventHandler):
    def __init__(self, client) -> None:
        super().__init__()
        self.client: OpenAI = client
    
    @override
    def on_text_created(self, text) -> None:
        print(f"\nassistant > ", end="", flush=True)

    @override
    def on_tool_call_created(self, tool_call):
        print(f"\nassistant > {tool_call.type}\n", flush=True)

    @override
    def on_message_done(self, message) -> None:
        # print a citation to the file searched
        message_content = message.content[0].text
        annotations = message_content.annotations
        citations = []
        for index, annotation in enumerate(annotations):
            message_content.value = message_content.value.replace(
                annotation.text, f"[{index}]"
            )
            if file_citation := getattr(annotation, "file_citation", None):
                cited_file = self.client.files.retrieve(file_citation.file_id)
                citations.append(f"[{index}] {cited_file.filename}")

        print(message_content.value)
        print("\n".join(citations))

class ChatGPTAssistant(LLM):
    def __init__(self, client, console, logger, engine, temperature, max_tokens):
        super().__init__(logger)
        self._engine = engine
        self._temperature = temperature
        self._max_tokens = max_tokens
        self._chat_history = None
        self.client: OpenAI = client
        self.console = console
        self._assistant = None
        self._vector_store_ids = []
        self._file_ids = []
        self._ci_files = []
        self._fs_files = []
        self._enable_file_search = False
        self._enable_code_interpreter = False
        self.thread = self.client.beta.threads.create()
        super().set_api_key(self.client.api_key)
    
    def list_assistants(self) -> SyncCursorPage[Assistant]:
        return self.client.beta.assistants.list().data
    
    def retrieve_assistant(self, assistant_id):
        if self._assistant is None:
            self._assistant = self.client.beta.assistants.retrieve(assistant_id=assistant_id)
        return self._assistant
    
    def create_assistant(self, name: str, instructions: str, file_search: bool=False, code_interpreter:bool=False):
        tools = []
        #[{"type": "code_interpreter"}, {"type": "file_search"}]
        self._enable_code_interpreter = code_interpreter
        self._enable_file_search = file_search
        if file_search:
            tools.append({"type": "file_search"})
        if code_interpreter:
            tools.append({"type": "code_interpreter"})
        self._assistant = self.client.beta.assistants.create(
            name=name,
            instructions=instructions,
            tools=tools,
            model=self._engine,
            temperature=self._temperature,
            )
        return self._assistant
    
    def add_file_search(self, *args):
        if not self._enable_file_search:
            logging.INFO("File search is not enabled")
            return
        file_paths = []
        file_streams = []
        for file_path in args:
            if not os.path.exists(file_path):
                raise FileExistsError(file_path)
        
            file_paths.append(file_path)
        file_streams = [open(path, "rb") for path in file_paths]
        
        if len(file_streams) == 0:
            return
        
        vector_name =  "vector_" + str(random.randrange(100000000))
        vector_store = self.client.beta.vector_stores.create(name=vector_name)
        
        file_batch = self.client.beta.vector_stores.file_batches.upload_and_poll(
            vector_store_id=vector_store.id, files=file_streams
        )
        
        logging.debug("File status", file_batch.status)
        logging.debug("File counts", file_batch.file_counts)
        
        self._vector_store_ids.append(vector_store.id)
        self.update_assistant()
        
    def add_code_interpreter(self, *args):
        if not self._enable_code_interpreter:
            logging.INFO("Code interpreter is not enabled")
            return
        for file_path in args:
            if not os.path.exists(file_path):
                raise FileExistsError(file_path)
        
            file = self.client.files.create(
                file=open(file_path, "rb"),
                purpose='assistants')
            self._ci_files.append(file)
            self._file_ids.append(file.id)
            logging.debug("File id", file.id)
        self.update_assistant()
        
    @check_assistant
    def update_assistant(self):
        tool_resources = {}
        if len(self._vector_store_ids) > 0:
            tool_resources["file_search"] = {"vector_store_ids": self._vector_store_ids}
        if len(self._file_ids) > 0:
            tool_resources["code_interpreter"] = {"file_ids": self._file_ids}
        self.client.beta.assistants.update(
            assistant_id=self._assistant.id,
            tool_resources=tool_resources,
        )
    
    def chat(self, message=None, **kwargs):
        self.append_user_message(message)
        self.logger.info("> {}: {}".format(self._assistant.name, message))
        response = self._send_request(formatted_messages=self.messages, **kwargs)
        if response == None:
            raise ErrorInResponse
        response_text = response[0].content[0].text.value
        self.logger.info("< {}: {}".format(self._assistant.name, response_text))
        self.messages = self.client.beta.threads.messages.list(thread_id=self.thread.id).data
        self.logger.debug("[DEBUG] {} msg left in assistant".format(len(self.messages)))
        return response_text
        
    @log_message
    def _append_message(self, message, role):
        if self._msg_header != None:
            message = "{}\n{}".format(self._msg_header, message)
        msg = self.client.beta.threads.messages.create(
            thread_id=self.thread.id,
            role=role,
            content=message,
        )
        self.logger.debug("[DEBUG] append assistant msg `{}`".format(msg.id))
        return
    
    def pop_message(self, num):
        for i in range(num):
            msg = self.messages[i]
            self.client.beta.threads.messages.delete(
                thread_id=self.thread.id,
                message_id=msg.id)
            self.logger.debug("[DEBUG] pop assistant msg `{}`".format(msg.id))
        self.messages = self.client.beta.threads.messages.list(thread_id=self.thread.id).data
        self.logger.debug("[DEBUG] {} msg left in assistant".format(len(self.messages)))
        return
        
    @check_api
    def _send_request(self, formatted_messages, instructions=None, model=None, temperature=None, max_tokens=None):
        time.sleep(0.2) # avoid rate limit
        
        if model == None:
            model = self._engine
        if temperature == None:
            temperature = self._temperature
        if max_tokens == None:
            max_tokens = self._max_tokens
        self.logger.debug("SENDING: {}".format(formatted_messages))
        try:
            with self.client.beta.threads.runs.stream(
                thread_id=self.thread.id,
                assistant_id=self._assistant.id,
                instructions=instructions,
                model=model,
                max_completion_tokens=max_tokens,
                max_prompt_tokens=max_tokens,
                temperature=temperature,
                top_p=1,
                event_handler=EventHandler(self.client),
            ) as stream:
                stream.until_done()
                response = stream.get_final_messages()
        except Exception as e:
            logging.error(e)
            return None

        self.logger.debug("RECEIVING: {}".format(response[0].content[0].text.value))
        return response