from django.db import models

# Create your models here.
class State(models.Model):
    emotion = models.IntegerField(default=0)
    number = models.IntegerField(default=0)
    humans = models.IntegerField(default=0)
    slave = models.IntegerField(default=0)
