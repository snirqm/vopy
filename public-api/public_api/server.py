import asyncio
from typing import List

from gen.vopy import PublicApiStub, Empty, Status, Session, Config, SimulationInfo
from grpclib.server import Server
import itertools
import os
from datetime import datetime


class PublicService(PublicApiStub):
    def __init__(self):
        self.simulation_processes: List[asyncio.subprocess.Process] = []
        self.sessions: List[Session] = []
        self.key_generator = itertools.count()
        self.path_to_simulation_files = r""
        self.server_dir_name = self.get_server_dir_name()

    @property
    def server_dir_path(self):
        return os.path.join(self.path_to_simulation_files, self.server_dir_name)

    async def get_status(self, empty: Empty) -> Status:
        # TODO: implement this method
        return Status()

    async def create_session(self, empty: Empty) -> Session:
        """
        Creates a key
        makes a directory to store the simulation files
        """
        key = next(self.key_generator)
        self.make_dir(dir_path=self.dir_path(key))

        session = Session(key=key)
        self.sessions.append(session)
        return session

    async def send_config(self, config: Config) -> Empty:
        session = config.session
        key = session.key

        if not os.path.exists(self.dir_path(key)):
            raise ValueError(f"No such simulation with key {key}")

        self.build_simulation_files()
        return Empty()


    async def start_sim(self, session: Session) -> SimulationInfo:
        # TODO: implement this method
        return SimulationInfo()

    @staticmethod
    def dir_name(key: int) -> str:
        return f"simulation_{key}"

    def dir_path(self, key: int) -> str:
        return os.path.join(self.server_dir_path, self.dir_name(key))

    @staticmethod
    def get_key(dir_name: str) -> int:
        return int(dir_name.replace("simulation_", ""))

    @staticmethod
    def make_dir(dir_path: str) -> None:
        if not os.path.exists(dir_path):
            os.mkdir(dir_path)
        else:
            raise ValueError(f"Directory {dir_path} already exists")

    @staticmethod
    def get_server_dir_name():
        now = datetime.now()
        return f"{now.second}{now.minute}{now.hour}{now.day}{now.month}{now.year}"


async def main():
    server = Server([PublicService()])
    await server.start("127.0.0.1", 9510)
    await server.wait_closed()


def start_server():
    loop = asyncio.get_event_loop()
    loop.run_until_complete(main())


if __name__ == '__main__':
    start_server()
