import sqlite3
import os
import datetime as dt
import pandas as pd
from typing import Tuple


class TempData:
    def __init__(self, db_file_path: str) -> None:
        self.db_file_path = db_file_path

    def add_temp(self, timestamp: dt.datetime, value: float, room_id: int) -> None:
        with self._open_db_conn() as conn:
            curs = conn.cursor()

            curs.execute("INSERT INTO temp_measures (timestamp, value, room_id) VALUES (?, ?, ?)",
                         (timestamp, value, room_id))
            conn.commit()

    def get_room_temps(self, room_id: int, since: dt.datetime = None) -> pd.DataFrame:
        if since is None:
            temps = self._build_full_temps_df(room_id)
        else:
            temps = self._build_recent_temps_df(room_id, since)

        temps_filled = self._add_nan_to_holes_in_measure(temps)

        return temps_filled

    def _open_db_conn(self) -> sqlite3.Connection:
        is_new_db = not os.path.exists(self.db_file_path)

        conn = sqlite3.connect(self.db_file_path)
        conn.execute("PRAGMA foreign_keys = ON")

        if is_new_db:
            with open(self.db_file_path + ".sql") as schema:
                conn.executescript(schema.read())
                conn.commit()

        return conn

    def _build_temps_df_from_query(self, query: str, params: Tuple) -> pd.DataFrame:
        with self._open_db_conn() as conn:
            temp_measures = pd.read_sql_query(query, conn, params=params, parse_dates="timestamp")
            temp_measures.sort_index(inplace=True)
            return temp_measures

    def _build_full_temps_df(self, room_id: int) -> pd.DataFrame:
        return self._build_temps_df_from_query("SELECT timestamp, value FROM temp_measures WHERE room_id = ?",
                                               (room_id,))

    def _build_recent_temps_df(self, room_id: int, since: dt.datetime) -> pd.DataFrame:
        return self._build_temps_df_from_query(
            "SELECT timestamp, value FROM temp_measures WHERE room_id = ? AND timestamp >= ?",
            (room_id, since)
        )

    @staticmethod
    def _add_nan_to_holes_in_measure(temp_measures: pd.DataFrame) -> pd.DataFrame:
        time_compare = pd.DataFrame({"left": temp_measures.timestamp,
                                     "right": temp_measures.timestamp.shift(-1)})
        time_compare = time_compare.iloc[:-1]
        time_compare = time_compare[time_compare.right - time_compare.left > dt.timedelta(minutes=20)]
        time_compare = time_compare.left + (time_compare.right - time_compare.left) / 2
        new_values = pd.DataFrame({"timestamp": time_compare, "value": None})
        return temp_measures.append(new_values).sort_values("timestamp")
