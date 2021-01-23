import sqlite3
import os
import datetime as dt
import pandas as pd


class TempData:
    def __init__(self, db_file_path: str) -> None:
        self.db_file_path = db_file_path

    def add_temp_measure(self, timestamp: dt.datetime, value: float, room_id: int) -> None:
        with self._open_db_conn() as conn:
            curs = conn.cursor()

            curs.execute("INSERT INTO temp_measures (timestamp, value, room_id) VALUES (?, ?, ?)",
                         (timestamp, value, room_id))
            conn.commit()

    def get_room_temp_measures(self, room_id: int) -> pd.DataFrame:
        temp_measures = self._build_df_temp_measures(room_id)
        temp_measures_filled = self._add_nan_to_holes_in_measure(temp_measures)

        return temp_measures_filled

    def _open_db_conn(self) -> sqlite3.Connection:
        is_new_db = not os.path.exists(self.db_file_path)

        conn = sqlite3.connect(self.db_file_path)
        conn.execute("PRAGMA foreign_keys = ON")

        if is_new_db:
            with open(self.db_file_path + ".sql") as schema:
                conn.executescript(schema.read())
                conn.commit()

        return conn

    def _build_df_temp_measures(self, room_id: int) -> pd.DataFrame:
        with self._open_db_conn() as conn:
            temp_measures = pd.read_sql_query("SELECT timestamp, value FROM temp_measures WHERE room_id = ?", conn,
                                              params=(room_id,), parse_dates="timestamp")
            temp_measures.sort_index(inplace=True)
            return temp_measures

    @staticmethod
    def _add_nan_to_holes_in_measure(temp_measures: pd.DataFrame) -> pd.DataFrame:
        time_compare = pd.DataFrame({"left": temp_measures.timestamp,
                                     "right": temp_measures.timestamp.shift(-1)})
        time_compare = time_compare.iloc[:-1]
        time_compare = time_compare[time_compare.right - time_compare.left > dt.timedelta(minutes=20)]
        time_compare = time_compare.left + (time_compare.right - time_compare.left) / 2
        new_values = pd.DataFrame({"timestamp": time_compare, "value": None})
        return temp_measures.append(new_values).sort_values("timestamp")
