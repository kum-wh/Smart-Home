from django.urls import path
from . import views

urlpatterns = [
    path("", views.index, name="index"),
    path("home", views.home, name="home"),
    path("emotion", views.emotion, name="emotion"),
    path("security", views.security, name="security"),
    path("getemo", views.getemo, name="getemo"),
    path("slave", views.slave, name="slave"),
    path("login", views.login_view, name="login"),
    path("register", views.register, name="register"),
    path("logout", views.logout_view, name="logout")
]
