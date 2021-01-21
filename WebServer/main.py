from flask import Flask, request
import time
import sqlite3
import datetime as dt
import pandas as pd

LOCAL_TIMEZONE = dt.datetime.now(dt.timezone(dt.timedelta(0))).astimezone().tzinfo
app = Flask(__name__)


def _open_db_conn():
    conn = sqlite3.connect("data.db")
    conn.execute("PRAGMA foreign_keys = ON")
    return conn


def _build_df_temp_measures(room_id):
    conn = _open_db_conn()
    temp_measures = pd.read_sql_query("SELECT value, timestamp FROM temp_measures WHERE room_id = ?", conn,
                                      params=(room_id,),
                                      index_col="timestamp", parse_dates="timestamp")
    temp_measures = temp_measures.tz_localize('UTC', copy=False).tz_convert(LOCAL_TIMEZONE, copy=False)
    temp_measures.sort_index(inplace=True)
    return temp_measures


def _add_nan_to_holes_in_measure(temp_measures):
    time_compare = pd.DataFrame({"left": temp_measures.index.to_series(),
                                 "right": temp_measures.index.to_series().shift(-1)})
    time_compare = time_compare.iloc[:-1]
    time_compare = time_compare[time_compare.right - time_compare.left > dt.timedelta(minutes=20)]
    time_compare = time_compare.left + (time_compare.right - time_compare.left) / 2
    new_values = pd.DataFrame({"value": None}, index=time_compare)
    new_values.index.name = "timestamp"
    return temp_measures.append(new_values).sort_index()


@app.route("/temp", methods=["POST"])
def add_temp_measure():
    value = float(request.form["value"])
    room_id = int(request.form["room_id"])
    now_ts = int(time.time())

    conn = _open_db_conn()
    curs = conn.cursor()

    curs.execute("INSERT INTO temp_measures (value, room_id, timestamp) VALUES (?, ?, ?)", (value, room_id, now_ts))
    conn.commit()

    return ""


if __name__ == '__main__':
    app.run()
