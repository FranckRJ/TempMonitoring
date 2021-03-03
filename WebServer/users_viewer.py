from typing import Callable

from users_data import UsersData


class UsersViewer:
    def __init__(self, users_data: UsersData, render_template: Callable[..., str]) -> None:
        self.__users_data = users_data
        self.__render_template = render_template

    def get_all_users_page(self) -> str:
        users = self.__users_data.get_all_users()
        return self.__render_template("all_users.html", users=users)

    def get_user_page(self, user_id: int) -> str:
        user = self.__users_data.get_user(user_id)
        return self.__render_template("user.html", user=user)
