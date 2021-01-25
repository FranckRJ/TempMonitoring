from temp_data import TempData
import datetime as dt
import pandas as pd
import plotly.express as px


class TempViewer:
    def __init__(self, temp_data: TempData) -> None:
        self.plotly_config = dict({"showTips": False, "displaylogo": False})
        self.full_temp_plot_cache = {}
        self.last_week_temp_plot_cache = {}
        self.temp_data = temp_data
        pass

    def get_full_temp_plot(self, room_id: int) -> str:
        html = self.full_temp_plot_cache.get(room_id, None)

        if html is None:
            self._rebuild_full_temp_plot_cache(room_id)
            html = self.full_temp_plot_cache[room_id]

        return html

    def get_last_week_temp_plot(self, room_id: int) -> str:
        now_dt = dt.datetime.today()
        timestamped_html = self.last_week_temp_plot_cache.get(room_id, None)

        if timestamped_html is None or timestamped_html[0] < now_dt - dt.timedelta(hours=1):
            self._rebuild_last_week_temp_plot_cache(room_id)
            timestamped_html = self.last_week_temp_plot_cache[room_id]

        return timestamped_html[1]

    def notify_temp_data_updated(self, room_id: int) -> None:
        self._rebuild_full_temp_plot_cache(room_id)
        self._rebuild_last_week_temp_plot_cache(room_id)

    def _build_temp_plot_from_df(self, temps: pd.DataFrame, title: str) -> str:
        last_temp = temps["value"].iloc[-1]

        fig = px.line(temps, x="timestamp", y="value",
                      labels={"timestamp": "Date", "value": "Température (°C)"},
                      title=title.format(last_temp=last_temp))

        return fig.to_html(config=self.plotly_config)

    def _rebuild_full_temp_plot_cache(self, room_id: int) -> None:
        temps = self.temp_data.get_room_temps(room_id)
        title = "Toutes les températures (actuellement : {last_temp}°C)"
        self.full_temp_plot_cache[room_id] = self._build_temp_plot_from_df(temps, title)

    def _rebuild_last_week_temp_plot_cache(self, room_id: int) -> None:
        now_dt = dt.datetime.today()
        temps = self.temp_data.get_room_temps(room_id, since=now_dt - dt.timedelta(days=7))
        title = "Températures de la semaine (actuellement : {last_temp}°C)"
        self.last_week_temp_plot_cache[room_id] = (now_dt, self._build_temp_plot_from_df(temps, title))
