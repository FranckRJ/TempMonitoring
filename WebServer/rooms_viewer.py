from typing import Callable

from rooms_data import RoomsData


class RoomsViewer:
    def __init__(self, room_data: RoomsData, render_template: Callable[..., str]) -> None:
        self.__room_data = room_data
        self.__render_template = render_template

    def get_all_rooms_page(self) -> str:
        rooms = self.__room_data.get_all_rooms()
        return self.__render_template("all_rooms.html", rooms=rooms)

    def get_room_page(self, room_id: int):
        room = self.__room_data.get_room(room_id)
        return self.__render_template("room.html", room=room)

    def get_room_temps_menu_page(self, room_id: int):
        room = self.__room_data.get_room(room_id)
        return self.__render_template("temps_menu.html", room=room)
