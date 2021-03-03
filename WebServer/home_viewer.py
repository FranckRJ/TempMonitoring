from typing import Callable


class HomeViewer:
    def __init__(self, render_template: Callable[..., str]) -> None:
        self.__render_template = render_template

    def get_home_page(self) -> str:
        return self.__render_template("home.html")
