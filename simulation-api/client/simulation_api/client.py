from __future__ import annotations

import asyncio
from simulation_api.command import Command, CommandResponse
class SimulationApiClient:
    def __init__(self, host, port):
        self.reader: asyncio.StreamReader | None = None
        self.writer: asyncio.StreamWriter | None = None
        self.host = host
        self.port = port

    async def connect(self):
        self.reader, self.writer = await asyncio.open_connection(self.host, self.port)

    async def send_operation(self, command: Command) -> CommandResponse:
        self.writer.write(command.encode())
        await self.writer.drain()
        if command.has_response:
            response = await self.reader.read(100)
            return CommandResponse.decode(response)

