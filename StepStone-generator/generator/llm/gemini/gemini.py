from __future__ import annotations

import logging
from typing import TYPE_CHECKING, final

from google import genai
from typing_extensions import override

if TYPE_CHECKING:
    from generator.cons import CusConsole
from generator.llm import LLM
from generator.llm.error import ChatTypeError

from .assistant import GeminiAssistant
from .conversation import Conversation


@final
class Gemini(LLM):
    OP_INIT: int = 0
    OP_SERLZ: int = 1
    OP_FOLUP: int = 2
    OP_CRTFMT: int = 3

    def __init__(
        self,
        engine: str = "gemini-2.0-flash",
        temperature: int = 1,
        max_tokens: int | None = None,
    ) -> None:
        super().__init__()
        self._engine = engine
        self._temperature = temperature
        self._max_tokens: int | None = max_tokens
        self._chat_history = None
        self.client: genai.Client | None = None
        self._api_key: str

    @override
    def __str__(self) -> str:
        return "gemini"

    @override
    def set_api_key(self, api_key: str) -> None:
        super().set_api_key(api_key=api_key)
        self.client = genai.Client(api_key=self._api_key)

    def verify_api(self) -> bool:
        return self.client is not None

    def start_new_assistant_session(
        self,
        console: CusConsole,
        logger: logging.Logger,
        engine: str | None = None,
        temperature: int | None = None,
        max_tokens: int | None = None,
    ) -> LLM:
        return self.start_new_session(
            type="assistant",
            console=console,
            logger=logger,
            engine=engine,
            temperature=temperature,
            max_tokens=max_tokens,
        )

    def start_new_conversation_session(
        self,
        console: CusConsole,
        logger: logging.Logger,
        engine: str | None = None,
        temperature: int | None = None,
        max_tokens: int | None = None,
    ) -> LLM:
        return self.start_new_session(
            type="conversation",
            console=console,
            logger=logger,
            engine=engine,
            temperature=temperature,
            max_tokens=max_tokens,
        )

    def start_new_session(
        self,
        type: str,
        console: CusConsole,
        logger: logging.Logger,
        engine: str | None = None,
        temperature: int | None = None,
        max_tokens: int | None = None,
    ) -> LLM:
        self.lock_chat_session()
        if engine is None:
            engine = self._engine
        if temperature is None:
            temperature = self._temperature
        if max_tokens is None:
            max_tokens = self._max_tokens

        if type == "conversation" and self.client is not None:
            self._chat_session[self._chat_index] = Conversation(
                self._api_key,
                self.client,
                console,
                logger,
                engine,
                temperature,
                max_tokens,
            )
        if type == "assistant" and self.client is not None:
            self._chat_session[self._chat_index] = GeminiAssistant(
                self._api_key,
                self.client,
                console,
                logger,
                engine,
                temperature,
                max_tokens,
            )
        try:
            session: LLM = self._chat_session[self._chat_index]
        except:
            raise ChatTypeError()
        self._chat_index += 1
        self.unlock_chat_session()
        return session
