from flask import Flask, request
from temp_data import TempData
from temp_viewer import TempViewer
import datetime as dt

DB_FILE_PATH = "data.db"

app = Flask(__name__)
temp_data = TempData(DB_FILE_PATH)
temp_viewer = TempViewer(temp_data)


@app.route("/api/temp", methods=["POST"])
def add_temp_measure():
    now_dt = dt.datetime.today()
    value = float(request.form["value"])
    room_id = int(request.form["room_id"])

    temp_data.add_temp_measure(now_dt, value, room_id)
    temp_viewer.notify_temp_data_update(room_id)

    return ""


@app.route("/temp")
def show_temp():
    return temp_viewer.get_full_html_temp_plot(1)


if __name__ == '__main__':
    app.run()
