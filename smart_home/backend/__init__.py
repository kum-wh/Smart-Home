from django.apps import AppConfig
class MyAppConfig(AppConfig):
    name = 'myapp'
    verbose_name = "My Application"
    def ready(self):
        print("hello")