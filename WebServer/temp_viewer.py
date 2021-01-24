from temp_data import TempData
import plotly.express as px


class TempViewer:
    def __init__(self, temp_data: TempData) -> None:
        self.plotly_config = dict({'showTips': False})
        self.temp_plot_cache = {}
        self.temp_data = temp_data
        pass

    def get_full_html_temp_plot(self, room_id: int) -> str:
        html = self.temp_plot_cache.get(room_id, None)

        if html is None:
            self.notify_temp_data_update(room_id)
            html = self.temp_plot_cache[room_id]

        return html

    def notify_temp_data_update(self, room_id: int) -> None:
        temp_measures = self.temp_data.get_room_temp_measures(room_id)

        fig = px.line(temp_measures, x="timestamp", y="value")
        self.temp_plot_cache[room_id] = fig.to_html(config=self.plotly_config)