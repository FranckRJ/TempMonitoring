import datetime as dt

from flask import Flask, request, render_template

from db_access import DbAccess
from home_viewer import HomeViewer
from rooms_data import RoomsData
from rooms_viewer import RoomsViewer
from temps_data import TempsData
from temps_viewer import TempsViewer
from users_data import UsersData
from users_viewer import UsersViewer

DB_FILE_PATH = "data.db"

app = Flask(__name__)
db_access = DbAccess(DB_FILE_PATH)
home_viewer = HomeViewer(render_template)
users_data = UsersData(db_access)
users_viewer = UsersViewer(users_data, render_template)
rooms_data = RoomsData(db_access)
rooms_viewer = RoomsViewer(rooms_data, render_template)
temps_data = TempsData(db_access)
temps_viewer = TempsViewer(temps_data, render_template)


@app.route("/api/rooms/<int:room_id>/temps", methods=["POST"])
def add_temp_measure(room_id: int):
    now_dt = dt.datetime.today()
    value = float(request.form["value"])

    temps_data.add_temp(now_dt, value, room_id)
    temps_viewer.notify_temp_data_updated(room_id)

    return ""


@app.route("/")
def show_home():
    return home_viewer.get_home_page()


@app.route("/users/")
def show_all_users():
    return users_viewer.get_all_users_page()


@app.route("/users/<int:user_id>/")
def show_user(user_id: int):
    return "users_viewer.get_user_page()"


@app.route("/rooms/")
def show_all_rooms():
    return rooms_viewer.get_all_rooms_page()


@app.route("/rooms/<int:room_id>/")
def show_room(room_id: int):
    return rooms_viewer.get_room_page(room_id)


@app.route("/rooms/<int:room_id>/temps/")
def show_room_temps_menu(room_id: int):
    return rooms_viewer.get_room_temps_menu_page(room_id)


@app.route("/rooms/<int:room_id>/temps/all")
def show_all_temps(room_id: int):
    return temps_viewer.get_full_temp_plot(room_id)


@app.route("/rooms/<int:room_id>/temps/lastweek")
def show_last_week_temps(room_id: int):
    return temps_viewer.get_last_week_temp_plot(room_id)


if __name__ == '__main__':
    app.run()
