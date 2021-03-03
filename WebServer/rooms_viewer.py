from typing import Callable

from rooms_data import RoomsData
from users_data import UsersData


class RoomsViewer:
    def __init__(self, users_data: UsersData, rooms_data: RoomsData, render_template: Callable[..., str]) -> None:
        self.__users_data = users_data
        self.__rooms_data = rooms_data
        self.__render_template = render_template

    def get_all_rooms_page(self) -> str:
        rooms = self.__rooms_data.get_all_rooms()
        return self.__render_template("all_rooms.html", rooms=rooms)

    def get_room_page(self, room_id: int) -> str:
        room = self.__rooms_data.get_room(room_id)
        return self.__render_template("room.html", room=room)

    def get_user_rooms_page(self, user_id: int) -> str:
        user = self.__users_data.get_user(user_id)
        rooms = self.__rooms_data.get_user_rooms(user_id)
        return self.__render_template("user_rooms.html", user=user, rooms=rooms)

    def get_room_temps_menu_page(self, room_id: int) -> str:
        room = self.__rooms_data.get_room(room_id)
        return self.__render_template("temps_menu.html", room=room)
