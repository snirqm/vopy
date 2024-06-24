import asyncio

from gen.vopy import PublicApiBase, Empty, Status, Session, Config, SimulationInfo
from grpclib.server import Server


class PublicService(PublicApiBase):
    async def get_status(self, empty: Empty) -> Status:
        # TODO: implement this method
        return Status()

    async def create_session(self, empty: Empty) -> Session:
        # TODO: implement this method
        return Session()

    async def send_config(self, config: Config) -> Empty:
        # TODO: implement this method
        return Empty()

    async def start_sim(self, session: Session) -> SimulationInfo:
        # TODO: implement this method
        return SimulationInfo()


async def main():
    server = Server([PublicService()])
    await server.start("127.0.0.1", 9510)
    await server.wait_closed()

def start_server():
    loop = asyncio.get_event_loop()
    loop.run_until_complete(main())

if __name__ == '__main__':
    start_server()
