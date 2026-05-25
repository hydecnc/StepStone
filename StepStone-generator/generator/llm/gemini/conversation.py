from __future__ import annotations

import logging
from typing import TYPE_CHECKING, final

from google import genai
from google.genai import types

if TYPE_CHECKING:
    from generator.cons import CusConsole
from generator.llm import LLM, check_api, log_message


@final
class Conversation(LLM):
    def __init__(
        self,
        api_key: str,
        client: genai.Client,
        console: CusConsole,
        logger: logging.Logger | None,
        engine: str,
        temperature: int,
        max_tokens: int | None,
    ):
        super().__init__(logger)
        self.logger: logging.Logger | None
        self._engine = engine
        self._temperature = temperature
        self._max_tokens = max_tokens
        self._chat_history = None
        self.console = console
        self.client: genai.Client = client
        super().set_api_key(api_key)

    def chat(self, message: str, **kwargs) -> str | None:
        self.append_user_message(message)
        if self.logger:
            self.logger.info("> Gemini: {}".format(message))
        response = self._send_request(formatted_messages=self.messages, **kwargs)
        if not response:
            return None
        response_text = response.text
        if self.logger:
            self.logger.info("Received: {}".format(response_text))

        self.append_assistant_message(response_text)
        return response_text

    @log_message
    def _append_message(self, message: str, role: str):
        self.messages.append({"role": role, "content": message})

    @check_api
    def _send_request(
        self,
        formatted_messages: list[dict[str, str]],
        model: str | None = None,
        temperature: int | None = None,
    ) -> types.GenerateContentResponse | None:
        if model is None:
            model = self._engine
        if temperature is None:
            temperature = self._temperature
        if self.logger:
            self.logger.debug(
                "[engine: {}] SENDING: {}".format(
                    model, "\n".join([e["content"] for e in formatted_messages])
                )
            )
        try:
            response = self.client.models.generate_content(
                model=model,
                contents=self._convert_messages(formatted_messages),
                config=genai.types.GenerateContentConfig(temperature=temperature),
            )
        except Exception as e:
            logging.error(e)
            return None

        if self.logger:
            self.logger.debug("RECEIVING: {}".format(response.text))

        return response

    def _convert_messages(
        self, formatted_messages: list[dict[str, str]]
    ) -> list[dict[str, str | list[dict[str, str]]]]:
        result: list[dict[str, str | list[dict[str, str]]]] = []
        for msg in formatted_messages:
            role = msg["role"]
            if role == "assistant":
                role = "model"
            if role == "system":
                continue
            result.append({"role": role, "parts": [{"text": msg["content"]}]})
        return result
