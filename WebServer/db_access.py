import os
import sqlite3


class DbAccess:
    def __init__(self, db_file_path: str) -> None:
        self.__db_file_path = db_file_path

    def open_db_conn(self) -> sqlite3.Connection:
        is_new_db = not os.path.exists(self.__db_file_path)

        conn = sqlite3.connect(self.__db_file_path)
        conn.execute("PRAGMA foreign_keys = ON")

        if is_new_db:
            with open(self.__db_file_path + ".sql") as schema:
                conn.executescript(schema.read())
                conn.commit()

        return conn
