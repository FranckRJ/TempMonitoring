import datetime as dt

import pandas as pd

from db_access import DbAccess


class TempsData:
    def __init__(self, db_access: DbAccess) -> None:
        self.__db_access = db_access

    def add_temp(self, timestamp: dt.datetime, value: float, room_id: int) -> None:
        with self.__db_access.open_db_conn() as conn:
            curs = conn.cursor()

            curs.execute("INSERT INTO temp_measures (timestamp, value, room_id) VALUES (?, ?, ?)",
                         (timestamp, value, room_id))
            conn.commit()

    def get_room_temps(self, room_id: int, since: dt.datetime = None) -> pd.DataFrame:
        if since is None:
            temps = self.__build_full_temps_df(room_id)
        else:
            temps = self.__build_recent_temps_df(room_id, since)

        temps_filled = self.__add_nan_to_holes_in_measure(temps)

        return temps_filled

    def __build_temps_df_from_query(self, query: str, params: tuple) -> pd.DataFrame:
        with self.__db_access.open_db_conn() as conn:
            temp_measures = pd.read_sql_query(query, conn, params=params, parse_dates="timestamp")
            temp_measures.sort_index(inplace=True)
            return temp_measures

    def __build_full_temps_df(self, room_id: int) -> pd.DataFrame:
        return self.__build_temps_df_from_query("SELECT timestamp, value FROM temp_measures WHERE room_id = ?",
                                                (room_id,))

    def __build_recent_temps_df(self, room_id: int, since: dt.datetime) -> pd.DataFrame:
        return self.__build_temps_df_from_query(
            "SELECT timestamp, value FROM temp_measures WHERE room_id = ? AND timestamp >= ?",
            (room_id, since)
        )

    @staticmethod
    def __add_nan_to_holes_in_measure(temp_measures: pd.DataFrame) -> pd.DataFrame:
        temp_measures.sort_values("timestamp", inplace=True)
        time_compare = pd.DataFrame({"left": temp_measures.timestamp,
                                     "right": temp_measures.timestamp.shift(-1)})
        time_compare = time_compare.iloc[:-1]
        time_compare = time_compare[time_compare.right - time_compare.left > dt.timedelta(minutes=20)]
        time_compare = time_compare.left + (time_compare.right - time_compare.left) / 2
        new_values = pd.DataFrame({"timestamp": time_compare, "value": None})
        temp_measures.append(new_values).sort_values("timestamp", inplace=True)
        return temp_measures
