from dataclasses import dataclass
from typing import List

from db_access import DbAccess


@dataclass
class User:
    id: int
    name: str


class UsersData:
    def __init__(self, db_access: DbAccess) -> None:
        self.__db_access = db_access

    def get_all_users(self) -> List[User]:
        with self.__db_access.open_db_conn() as conn:
            curs = conn.cursor()

            curs.execute("SELECT id, name FROM users")

            return [User(id=resp[0], name=resp[1]) for resp in curs.fetchall()]

    def get_user(self, user_id: int) -> User:
        with self.__db_access.open_db_conn() as conn:
            curs = conn.cursor()

            curs.execute("SELECT id, name FROM users WHERE id = ?", (user_id,))
            resp = curs.fetchone()

            return User(id=resp[0], name=resp[1])
