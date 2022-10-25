from django.urls import path
from . import views

urlpatterns = [
    path("", views.index, name="index"),
    path("emotion", views.emotion, name="emotion"),
    path("security", views.security, name="security"),
    path("getemo", views.getemo, name="getemo")
]