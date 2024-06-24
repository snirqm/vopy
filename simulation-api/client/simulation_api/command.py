from functools import cached_property


class Command:
    pass

    def encode(self) -> bytes:
        pass

    @cached_property
    def has_response(self) -> bool:
        pass


class CommandResponse:
    pass
    @staticmethod
    def decode(data: bytes) -> CommandResponse:
        pass
