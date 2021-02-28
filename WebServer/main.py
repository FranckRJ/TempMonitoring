import datetime as dt

from flask import Flask, request, render_template

from db_access import DbAccess
from room_data import RoomData
from room_viewer import RoomViewer
from temp_data import TempData
from temp_viewer import TempViewer

DB_FILE_PATH = "data.db"

app = Flask(__name__)
db_access = DbAccess(DB_FILE_PATH)
room_data = RoomData(db_access)
room_viewer = RoomViewer(room_data, render_template)
temp_data = TempData(db_access)
temp_viewer = TempViewer(temp_data)


@app.route("/api/rooms/<int:room_id>/temps", methods=["POST"])
def add_temp_measure(room_id: int):
    now_dt = dt.datetime.today()
    value = float(request.form["value"])

    temp_data.add_temp(now_dt, value, room_id)
    temp_viewer.notify_temp_data_updated(room_id)

    return ""


@app.route("/rooms/")
def show_all_rooms():
    return room_viewer.get_all_rooms_page()


@app.route("/rooms/<int:room_id>/temps/all")
def show_all_temps(room_id: int):
    return temp_viewer.get_full_temp_plot(room_id)


@app.route("/rooms/<int:room_id>/temps/lastweek")
def show_last_week_temps(room_id: int):
    return temp_viewer.get_last_week_temp_plot(room_id)


if __name__ == '__main__':
    app.run()
