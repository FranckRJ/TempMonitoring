from dataclasses import dataclass
from typing import List

from db_access import DbAccess


@dataclass
class Room:
    id: int
    name: str


class RoomsData:
    def __init__(self, db_access: DbAccess) -> None:
        self.__db_access = db_access

    def get_all_rooms(self) -> List[Room]:
        with self.__db_access.open_db_conn() as conn:
            curs = conn.cursor()

            curs.execute("SELECT id, name FROM rooms")

            return [Room(id=resp[0], name=resp[1]) for resp in curs.fetchall()]

    def get_user_rooms(self, user_id: int) -> List[Room]:
        with self.__db_access.open_db_conn() as conn:
            curs = conn.cursor()

            curs.execute("SELECT id, name FROM rooms WHERE user_id = ?", (user_id,))

            return [Room(id=resp[0], name=resp[1]) for resp in curs.fetchall()]

    def get_room(self, room_id: int) -> Room:
        with self.__db_access.open_db_conn() as conn:
            curs = conn.cursor()

            curs.execute("SELECT id, name FROM rooms WHERE id = ?", (room_id,))
            resp = curs.fetchone()

            return Room(id=resp[0], name=resp[1])
