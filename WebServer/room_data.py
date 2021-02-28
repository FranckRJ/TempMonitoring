from dataclasses import dataclass
from typing import List

from db_access import DbAccess


@dataclass
class Room:
    id: int
    name: str


class RoomData:
    def __init__(self, db_access: DbAccess) -> None:
        self.__db_access = db_access

    def get_all_rooms(self) -> List[Room]:
        with self.__db_access.open_db_conn() as conn:
            curs = conn.cursor()

            curs.execute("SELECT id, name FROM rooms")

            return [Room(id=resp[0], name=resp[1]) for resp in curs.fetchall()]
