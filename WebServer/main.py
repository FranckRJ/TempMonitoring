from flask import Flask, request
from temp_data import TempData
import datetime as dt
import plotly.express as px

DB_FILE_PATH = "data.db"

app = Flask(__name__)
temp_data = TempData(DB_FILE_PATH)


@app.route("/api/temp", methods=["POST"])
def add_temp_measure():
    now_dt = dt.datetime.today()
    value = float(request.form["value"])
    room_id = int(request.form["room_id"])

    temp_data.add_temp_measure(now_dt, value, room_id)

    return ""


@app.route("/temp")
def show_temp():
    config = dict({'showTips': False})

    temp_measures = temp_data.get_room_temp_measures(1)

    fig = px.line(temp_measures, x="timestamp", y="value")
    return fig.to_html(config=config)


if __name__ == '__main__':
    app.run()
