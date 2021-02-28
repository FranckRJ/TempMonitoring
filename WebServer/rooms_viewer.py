from typing import Callable

from rooms_data import RoomsData


class RoomsViewer:
    def __init__(self, room_data: RoomsData, render_template: Callable[..., str]) -> None:
        self.__room_data = room_data
        self.__render_template = render_template

    def get_all_rooms_page(self) -> str:
        rooms = self.__room_data.get_all_rooms()
        return self.__render_template("rooms.html", rooms=rooms)
