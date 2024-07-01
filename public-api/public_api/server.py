import asyncio
from typing import Dict

from gen import vopy
from grpclib.server import Server
from grpclib.exceptions import GRPCError
from grpclib.const import Status
import itertools
import os
from datetime import datetime
from dataclasses import dataclass
from helpers import find_free_port
import subprocess


@dataclass
class SessionInfo:
    session: vopy.Session
    is_build_successful: bool = False
    process: subprocess.Popen = None

    @property
    def is_sim_running(self) -> bool:
        return self.process is not None and self.process.poll() is None

    @property
    def is_sim_failed(self) -> bool:
        return self.process is not None and self.process.poll() != 0


class PublicService(vopy.PublicApiStub):
    def __init__(self):
        self.sessions_info_dict: Dict[int, SessionInfo] = {}
        self.key_generator = itertools.count()
        self.path_to_server_workspace = r""  # TODO: set the path to the server workspace
        self.server_dir_name = self.get_server_dir_name()

    async def get_status(self, empty: vopy.Empty) -> Status:
        return vopy.Status(open_simulations=self.get_open_simulations(),
                      running_simulations=self.get_running_simulations(),
                      failed_simulations=self.get_failed_simulations())

    async def create_session(self, empty: vopy.Empty) -> vopy.Session:
        """
        Creates a key
        makes a directory to store the simulation files
        """
        key = next(self.key_generator)
        self.make_dir(dir_path=self.dir_path(key))

        session = vopy.Session(key=key)
        session_info = SessionInfo(session=session)
        self.sessions_info_dict[key] = session_info
        return session

    async def send_config(self, config: vopy.Config) -> vopy.Empty:
        session = config.session
        key = session.key

        self.check_session_exists(key)

        self.build_simulation_files()
        self.sessions_info_dict[key].is_build_successful = True
        return vopy.Empty()

    async def start_sim(self, session: vopy.Session) -> vopy.SimulationInfo:
        self.check_session_exists(session.key)
        self.check_send_config_successful(session.key)
        self.check_simulation_is_not_running(session.key)

        port = find_free_port()

        run_sim_command = "" + port  # TODO: replace with the command to run the simulation

        simulation_session = subprocess.Popen(run_sim_command, shell=True, stdout=subprocess.PIPE,
                                              stderr=subprocess.PIPE)
        self.sessions_info_dict[session.key].process = simulation_session

        return vopy.SimulationInfo(port=port, session=session)

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
            raise GRPCError(status=Status.ALREADY_EXISTS,
                            message=f"There has been an error creating the directory {dir_path}, it already exists.")

    @staticmethod
    def get_server_dir_name():
        now = datetime.now()
        return f"{now.second}{now.minute}{now.hour}{now.day}{now.month}{now.year}"

    def build_simulation_files(self):
        # TODO: implement this method
        pass

    def check_session_exists(self, key: int) -> None:
        if key not in self.sessions_info_dict:
            raise GRPCError(status=Status.FAILED_PRECONDITION,
                            message=f"No such simulation with key {key}")

    def check_send_config_successful(self, key: int) -> None:
        if not self.sessions_info_dict[key].is_build_successful:
            raise GRPCError(status=Status.FAILED_PRECONDITION,
                            message=f"Config was not sent successfully for simulation with key {key}")

    def check_simulation_is_not_running(self, key: int) -> None:
        if self.sessions_info_dict[key].is_sim_running:
            raise GRPCError(status=Status.ALREADY_EXISTS,
                            message=f"Simulation with key {key} is already running")

    @property
    def server_dir_path(self):
        return os.path.join(self.path_to_server_workspace, self.server_dir_name)

    def get_open_simulations(self):
        return len(self.sessions_info_dict.keys())

    def get_running_simulations(self):
        return len([session_info for session_info in self.sessions_info_dict.values() if session_info.is_sim_running])

    def get_failed_simulations(self):
        return len([session_info for session_info in self.sessions_info_dict.values() if session_info.is_sim_failed])


async def main():
    server = Server([PublicService()])
    await server.start("127.0.0.1", 9510)
    await server.wait_closed()


def start_server():
    loop = asyncio.get_event_loop()
    loop.run_until_complete(main())


if __name__ == '__main__':
    start_server()
