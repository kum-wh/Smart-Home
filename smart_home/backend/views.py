import urllib
import cv2
# from keras.models import load_model
from django.shortcuts import render
from django.http import HttpResponse, HttpResponseRedirect
from .models import *

# emotion_model = load_model()
# security_model = load_model()

# Create your views here.

def index(request):
    if request.method == "POST":
        number = request.POST.get('rnumber')
        state = State.objects.get(id=0)
        state.number = number
    return render(request, "index.html")


def emotion(request):
    if request.method == "POST":
        link = request.POST.get('image')
        f = urllib.request.urlopen(link)
        data = f.read()
        output = open("emotion.jpg","wb")
        output.write(data)
        output.close()
        image =  cv2.imread("emotion.jpg")
        '''
        result = model.predict(image)
        if result == "anger":
            colour = 1"purple"
        elif result == "sad":
            colour = 2 "blue"
        else:
            colour == 0 "white"
        state = State.objects.get(id=0)
        state.emotion = colour
        '''
    return render(request, "emotion.html")


def security(request):
    if request.method == "POST":
        link = request.POST.get('image')
        f = urllib.request.urlopen(link)
        data = f.read()
        output = open("security.jpg","wb")
        output.write(data)
        output.close()
        image =  cv2.imread("security.jpg")
        '''
        number = model.predict(image)
        state = State.objects.get(id=0)
        rnumber = state.number
        if rnumber > state.number:
            raise flag
        '''
    return render(request, "security.html")


def getemo(request):
    '''
    state = State.objects.get(id=0)
    value = state.emotion
    '''
    value = 0
    return HttpResponse(f'{value}')