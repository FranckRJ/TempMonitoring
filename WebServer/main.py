from flask import Flask, request
import sqlite3
import datetime as dt
import pandas as pd

app = Flask(__name__)


def _open_db_conn():
    conn = sqlite3.connect("data.db")
    conn.execute("PRAGMA foreign_keys = ON")
    return conn


def _build_df_temp_measures(room_id):
    with _open_db_conn() as conn:
        temp_measures = pd.read_sql_query("SELECT timestamp, value FROM temp_measures WHERE room_id = ?", conn,
                                          params=(room_id,), parse_dates="timestamp")
        temp_measures.sort_index(inplace=True)
        return temp_measures


def _add_nan_to_holes_in_measure(temp_measures):
    time_compare = pd.DataFrame({"left": temp_measures.timestamp,
                                 "right": temp_measures.timestamp.shift(-1)})
    time_compare = time_compare.iloc[:-1]
    time_compare = time_compare[time_compare.right - time_compare.left > dt.timedelta(minutes=20)]
    time_compare = time_compare.left + (time_compare.right - time_compare.left) / 2
    new_values = pd.DataFrame({"timestamp": time_compare, "value": None})
    return temp_measures.append(new_values).sort_values("timestamp")


@app.route("/api/temp", methods=["POST"])
def add_temp_measure():
    now_dt = dt.datetime.today()
    value = float(request.form["value"])
    room_id = int(request.form["room_id"])

    with _open_db_conn() as conn:
        curs = conn.cursor()

        curs.execute("INSERT INTO temp_measures (timestamp, value, room_id) VALUES (?, ?, ?)", (now_dt, value, room_id))
        conn.commit()

    return ""


if __name__ == '__main__':
    # df = _build_df_temp_measures(1)
    # df_filled = _add_nan_to_holes_in_measure(df)
    app.run()
