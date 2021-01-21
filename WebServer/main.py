from flask import Flask, request
import time
import sqlite3

app = Flask(__name__)


def _open_db_conn():
    conn = sqlite3.connect("data.db")
    conn.execute("PRAGMA foreign_keys = ON")
    return conn


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
