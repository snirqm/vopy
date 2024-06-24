import asyncio
from simulation_api.client import SimulationApiClient
from grpclib.client import Channel

import gen.vopy as vopy


class VOpyClient:
    def __init__(self, host: str, public_api_port: int):
        self.host = host
        self.channel = Channel(host=host, port=public_api_port)
        self.service = vopy.PublicApiStub(self.channel)
        self.simulation = None

    def __del__(self):
        self.channel.close()

    async def get_status(self) -> vopy.Status:
        return await self.service.get_status(vopy.Empty())

    async def create_session(self) -> vopy.Session:
        return await self.service.create_session(vopy.Empty())

    async def send_config(self, config: vopy.Config) -> vopy.Empty:
        return await self.service.send_config(config)

    async def start_sim(self, session: vopy.Session) -> vopy.SimulationInfo:
        return await self.service.start_sim(session)

    async def start_simulation(self, config_with_no_session: vopy.Config) -> SimulationApiClient:
        session = await self.create_session()
        config_with_session = vopy.Config()
        config_with_session.MergeFrom(config_with_no_session)
        config_with_session.session.CopyFrom(session)
        await self.send_config(config_with_session)
        sim_info = await self.start_sim(session)
        self.simulation = SimulationApiClient(self.host, sim_info)
        await self.simulation.connect()


async def main():
    client = VOpyClient("127.0.0.1", 9510)
    status = await client.get_status()
    print(status)


if __name__ == "__main__":
    loop = asyncio.get_event_loop()
    loop.run_until_complete(main())
